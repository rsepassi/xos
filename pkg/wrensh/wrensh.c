#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "uv.h"
#include "wren.h"
#include "xglob.h"
#include "sds.h"

#define LOG_HELPER(prefix, fmt, ...) do { \
	fprintf(stderr, "[" prefix " %s:%d] " fmt "%s\n", &__FILE__[0], __LINE__, __VA_ARGS__); \
	} while(0)
#define LOG(...) LOG_HELPER("info", __VA_ARGS__, "")

#define CHECK(cond, ...) do { \
    if (!(cond)) { \
      LOG_HELPER("error", __VA_ARGS__, ""); \
      exit(1); \
    } \
  } while(0)
#define QCHECK(cond, ...) do { \
    if (!(cond)) { \
      exit(1); \
    } \
  } while(0)


#ifdef DEBUG
#define DLOG(...) LOG_HELPER("dbg", __VA_ARGS__, "")
#define DCHECK(...) CHECK(__VA_ARGS__)
#else
#define DLOG(...)
#define DCHECK(...)
#endif

#define WREN_CHECK_HELPER(cond, fmt, ...) do { \
    if (!(cond)) { \
      sprintf(wrenErrorStr, fmt "%s", __VA_ARGS__); \
      wrenshError(vm); \
      return; \
    } \
  } while(0)
#define WREN_CHECK(cond, ...) WREN_CHECK_HELPER(cond, __VA_ARGS__, "");

#define UV_CHECK_HELPER(cond, fmt, ...) do { \
    int res = cond; \
    CHECK(res == 0, fmt "%s %s", __VA_ARGS__, uv_strerror(res)); \
  } while(0)
#define UV_CHECK(cond, ...) UV_CHECK_HELPER(cond, __VA_ARGS__, "");

#define PTR_SIZE 8
#define PTR_INT_T uint64_t

static char* usage_str;
static const char* io_src;
static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

typedef struct {
  uv_signal_t handle;
  WrenVM* vm;
  WrenHandle* fn;
} signalstate;

typedef struct {
  int argc;
  char** argv;
  uv_loop_t* loop;
  uv_pipe_t stdin_pipe;
  uv_pipe_t stdout_pipe;
  WrenHandle* wren_tx_val;
  WrenHandle* wren_tx_err;
  WrenHandle* wren_tx;
  WrenHandle* wren_call;
  WrenHandle* wren_call_val;
  signalstate signals[8];
  int signalc;
} Ctx;

void dbgArgs(int argc, char** argv) {
  DLOG("argc=%d", argc);
  for (int i = 0; i < argc; ++i) {
    DLOG("argv[%d]=%s", i, argv[i]);
  }
}

void* ptrFromBytes(const char* b) {
  PTR_INT_T p;
  memcpy(&p, b, PTR_SIZE);
  return (void*)p;
}

void wrenshError(WrenVM* vm) {
  wrenEnsureSlots(vm, 1);
  wrenSetSlotBytes(vm, 0, wrenErrorStr, strlen(wrenErrorStr));
  wrenAbortFiber(vm, 0);
}

void wrenWriteFn(WrenVM* vm, const char* text) {
  fprintf(stderr, "%s", text);
}

void wrenErrorFn(WrenVM* vm, WrenErrorType errorType,
             const char* module, const int line,
             const char* msg) {
  switch (errorType) {
    case WREN_ERROR_COMPILE:
    {
      fprintf(stderr, "[%s line %d] [Error] %s\n", module, line, msg);
    } break;
    case WREN_ERROR_STACK_TRACE:
    {
      fprintf(stderr, "[%s line %d] in %s\n", module, line, msg);
    } break;
    case WREN_ERROR_RUNTIME:
    {
      fprintf(stderr, "[Runtime Error] %s\n", msg);
    } break;
  }
}

typedef struct {
  WrenVM* vm;
  WrenHandle* fiber;
  sds str;
} readstate;

void wrenshReadAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
  static char readbuf[1 << 16];
  *buf = uv_buf_init(readbuf, sizeof(readbuf));
}

void wrenshReadFinalize(readstate* state) {
  WrenVM* vm = state->vm;
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, state->fiber);
  wrenSetSlotBytes(vm, 1, state->str, sdslen(state->str));
  QCHECK(wrenCall(vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
  wrenReleaseHandle(vm, state->fiber);
  sdsfree(state->str);
  free(state);
}

void wrenshReadCb(uv_stream_t *stream, ssize_t nread, const uv_buf_t* buf) {
  readstate* state = (readstate*)uv_handle_get_data((uv_handle_t*)stream);

  if (nread < 0) {
    uv_read_stop(stream);
    wrenshReadFinalize(state);
    return;
  }
  state->str = sdscatlen(state->str, buf->base, nread);
}

void wrenshRead(WrenVM* vm) {
  DLOG("wrenshRead");
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);

  const readstate s2 = {
    .vm = vm,
    .fiber = fiber,
    .str = sdsempty(),
  };
  readstate* state = malloc(sizeof(readstate));
  CHECK(state);
  *state = s2;

  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_handle_set_data((uv_handle_t*)&ctx->stdin_pipe, state);
  UV_CHECK(uv_read_start((uv_stream_t*)&ctx->stdin_pipe, wrenshReadAlloc, wrenshReadCb));
}

typedef struct {
  WrenVM* vm;
  uv_write_t req;
  WrenHandle* fiber;
  WrenHandle* str;
  uv_buf_t bufs[1];
} writestate;

void wrenshWriteCb(uv_write_t* req, int status) {
  writestate* state = (writestate*)uv_req_get_data((uv_req_t*)req);

  WrenVM* vm = state->vm;
  wrenEnsureSlots(vm, 1);

  if (status == 0) {
    Ctx* ctx = (Ctx*)wrenGetUserData(vm);
    wrenSetSlotHandle(vm, 0, state->fiber);
    QCHECK(wrenCall(vm, ctx->wren_tx) == WREN_RESULT_SUCCESS);
  } else {
    sprintf(wrenErrorStr, "write failed code=%d", status);
    wrenshError(vm);
  }

  wrenReleaseHandle(vm, state->fiber);
  wrenReleaseHandle(vm, state->str);
  free(state);
}

void wrenshWrite(WrenVM* vm) {
  DLOG("wrenshWrite");
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);
  WrenType t = wrenGetSlotType(vm, 2);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to IO.write");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 2, &len);
  WrenHandle* wrenstr = wrenGetSlotHandle(vm, 2);

  const writestate s2 = {
    .vm = vm,
    .fiber = fiber,
    .str = wrenstr,
    .bufs = {uv_buf_init(s, len)},
  };
  writestate* state = malloc(sizeof(writestate));
  CHECK(state);
  *state = s2;

  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_req_set_data((uv_req_t*)&state->req, state);
  UV_CHECK(uv_write(&state->req, (uv_stream_t*)&ctx->stdout_pipe, state->bufs, 1, wrenshWriteCb));
}

typedef struct {
  WrenHandle* fiber;
  WrenVM* vm;
  uv_timer_t handle;
} timerstate;

void sleep_cb(uv_timer_t* handle) {
  timerstate* state = (timerstate*)uv_handle_get_data((uv_handle_t*)handle);
  wrenEnsureSlots(state->vm, 1);
  wrenSetSlotHandle(state->vm, 0, state->fiber);
  Ctx* ctx = (Ctx*)wrenGetUserData(state->vm);
  QCHECK(wrenCall(state->vm, ctx->wren_tx) == WREN_RESULT_SUCCESS);

  uv_timer_stop(&state->handle);
  wrenReleaseHandle(state->vm, state->fiber);
  free(state);
}

void wrenshSleep(WrenVM* vm) {
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);
  WrenType t = wrenGetSlotType(vm, 2);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to IO.sleep");
  int n = (int)wrenGetSlotDouble(vm, 2);
  DLOG("wrenshSleep n=%d", n);

  const timerstate state_ = {
    .fiber = fiber,
    .vm = vm,
  };
  timerstate* state = malloc(sizeof(timerstate));
  CHECK(state);
  *state = state_;

  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  UV_CHECK(uv_timer_init(ctx->loop, &state->handle));
  UV_CHECK(uv_timer_start(&state->handle, sleep_cb, n, 0));
}

void wrenshFlush(WrenVM* vm) {
  DLOG("wrenshFlush");
  fflush(stdout);
}

void wrenshArg(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to IO.arg");
  int n = (int)wrenGetSlotDouble(vm, 1);
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  WREN_CHECK(n < ctx->argc, "only %d args", ctx->argc);
  wrenSetSlotBytes(vm, 0, ctx->argv[n], strlen(ctx->argv[n]));
}

void wrenshArgc(WrenVM* vm) {
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenSetSlotDouble(vm, 0, ctx->argc);
}

void wrenshArgs(WrenVM* vm) {
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);

  wrenEnsureSlots(vm, 2);
  wrenSetSlotNewList(vm, 0);
  for (int i = 0; i < ctx->argc; ++i) {
    wrenSetSlotString(vm, 1, ctx->argv[i]);
    wrenInsertInList(vm, 0, -1, 1);
  }
}

void wrenshEnv(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to IO.env");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  const char* val = getenv(s);
  wrenSetSlotBytes(vm, 0, val, strlen(val));
}

void wrenshExit(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to IO.exit");
  int n = (int)wrenGetSlotDouble(vm, 1);
  exit(n);
}

int findexe(const char* exe, char* exepath) {
  int exelen = strlen(exe);
  if (exe[0] == '/' || exe[0] == '\\') {
    // absolute path
    memcpy(exepath, exe, exelen);
    return 0;
  }

  char* path = getenv("PATH");
  if (!path) {
    sprintf(wrenErrorStr, "no PATH set");
    return -1;
  }
  int pathlen = strlen(path);

  for (char *z = path, *p = path; *z != '\0'; p = z+1) {
    z = strchr(p, ':');
    if (!z) z = &path[pathlen];

    int segmentlen = z-p;
    memcpy(exepath, p, segmentlen);
    exepath[segmentlen] = '/';
    memcpy(exepath+segmentlen+1, exe, exelen);
    exepath[segmentlen + exelen + 1] = '\0';

    if (access(exepath, F_OK) == 0) return 0;
  }

  sprintf(wrenErrorStr, "could not find %s in PATH", exe);
  return -1;
}

void wrenshGlob(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to glob");
  const char* pattern = wrenGetSlotString(vm, 1);
  int nslots = wrenGetSlotCount(vm);
  char path[maxpathlen];
  CHECK(getcwd(path, sizeof(path)));
  if (nslots == 3) {
    t = wrenGetSlotType(vm, 2);
    WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to glob");
    int len = 0;
    const char* dir = wrenGetSlotBytes(vm, 2, &len);
    memcpy(path, dir, len);
    path[len] = '\0';
  }
  xglob_ctx* ctx = xglob_init(pattern, path);
  WREN_CHECK(ctx, "error initializing glob at directory %s", path);

  wrenEnsureSlots(vm, 2);
  wrenSetSlotNewList(vm, 0);
  char* match;
  while ((match = xglob_next(ctx))) {
    wrenSetSlotString(vm, 1, match);
    wrenInsertInList(vm, 0, -1, 1);
  }
  xglob_free(ctx);
}

void wrenshExec(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_LIST, "must pass a list to exec");
  int nslots = wrenGetSlotCount(vm);

  wrenEnsureSlots(vm, 4);
  int argc = wrenGetListCount(vm, 1);
  const char** argv = malloc((argc + 1) * sizeof(char*));
  CHECK(argv != NULL, "malloc failed");
  argv[argc] = NULL;
  for (int i = 0; i < argc; ++i) {
    wrenGetListElement(vm, 1, i, 3);
    const char* warg = wrenGetSlotString(vm, 3);
    argv[i] = warg;
  }

  // path lookup
  char path[maxpathlen];
  if (findexe(argv[0], (char*)path) != 0) {
    wrenshError(vm);
    return;
  }

  if (nslots == 3) {
    // with env
    WrenType t = wrenGetSlotType(vm, 2);
    WREN_CHECK(t == WREN_TYPE_LIST, "must pass a list to exec");
    int argc = wrenGetListCount(vm, 2);
    const char** env = malloc((argc + 1) * sizeof(char*));
    env[argc] = NULL;
    for (int i = 0; i < argc; ++i) {
      wrenGetListElement(vm, 2, i, 3);
      const char* warg = wrenGetSlotString(vm, 3);
      env[i] = warg;
    }
    execve(path, argv, env);
  } else {
    execvp(path, argv);
  }

  WREN_CHECK(false, "exec failed");
}

typedef struct {
  WrenHandle* fiber;
  WrenVM* vm;
  uv_pipe_t stdout_pipe;
  uv_process_t handle;
  char readbuf[1 << 16];
  sds stdout_str;
} runstate;

void wrenshRunExitCb(uv_process_t* process, int64_t exit_status, int term_signal) {
  runstate* state = (runstate*)uv_handle_get_data((uv_handle_t*)process);
  DLOG("wrenshRun exit code=%d ps_ptr=%p vm=%p", exit_status, state, state->vm);

  wrenEnsureSlots(state->vm, 2);
  wrenSetSlotHandle(state->vm, 0, state->fiber);
  Ctx* ctx = (Ctx*)wrenGetUserData(state->vm);
  if (exit_status == 0) {
    wrenSetSlotBytes(state->vm, 1, state->stdout_str, sdslen(state->stdout_str));
    QCHECK(wrenCall(state->vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
  } else {
    sprintf(wrenErrorStr, "process failed with code=%lld", exit_status);
    wrenSetSlotString(state->vm, 1, wrenErrorStr);
    QCHECK(wrenCall(state->vm, ctx->wren_tx_err) == WREN_RESULT_SUCCESS);
  }

  wrenReleaseHandle(state->vm, state->fiber);
  uv_close((uv_handle_t*)&state->stdout_pipe, NULL);
  sdsfree(state->stdout_str);
  free(state);
}

void wrenshRunStdoutAlloc(uv_handle_t* process, size_t suggested_size, uv_buf_t* buf) {
  runstate* state = (runstate*)uv_handle_get_data(process);
  *buf = uv_buf_init(state->readbuf, sizeof(state->readbuf));
}

void wrenshRunStdoutRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  runstate* state = (runstate*)uv_handle_get_data((uv_handle_t*)stream);
  if (nread < 0) {
    DLOG("stdout done ps_ptr=%p", state);
    return;
  }

  state->stdout_str = sdscatlen(state->stdout_str, buf->base, nread);
}

void wrenshRun(WrenVM* vm) {
  DLOG("wrenshRun");
  int nargs = wrenGetSlotCount(vm); // IO fiber args [env]
  int scratch_args = 1;

  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);

  // Read args
  WrenType t = wrenGetSlotType(vm, 2);
  WREN_CHECK(t == WREN_TYPE_LIST, "run args must be a list");
  int argc = wrenGetListCount(vm, 2);
  const char* args[argc + 1];
  wrenEnsureSlots(vm, nargs + scratch_args);
  for (int i = 0; i < argc; ++i) {
    wrenGetListElement(vm, 2, i, nargs + 1);
    args[i] = wrenGetSlotString(vm, nargs + 1);
  }
  args[argc] = NULL;
  dbgArgs(argc, (char**)args);

  // Read env
  char** env = NULL;
  if (nargs == 4) {
    WrenType t = wrenGetSlotType(vm, 3);
    WREN_CHECK(t == WREN_TYPE_LIST, "run env must be a list");
    int envc = wrenGetListCount(vm, 3);
    env = malloc(envc * sizeof(env));
    CHECK(env);
    for (int i = 0; i < envc; ++i) {
      wrenGetListElement(vm, 3, i, nargs + 1);
      env[i] = wrenGetSlotString(vm, nargs + 1);
    }
    env[envc] = NULL;
  }

  // Alloc runstate
  const runstate state_ = {.vm = vm, .fiber = fiber, .stdout_str = sdsempty()};
  runstate* state = malloc(sizeof(runstate));
  CHECK(state);
  *state = state_;

  // Setup stdio
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_stdio_container_t stdio[3];
  stdio[0].flags = UV_IGNORE;
  stdio[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
  stdio[1].data.stream = (uv_stream_t*)&state->stdout_pipe;
  uv_pipe_init(ctx->loop, &state->stdout_pipe, 0);
  uv_handle_set_data((uv_handle_t*)&state->stdout_pipe, state);
  stdio[2].flags = UV_INHERIT_FD;
  stdio[2].data.fd = STDERR_FILENO;
  DCHECK(state->vm == vm);

  // Setup process options
  uv_process_options_t opts = {0};
  opts.file = args[0];
  opts.args = (char**)args;
  opts.env = env;
  opts.cwd = NULL;
  opts.stdio_count = 3;
  opts.stdio = stdio;
  opts.exit_cb = wrenshRunExitCb;

  // Spawn
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  int res = uv_spawn(ctx->loop, &state->handle, &opts);
  WREN_CHECK(res == 0, "process spawn failed args[0]=%s: %s", opts.file, uv_strerror(res));
  if (env != NULL) free(env);

  // Start stdout read
  UV_CHECK(uv_read_start((uv_stream_t*)&state->stdout_pipe, wrenshRunStdoutAlloc, wrenshRunStdoutRead))
}

void wrenshChdir(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "chdir arg must be a string");
  const char* path = wrenGetSlotString(vm, 1);
  WREN_CHECK(chdir(path) == 0, "unable to change directory to %s", path);
}

void wrenshCwd(WrenVM* vm) {
  char path[maxpathlen];
  WREN_CHECK(getcwd(path, sizeof(path)), "could not get cwd");
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, path);
}

void wrenshSignalCb(uv_signal_t *handle, int signum) {
  signalstate* state = (signalstate*)uv_handle_get_data((uv_handle_t*)handle);

  wrenEnsureSlots(state->vm, 2);
  wrenSetSlotHandle(state->vm, 0, state->fn);
  wrenSetSlotDouble(state->vm, 1, signum);

  Ctx* ctx = (Ctx*)wrenGetUserData(state->vm);
  QCHECK(wrenCall(state->vm, ctx->wren_call_val) == WREN_RESULT_SUCCESS);
  wrenReleaseHandle(state->vm, state->fn);
}

void wrenshTrapCancel(WrenVM* vm) {
  int len = 0;
  const char* b = wrenGetSlotBytes(vm, 1, &len);
  signalstate* state = (signalstate*)ptrFromBytes(b);
  uv_signal_stop(&state->handle);
  wrenReleaseHandle(state->vm, state->fn);
}

void wrenshTrap(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "trap signal must be an integer");
  int signal = (int)wrenGetSlotDouble(vm, 1);
  WrenHandle* fn = wrenGetSlotHandle(vm, 2);
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  WREN_CHECK(ctx->signalc < (sizeof(ctx->signals) - 1), "maximum number of signals trapped");
  signalstate* state = &ctx->signals[ctx->signalc++];
  state->vm = vm;
  state->fn = fn;
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  uv_signal_init(ctx->loop, &state->handle);
  uv_signal_start_oneshot(&state->handle, wrenshSignalCb, signal);

  wrenSetSlotBytes(vm, 0, (char*)state, PTR_SIZE);
}

WrenForeignMethodFn bindForeignMethod(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature) {
  CHECK(!strcmp(module, "io") &&
        !strcmp(className, "IO") &&
        isStatic, "unexpected foreign method");
  if (!strcmp(signature, "write_(_,_)")) return wrenshWrite;
  if (!strcmp(signature, "flush()")) return wrenshFlush;
  if (!strcmp(signature, "read_(_)")) return wrenshRead;
  if (!strcmp(signature, "sleep_(_,_)")) return wrenshSleep;
  if (!strcmp(signature, "arg(_)")) return wrenshArg;
  if (!strcmp(signature, "argc()")) return wrenshArgc;
  if (!strcmp(signature, "args()")) return wrenshArgs;
  if (!strcmp(signature, "env(_)")) return wrenshEnv;
  if (!strcmp(signature, "exit(_)")) return wrenshExit;
  if (!strcmp(signature, "exec(_)")) return wrenshExec;
  if (!strcmp(signature, "exec(_,_)")) return wrenshExec;
  if (!strcmp(signature, "glob(_)")) return wrenshGlob;
  if (!strcmp(signature, "glob(_,_)")) return wrenshGlob;
  if (!strcmp(signature, "run_(_,_)")) return wrenshRun;
  if (!strcmp(signature, "run_(_,_,_)")) return wrenshRun;
  if (!strcmp(signature, "chdir(_)")) return wrenshChdir;
  if (!strcmp(signature, "cwd()")) return wrenshCwd;
  if (!strcmp(signature, "trap_(_,_)")) return wrenshTrap;
  if (!strcmp(signature, "trap_cancel_(_)")) return wrenshTrapCancel;
  fprintf(stderr, "unexpected foreign method %s\n", signature);
  exit(1);
}

WrenVM* setupWren(Ctx* ctx) {
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.writeFn = wrenWriteFn;
  config.errorFn = wrenErrorFn;
  config.bindForeignMethodFn = bindForeignMethod;
  config.userData = (void*)ctx;
  WrenVM* vm = wrenNewVM(&config);
  CHECK(vm != NULL, "could not initialize wren");

  ctx->wren_tx_val = wrenMakeCallHandle(vm, "transfer(_)");
  ctx->wren_tx_err = wrenMakeCallHandle(vm, "transferError(_)");
  ctx->wren_tx = wrenMakeCallHandle(vm, "transfer()");
  ctx->wren_call = wrenMakeCallHandle(vm, "call()");
  ctx->wren_call_val = wrenMakeCallHandle(vm, "call(_)");

  CHECK(wrenInterpret(vm, "io", io_src) == WREN_RESULT_SUCCESS, "bad io src");
  CHECK(wrenInterpret(vm, "main", "import \"io\" for IO") == WREN_RESULT_SUCCESS);

  return vm;
}

typedef struct {
  WrenVM* vm;
  WrenHandle* fiber;
  uv_prepare_t handle;
} preparestate;

void wrenshPrepareCb(uv_prepare_t *handle) {
  DLOG("wrenshPrepare");
  preparestate* state = (preparestate*)uv_handle_get_data((uv_handle_t*)handle);
  WrenVM* vm = state->vm;
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotHandle(vm, 0, state->fiber);
  QCHECK(wrenCall(vm, ctx->wren_tx) == WREN_RESULT_SUCCESS);
}

char* readFile(char* path) {
  FILE *f = fopen(path, "rb");
  CHECK(f, "could not open file %s", path);
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc(fsize + 1);
  CHECK(buf, "malloc failed");
  fread(buf, fsize, 1, f);
  fclose(f);
  buf[fsize] = 0;
  return buf;
}

int main(int argc, char** argv) {
  dbgArgs(argc, argv);
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
    fputs(usage_str, stderr);
    exit(1);
  }

  char* user_src;
  bool file_src = false;
  if (argc > 2 && !strcmp(argv[1], "-c")) {
    user_src = argv[2];
  } else {
    user_src = readFile(argv[1]);
    file_src = true;
  }

  // uv setup
  uv_loop_t loop;
  uv_loop_init(&loop);
  uv_disable_stdio_inheritance();
  DLOG("uv setup");

  // process context
  Ctx ctx = {.argc = argc, .argv = argv, .loop = &loop};

  // wren setup
  WrenVM* wren = setupWren(&ctx);
  DLOG("wren setup vm=%p", wren);

  // setup std{in,out}
  UV_CHECK(uv_pipe_init(&loop, &ctx.stdin_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdin_pipe, STDIN_FILENO));
  UV_CHECK(uv_pipe_init(&loop, &ctx.stdout_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdout_pipe, STDOUT_FILENO));
  DLOG("stdio setup");

  // user script
  int res = wrenInterpret(wren, "main", user_src);
  DLOG("user script run, code=%d", res);
  if (res != WREN_RESULT_SUCCESS) return res;

  // io loop run
  DLOG("uv loop start");
  uv_run(&loop, UV_RUN_DEFAULT);
  DLOG("uv loop done, exiting");

  // Clean shutdown. Skipped since the OS will clean up for us.
  // if (file_src) free(user_src);
  // wrenFreeVM(wren);
  // uv_close(&ctx.stdin_pipe, NULL)
  // uv_close(&ctx.stdout_pipe, NULL)
  // uv_loop_close(&loop);

  return 0;
}

static char* usage_str = \@@WRENSHUSAGE@@;
static const char* io_src = \@@IOWREN@@;

// TODO:
// * read(n), readln
// * Background process, job control
// * Redirection
// * -x print all commands
// * Loading/running other scripts
