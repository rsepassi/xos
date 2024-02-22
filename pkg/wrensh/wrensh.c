#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "uv.h"
#include "wren.h"

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

static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

typedef struct {
  int argc;
  char** argv;
  uv_loop_t* loop;
  uv_pipe_t stdin_pipe;
  uv_pipe_t stdout_pipe;
  WrenHandle* wren_tx_val;
  WrenHandle* wren_tx;
} Ctx;

char* dbgArgs(int argc, char** argv) {
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
  wrenSetSlotBytes(vm, 0, wrenErrorStr, strlen(wrenErrorStr));
  wrenAbortFiber(vm, 0);
}

void wrenWriteFn(WrenVM* vm, const char* text) {
  fprintf(stderr, "%s", text);
}

void wrenErrorFn(WrenVM* vm, WrenErrorType errorType,
             const char* module, const int line,
             const char* msg)
{
  switch (errorType)
  {
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
  int n;
  char* buf;
  int bufcap;
  int buflen;
} readstate;

void wrenshReadAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
  static char readbuf[4096];
  *buf = uv_buf_init(readbuf, sizeof(readbuf));
}

void wrenshReadFinalize(readstate* state) {
  WrenVM* vm = state->vm;
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, state->fiber);
  wrenSetSlotBytes(vm, 1, state->buf, state->buflen);
  CHECK(wrenCall(vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
  wrenReleaseHandle(vm, state->fiber);
  free(state->buf);
  free(state);
}

void wrenshReadCb(uv_stream_t *stream, ssize_t nread, const uv_buf_t* buf) {
  readstate* state = (readstate*)uv_handle_get_data(stream);

  if (nread < 0) {
    uv_read_stop(stream);
    wrenshReadFinalize(state);
    return;
  }

  if (state->bufcap < (state->buflen + nread)) {
    int cap = (state->bufcap || 2048) * 2;
    state->buf = realloc(state->buf, cap);
    state->bufcap = cap;
  }

  memcpy(state->buf + state->buflen, buf->base, nread);
  state->buflen += nread;
}

void wrenshRead(WrenVM* vm) {
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);

  const readstate s2 = {
    .vm = vm,
    .fiber = fiber,
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
  writestate* state = (writestate*)uv_req_get_data(req);

  WrenVM* vm = state->vm;
  wrenEnsureSlots(vm, 1);

  if (status == 0) {
    Ctx* ctx = (Ctx*)wrenGetUserData(vm);
    wrenSetSlotHandle(vm, 0, state->fiber);
    CHECK(wrenCall(vm, ctx->wren_tx) == WREN_RESULT_SUCCESS);
  } else {
    sprintf(wrenErrorStr, "write failed code=%d", status);
    wrenshError(vm);
  }

  wrenReleaseHandle(vm, state->fiber);
  wrenReleaseHandle(vm, state->str);
  free(state);
}

void wrenshWrite(WrenVM* vm) {
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
  uv_req_set_data(&state->req, state);
  UV_CHECK(uv_write(&state->req, (uv_stream_t*)&ctx->stdout_pipe, state->bufs, 1, wrenshWriteCb));
}

void wrenshFlush(WrenVM* vm) {
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

  // path lookup using "which"
  char path[maxpathlen];
  if (findexe(argv[0], &path) != 0) {
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
  WrenVM* vm;
  WrenHandle* fiber;
  bool exited;
  int64_t exit_status;
  uv_process_t handle;
  uv_pipe_t stdin_pipe;
  uv_pipe_t stdout_pipe;
} processstate;

void wrenshProcessReturn(processstate* state) {
  DLOG("wrenshProcess return");
  DCHECK(state->exited);
  DCHECK(state->fiber);
  WrenVM* vm = state->vm;
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenEnsureSlots(vm, 2);
  wrenSetSlotHandle(vm, 0, state->fiber);
  wrenSetSlotDouble(vm, 1, state->exit_status);
  DLOG("fiber=%p", state->fiber);
  CHECK(wrenCall(vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
  wrenReleaseHandle(vm, state->fiber);

  // TODO: stdout?
  // close streams?
  // should be freed when SpawnedProcess is garbage-collected
  // foreign class
  free(state);
}

// wait - exit - return works
// exit - wait - return doesn't

void wrenshProcessExitCb(uv_process_t* process, int64_t exit_status, int term_signal) {
  processstate* state = (processstate*)uv_handle_get_data((uv_handle_t*)process);
  DLOG("wrenshProcess exit ps_ptr=%p vm=%p", state, state->vm);
  state->exited = true;
  state->exit_status = exit_status;
  if (state->fiber != NULL) wrenshProcessReturn(state);
}

void wrenshProcessWait(WrenVM* vm) {
  DLOG("wrenshProcess wait");
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);

  int len = 0;
  const char* b = wrenGetSlotBytes(vm, 2, &len);
  CHECK(len == PTR_SIZE);
  processstate* state = (processstate*)ptrFromBytes(b);
  DCHECK(state->vm == vm);
  DLOG("ps_ptr=%p fiber=%p", state, fiber);

  state->fiber = fiber;
  if (state->exited) wrenshProcessReturn(state);
}

void wrenshProcess(WrenVM* vm) {
  DLOG("wrenshProcess");
  int nargs = 3;
  int scratch_args = 1;

  // Read args
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_LIST, "process args must be a list");
  int argc = wrenGetListCount(vm, 1);
  wrenEnsureSlots(vm, nargs + scratch_args);
  const char* args[argc + 1];
  for (int i = 0; i < argc; ++i) {
    wrenGetListElement(vm, 1, i, nargs + 1);
    args[i] = wrenGetSlotString(vm, nargs + 1);
  }
  args[argc] = NULL;
  dbgArgs(argc, (char**)args);

  // Alloc processstate
  const processstate s2 = {.vm = vm};
  processstate* state = malloc(sizeof(processstate));
  CHECK(state);
  *state = s2;
  DLOG("ps_ptr=%p", state);

  // Setup stdio
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_stdio_container_t stdio[3];
  stdio[0].flags = UV_CREATE_PIPE | UV_READABLE_PIPE;
  stdio[0].data.stream = (uv_stream_t*)&state->stdin_pipe;
  uv_pipe_init(ctx->loop, &state->stdin_pipe, 0);
  stdio[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
  stdio[1].data.stream = (uv_stream_t*)&state->stdout_pipe;
  uv_pipe_init(ctx->loop, &state->stdout_pipe, 0);
  stdio[2].flags = UV_INHERIT_FD;
  stdio[2].data.fd = 2;
  DCHECK(state->vm == vm);

  // Setup process options
  uv_process_options_t opts = {0};
  opts.file = args[0];
  opts.args = (char**)args;
  opts.env = NULL;
  opts.cwd = NULL;
  opts.stdio_count = 3;
  opts.stdio = stdio;
  opts.exit_cb = wrenshProcessExitCb;

  // Spawn
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  int res = uv_spawn(ctx->loop, &state->handle, &opts);
  WREN_CHECK(res == 0, "process spawn failed args[0]=%s: %s", opts.file, uv_strerror(res));

  // Return processstate pointer
  wrenSetSlotBytes(vm, 0, (char*)&state, PTR_SIZE);
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
  if (!strcmp(signature, "arg(_)")) return wrenshArg;
  if (!strcmp(signature, "argc()")) return wrenshArgc;
  if (!strcmp(signature, "env(_)")) return wrenshEnv;
  if (!strcmp(signature, "exit(_)")) return wrenshExit;
  if (!strcmp(signature, "exec(_)")) return wrenshExec;
  if (!strcmp(signature, "exec(_,_)")) return wrenshExec;
  if (!strcmp(signature, "process_(_,_,_)")) return wrenshProcess;
  if (!strcmp(signature, "process_wait_(_,_)")) return wrenshProcessWait;
  fprintf(stderr, "unexpected foreign method %s\n", signature);
  exit(1);
}

WrenVM* setupWren(void* userdata) {
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.writeFn = wrenWriteFn;
  config.errorFn = wrenErrorFn;
  config.bindForeignMethodFn = bindForeignMethod;
  config.userData = userdata;
  WrenVM* vm = wrenNewVM(&config);
  CHECK(vm != NULL, "could not initialize wren");
  return vm;
}

void usage() {
  static char* usage_str = \@@WRENSHUSAGE@@;
  fputs(usage_str, stderr);
}

static const char* io_src = \@@IOWREN@@;

int main(int argc, char** argv) {
  dbgArgs(argc - 1, argv);
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
    usage();
    exit(1);
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
  CHECK(wrenInterpret(wren, "io", io_src) == WREN_RESULT_SUCCESS, "bad io src");
  CHECK(wrenInterpret(wren, "main", "import \"io\" for IO") == WREN_RESULT_SUCCESS);
  ctx.wren_tx_val = wrenMakeCallHandle(wren, "transfer(_)");
  ctx.wren_tx = wrenMakeCallHandle(wren, "transfer()");
  DLOG("wren setup vm=%p", wren);

  // setup std{in,out}
  UV_CHECK(uv_pipe_init(&loop, &ctx.stdin_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdin_pipe, STDIN_FILENO));
  UV_CHECK(uv_pipe_init(&loop, &ctx.stdout_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdout_pipe, STDOUT_FILENO));
  DLOG("stdio setup");

  // user script
  char* user_src = argv[argc - 1];
  int res = wrenInterpret(wren, "main", user_src);
  DLOG("user script run");
  if (res != WREN_RESULT_SUCCESS) return res;

  // io loop run
  DLOG("uv loop start");
  uv_run(&loop, UV_RUN_DEFAULT);
  DLOG("uv loop done, exiting");

  // Clean shutdown. Skipped since the OS will clean up for us.
  // wrenFreeVM(wren);
  // uv_close(&ctx.stdin_pipe, NULL)
  // uv_close(&ctx.stdout_pipe, NULL)
  // uv_loop_close(&loop);
}

// TODO:
// * Enable process stdin to be passed in (stdout of another process)
// * Detached process
// * Nonblock pipe
// * read(n) on stdin and from process stdout
// * readl() readline on stdin and from process stdout
// * Expose term_signal
// * Log timestamps
