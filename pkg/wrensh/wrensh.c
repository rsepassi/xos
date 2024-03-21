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

extern const char* wrensh_src_usage;
extern const char* wrensh_src_io;
extern const char* wrensh_src_user;
extern const char* wrensh_src_meta;
static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

typedef struct Node {
  void* data;
  struct Node* next;
} Node;

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
  Node* garbage;
} Ctx;

void add_garbage(Ctx* ctx, Node* garbage) {
  garbage->next = ctx->garbage;
  ctx->garbage = garbage;
}

void cleanup_garbage(Ctx* ctx) {
  void* data = NULL;
  Node* g = ctx->garbage;
  while (g) {
    data = g->data;
    g = g->next;
    free(data);
  }
  ctx->garbage = NULL;
}

void dbgArgs(int argc, const char* const* argv) {
  DLOG("argc=%d", argc);
  for (int i = 0; i < argc; ++i) {
    DLOG("argv[%d]=%s", i, argv[i]);
  }
}

void wrenshError(WrenVM* vm) {
  DLOG("wrenshError: %s", wrenErrorStr);
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, wrenErrorStr);
  wrenAbortFiber(vm, 0);
}

void wrenWriteFn(WrenVM* vm, const char* text) {
  DLOG("wrenWriteFn");
  fprintf(stderr, "%s", text);
}

void wrenErrorFn(WrenVM* vm, WrenErrorType errorType,
             const char* module, const int line,
             const char* msg) {
  DLOG("wrenErrorFn: %s", msg);
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
  DLOG("wrenshReadAlloc");
  static char readbuf[1 << 16];
  *buf = uv_buf_init(readbuf, sizeof(readbuf));
}

void wrenshReadFinalize(readstate* state) {
  DLOG("wrenshReadFinalize");
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
  DLOG("wrenshReadCb");
  readstate* state = (readstate*)uv_handle_get_data((uv_handle_t*)stream);

  if (nread < 0) {
    uv_read_stop(stream);
    uv_close((uv_handle_t*)stream, NULL);
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
  DLOG("wrenshWriteCb");
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
  char* s = (char*)wrenGetSlotBytes(vm, 2, &len);
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
  DLOG("sleep_cb");
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
  DLOG("wrenshSleep");
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
  DLOG("wrenshArg");
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
  DLOG("wrenshArgs");
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);

  wrenEnsureSlots(vm, 2);
  wrenSetSlotNewList(vm, 0);
  for (int i = 0; i < ctx->argc; ++i) {
    wrenSetSlotString(vm, 1, ctx->argv[i]);
    wrenInsertInList(vm, 0, -1, 1);
  }
}

void wrenshEnv(WrenVM* vm) {
  DLOG("wrenshEnv");
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to IO.env");
  const char* s = wrenGetSlotString(vm, 1);
  const char* val = getenv(s);
  if (val) {
    wrenSetSlotString(vm, 0, val);
  } else {
    wrenSetSlotNull(vm, 0);
  }
}

void wrenshExit(WrenVM* vm) {
  DLOG("wrenshExit");
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to IO.exit");
  int n = (int)wrenGetSlotDouble(vm, 1);
  exit(n);
}

void wrenshGlob(WrenVM* vm) {
  DLOG("wrenshGlob");
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
  DLOG("wrenshExec");
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

  const char** env = NULL;
  bool has_env = wrenGetSlotType(vm, 2) != WREN_TYPE_NULL;
  if (has_env) {
    // with env
    WrenType t = wrenGetSlotType(vm, 2);
    WREN_CHECK(t == WREN_TYPE_LIST, "must pass a list to exec");
    int envc = wrenGetListCount(vm, 2);
    env = malloc((envc + 1) * sizeof(char*));
    env[envc] = NULL;
    for (int i = 0; i < envc; ++i) {
      wrenGetListElement(vm, 2, i, 3);
      const char* warg = wrenGetSlotString(vm, 3);
      env[i] = warg;
    }
  }

  if (env) {
    DLOG("execve");
    dbgArgs(argc, argv);
    const char* val;
    int i = 0;
    while ((val = env[i++])) DLOG("env %s", env[i-1]);
    execve(argv[0], (char* const*)argv, (char* const*)env);
  } else {
    DLOG("execvp");
    dbgArgs(argc, argv);
    execvp(argv[0], (char* const*)argv);
  }

  WREN_CHECK(false, "exec failed");
}

typedef struct {
  WrenHandle* fiber;
  WrenVM* vm;
  uv_process_t handle;
  int64_t exit_status;
  bool return_code;
  bool process_done;

  // If reading from stdout
  uv_pipe_t stdout_pipe;
  char readbuf[1 << 16];
  sds stdout_str;
  bool stdout_done;

  // If redirecting to a file
  int stdout_fd;
  int stderr_fd;

  Node gc;
} runstate;

void wrenshRunFinalize(runstate* state) {
  wrenEnsureSlots(state->vm, 2);
  wrenSetSlotHandle(state->vm, 0, state->fiber);
  Ctx* ctx = (Ctx*)wrenGetUserData(state->vm);
  if (state->exit_status == 0 || state->return_code) {
    if (state->exit_status != 0) {
      // return exit code
      wrenSetSlotDouble(state->vm, 1, state->exit_status);
      QCHECK(wrenCall(state->vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
    } else if (state->stdout_fd == -1) {
      // return stdout
      wrenSetSlotBytes(state->vm, 1, state->stdout_str, sdslen(state->stdout_str));
      QCHECK(wrenCall(state->vm, ctx->wren_tx_val) == WREN_RESULT_SUCCESS);
    } else {
      // return nothing
      QCHECK(wrenCall(state->vm, ctx->wren_tx) == WREN_RESULT_SUCCESS);
    }
  } else {
    sprintf(wrenErrorStr, "process failed with code=%ld", state->exit_status);
    wrenSetSlotString(state->vm, 1, wrenErrorStr);
    QCHECK(wrenCall(state->vm, ctx->wren_tx_err) == WREN_RESULT_SUCCESS);
  }

  wrenReleaseHandle(state->vm, state->fiber);
  sdsfree(state->stdout_str);
  uv_close((uv_handle_t*)&state->handle, NULL);
  if (state->stdout_fd > 0) close(state->stdout_fd);
  if (state->stderr_fd > 0) close(state->stderr_fd);

  add_garbage(ctx, &state->gc);
}

void wrenshRunExitCb(uv_process_t* process, int64_t exit_status, int term_signal) {
  runstate* state = (runstate*)uv_handle_get_data((uv_handle_t*)process);
  DLOG("wrenshRun exit code=%ld state=%p vm=%p", exit_status, state, state->vm);
  state->process_done = true;
  state->exit_status = exit_status;

  if (state->process_done && (state->stdout_fd > 0 || state->stdout_done))
    wrenshRunFinalize(state);

  DLOG("wrenshRunExitCb done");
}

void wrenshRunStdoutAlloc(uv_handle_t* process, size_t suggested_size, uv_buf_t* buf) {
  DLOG("wrenshRunStdoutAlloc");
  runstate* state = (runstate*)uv_handle_get_data(process);
  *buf = uv_buf_init(state->readbuf, sizeof(state->readbuf));
}

void wrenshRunStdoutRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
  DLOG("wrenshRunStdoutRead nread=%zd", nread);
  runstate* state = (runstate*)uv_handle_get_data((uv_handle_t*)stream);
  if (nread < 0) {
    DLOG("wrenshRunStdoutRead done state=%p", state);
    state->stdout_done = true;
    uv_read_stop(stream);
    uv_close((uv_handle_t*)stream, NULL);
    if (state->process_done && (state->stdout_fd > 0 || state->stdout_done))
      wrenshRunFinalize(state);
    return;
  }

  if (nread)
    state->stdout_str = sdscatlen(state->stdout_str, buf->base, nread);
}

void wrenshRun(WrenVM* vm) {
  DLOG("wrenshRun");
  int nargs = wrenGetSlotCount(vm); // IO fiber args [env]
  int scratch_args = 1;
  const int args_i = 2;
  const int env_i = 3;
  const int rc_i = 4;
  const int stdout_i = 5;
  const int stderr_i = 6;

  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);

  // Read args
  WrenType t = wrenGetSlotType(vm, args_i);
  WREN_CHECK(t == WREN_TYPE_LIST, "run args must be a list");
  int argc = wrenGetListCount(vm, args_i);
  const char* args[argc + 1];
  wrenEnsureSlots(vm, nargs + scratch_args);
  for (int i = 0; i < argc; ++i) {
    wrenGetListElement(vm, args_i, i, nargs + 1);
    args[i] = wrenGetSlotString(vm, nargs + 1);
  }
  args[argc] = NULL;
  dbgArgs(argc, args);

  // Read env
  WrenType envt = wrenGetSlotType(vm, env_i);
  bool has_env = envt != WREN_TYPE_NULL;
  char** env = NULL;
  if (has_env) {
    WREN_CHECK(envt == WREN_TYPE_LIST, "run env must be a list");
    int envc = wrenGetListCount(vm, env_i);
    env = malloc(envc * sizeof(env));
    CHECK(env);
    for (int i = 0; i < envc; ++i) {
      wrenGetListElement(vm, env_i, i, nargs + 1);
      env[i] = (char*)wrenGetSlotString(vm, nargs + 1);
    }
    env[envc] = NULL;
  }

  // Redirects
  WrenType stdoutt = wrenGetSlotType(vm, stdout_i);
  bool stdout_redir = stdoutt != WREN_TYPE_NULL;
  WrenType stderrt = wrenGetSlotType(vm, stderr_i);
  bool stderr_redir = stderrt != WREN_TYPE_NULL;
  DLOG("stdout_redir=%d stderr_redir=%d", stdout_redir, stderr_redir);

  // Alloc runstate
  const runstate state_ = {.vm = vm, .fiber = fiber, .stdout_str = sdsempty(), .stdout_fd = -1, .stderr_fd = -1 };
  runstate* state = malloc(sizeof(runstate));
  CHECK(state);
  *state = state_;
  state->gc.data = state;

  state->return_code = wrenGetSlotBool(vm, rc_i);

  // Setup stdio
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_stdio_container_t stdio[3];
  stdio[0].flags = UV_IGNORE;
  if (stdout_redir) {
    if (wrenGetSlotType(vm, stdout_i) == WREN_TYPE_STRING) {
      FILE* f = fopen(wrenGetSlotString(vm, stdout_i), "w");
      WREN_CHECK(f, "stdout redirect bad file");
      state->stdout_fd = fileno(f);
    } else {
      state->stdout_fd = (int)wrenGetSlotDouble(vm, stdout_i);
      WREN_CHECK(state->stdout_fd <= 2, "stdout redirect bad fileno");
    }
    stdio[1].flags = UV_INHERIT_FD;
    stdio[1].data.fd = state->stdout_fd;
  } else {
    stdio[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    stdio[1].data.stream = (uv_stream_t*)&state->stdout_pipe;
    uv_pipe_init(ctx->loop, &state->stdout_pipe, 0);
    uv_handle_set_data((uv_handle_t*)&state->stdout_pipe, state);
  }
  if (stderr_redir) {
    if (wrenGetSlotType(vm, stderr_i) == WREN_TYPE_STRING) {
      FILE* f = fopen(wrenGetSlotString(vm, stderr_i), "w");
      WREN_CHECK(f, "stderr redirect bad file");
      state->stderr_fd = fileno(f);
    } else {
      state->stderr_fd = (int)wrenGetSlotDouble(vm, stderr_i);
      WREN_CHECK(state->stderr_fd <= 2, "stderr redirect bad fileno");
    }
    stdio[2].flags = UV_INHERIT_FD;
    stdio[2].data.fd = state->stderr_fd;
  } else {
    stdio[2].flags = UV_INHERIT_FD;
    stdio[2].data.fd = STDERR_FILENO;
  }

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
  if (!stdout_redir) {
    UV_CHECK(uv_read_start((uv_stream_t*)&state->stdout_pipe, wrenshRunStdoutAlloc, wrenshRunStdoutRead))
  }
}

void wrenshChdir(WrenVM* vm) {
  DLOG("wrenshChdir");
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "chdir arg must be a string");
  const char* path = wrenGetSlotString(vm, 1);
  WREN_CHECK(chdir(path) == 0, "unable to change directory to %s", path);
}

void wrenshCwd(WrenVM* vm) {
  DLOG("wrenshCwd");
  char path[maxpathlen];
  WREN_CHECK(getcwd(path, sizeof(path)), "could not get cwd");
  wrenEnsureSlots(vm, 1);
  wrenSetSlotString(vm, 0, path);
}

typedef struct {
  uv_signal_t handle;
  WrenVM* vm;
  WrenHandle* fn;
  bool cancelled;
} trapstate;

void trapCb(uv_signal_t* handle, int signal) {
  DLOG("trapCb");
  trapstate* state = (trapstate*)uv_handle_get_data((uv_handle_t*)handle);
  wrenEnsureSlots(state->vm, 1);
  wrenSetSlotHandle(state->vm, 0, state->fn);
  Ctx* ctx = (Ctx*)wrenGetUserData(state->vm);
  QCHECK(wrenCall(state->vm, ctx->wren_call) == WREN_RESULT_SUCCESS);
}

void trapAlloc(WrenVM* vm) {
  DLOG("trapAlloc");
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "trap signal must be an integer");
  int signal = (int)wrenGetSlotDouble(vm, 1);
  WrenHandle* fn = wrenGetSlotHandle(vm, 2);
  trapstate* state = (trapstate*)wrenSetSlotNewForeign(
      vm, 0, 0, sizeof(trapstate));

  state->vm = vm;
  state->fn = fn;
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  uv_signal_init(ctx->loop, &state->handle);
  uv_signal_start_oneshot(&state->handle, trapCb, signal);
}

void trapFinal(void* data) {
  DLOG("trapFinal");
  trapstate* state = (trapstate*)data;
  if (!state->cancelled) {
    uv_signal_stop(&state->handle);
    uv_close((uv_handle_t*)&state->handle, NULL);
  }
}

void trapCancel(WrenVM* vm) {
  DLOG("trapCancel");
  void* data = wrenGetSlotForeign(vm, 0);
  trapstate* state = (trapstate*)data;
  trapFinal(data);
  state->cancelled = true;
}

WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className) {
  CHECK(!strcmp(module, "io"), "unexpected foreign class");
  WrenForeignClassMethods m;
  if (!strcmp(className, "Trap")) {
    m.allocate = trapAlloc;
    m.finalize = trapFinal;
  }
  return m;
}

extern void wrenshEnvMap(WrenVM* vm);

extern char* wrenMetaSource();
extern WrenForeignMethodFn wrenMetaBindForeignMethod(
    WrenVM* vm,
    const char* className,
    bool isStatic,
    const char* signature);

WrenForeignMethodFn bindForeignMethod(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature) {
  CHECK(!strcmp(module, "io"), "unexpected foreign method");

  // Trap
  if (!strcmp(className, "Trap")
      && !isStatic
      && !strcmp(signature, "cancel()")) return trapCancel;

  WrenForeignMethodFn meta = wrenMetaBindForeignMethod(vm, className, isStatic, signature);
  if (meta != NULL) return meta;

  // IO
  CHECK(!strcmp(className, "IO") &&
        isStatic, "unexpected foreign method");
  if (!strcmp(signature, "write_(_,_)")) return wrenshWrite;
  if (!strcmp(signature, "flush()")) return wrenshFlush;
  if (!strcmp(signature, "read_(_)")) return wrenshRead;
  if (!strcmp(signature, "sleep_(_,_)")) return wrenshSleep;
  if (!strcmp(signature, "arg(_)")) return wrenshArg;
  if (!strcmp(signature, "argc()")) return wrenshArgc;
  if (!strcmp(signature, "args()")) return wrenshArgs;
  if (!strcmp(signature, "env()")) return wrenshEnvMap;
  if (!strcmp(signature, "env(_)")) return wrenshEnv;
  if (!strcmp(signature, "exit(_)")) return wrenshExit;
  if (!strcmp(signature, "exec_(_,_)")) return wrenshExec;
  if (!strcmp(signature, "glob(_)")) return wrenshGlob;
  if (!strcmp(signature, "glob(_,_)")) return wrenshGlob;
  if (!strcmp(signature, "run_(_,_,_,_,_,_)")) return wrenshRun;
  if (!strcmp(signature, "chdir(_)")) return wrenshChdir;
  if (!strcmp(signature, "cwd()")) return wrenshCwd;

  CHECK(false, "unexpected foreign method: %s", signature);
}

WrenVM* setupWren(Ctx* ctx) {
  DLOG("setupWren");
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.writeFn = wrenWriteFn;
  config.errorFn = wrenErrorFn;
  config.bindForeignMethodFn = bindForeignMethod;
  config.bindForeignClassFn = bindForeignClass;
  config.userData = (void*)ctx;
  WrenVM* vm = wrenNewVM(&config);
  CHECK(vm != NULL, "could not initialize wren");

  ctx->wren_tx_val = wrenMakeCallHandle(vm, "transfer(_)");
  ctx->wren_tx_err = wrenMakeCallHandle(vm, "transferError(_)");
  ctx->wren_tx = wrenMakeCallHandle(vm, "transfer()");
  ctx->wren_call = wrenMakeCallHandle(vm, "call()");
  ctx->wren_call_val = wrenMakeCallHandle(vm, "call(_)");

  CHECK(wrenInterpret(vm, "io", wrenMetaSource()) == WREN_RESULT_SUCCESS, "bad meta src");
  CHECK(wrenInterpret(vm, "io", wrensh_src_io) == WREN_RESULT_SUCCESS, "bad io src");
  CHECK(wrenInterpret(vm, "main", "import \"io\" for IO, X, Data") == WREN_RESULT_SUCCESS);

  return vm;
}

void cleanupWren(WrenVM* vm) {
  DLOG("cleanupWren");
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);

  wrenReleaseHandle(vm, ctx->wren_tx_val);
  wrenReleaseHandle(vm, ctx->wren_tx_err);
  wrenReleaseHandle(vm, ctx->wren_tx);
  wrenReleaseHandle(vm, ctx->wren_call);
  wrenReleaseHandle(vm, ctx->wren_call_val);

  wrenFreeVM(vm);
}

char* readFile(char* path) {
  DLOG("readFile");
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

void uv_cleanup_cb(uv_handle_t *handle, void *arg) {
  DLOG("uv_cleanup_cb type=%s", uv_handle_type_name(handle->type));
  uv_close(handle, 0);
}

void cleanupUV(Ctx* ctx) {
  DLOG("cleanupUV");
  uv_close((uv_handle_t*)&ctx->stdout_pipe, NULL);
  uv_close((uv_handle_t*)&ctx->stdin_pipe, NULL);
  uv_run(ctx->loop, UV_RUN_DEFAULT);
  uv_walk(ctx->loop, uv_cleanup_cb, 0);
  uv_run(ctx->loop, UV_RUN_DEFAULT);
  uv_loop_close(ctx->loop);
  DLOG("cleanupUV done");
}

void tickerCb(uv_timer_t* handle) {
  Ctx* ctx = (Ctx*)uv_handle_get_data((uv_handle_t*)handle);
  cleanup_garbage(ctx);
}

int main(int argc, char** argv) {
  DLOG("wrensh main");
  dbgArgs(argc, (const char* const*)argv);
  bool has_user_src = wrensh_src_user != NULL;
  DLOG("has_user_src %d", has_user_src);

  if (!has_user_src) {
    // usage
    if (argc == 1 ||
        (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
      fputs(wrensh_src_usage, stderr);
      exit(1);
    }
  }

  // read src
  char* user_src;
  bool file_src = false;
  if (has_user_src) {
    user_src = (char*)wrensh_src_user;
  } else if (argc > 2 && !strcmp(argv[1], "-c")) {
    user_src = argv[2];
  } else {
    user_src = readFile(argv[1]);
    file_src = true;
  }

  // uv setup
  uv_loop_t loop_;
  uv_loop_t* loop = &loop_;
  uv_loop_init(loop);
  uv_disable_stdio_inheritance();
  DLOG("uv setup");

  // context
  Ctx ctx = {.argc = argc, .argv = argv, .loop = loop};

  // wren setup
  WrenVM* wren = setupWren(&ctx);
  DLOG("wren setup vm=%p", wren);

  // setup std{in,out}
  UV_CHECK(uv_pipe_init(loop, &ctx.stdin_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdin_pipe, STDIN_FILENO));
  UV_CHECK(uv_pipe_init(loop, &ctx.stdout_pipe, 0));
  UV_CHECK(uv_pipe_open(&ctx.stdout_pipe, STDOUT_FILENO));
  DLOG("stdio setup");

  // setup ticker (garbage collection)
  uv_timer_t ticker;
  uv_timer_init(loop, &ticker);
  uv_handle_set_data((uv_handle_t*)&ticker, &ctx);
  uv_unref((uv_handle_t*)&ticker);
  uv_timer_start(&ticker, tickerCb, 0, 1000);

  // user script
  int res = wrenInterpret(wren, "main", user_src);
  DLOG("user script run, code=%d", res);
  if (res != WREN_RESULT_SUCCESS) return res;

  // io loop run
  DLOG("uv loop start");
  int live = 1;
  while (live) {
    DLOG("uv loop tick %lu", uv_now(loop));
    live = uv_run(loop, UV_RUN_ONCE);
  }
  uv_timer_stop(&ticker);
  uv_close((uv_handle_t*)&ticker, NULL);
  DLOG("uv loop done, exiting");

  // Cleanup
  if (file_src) free(user_src);
  cleanupWren(wren);
  cleanupUV(&ctx);
  cleanup_garbage(&ctx);

  return 0;
}

// TODO:
// * read(n), readln
// * Kill process, process signals
// * Streams
// * Redirection >>
// * -x print all commands
// * Loading/running other scripts
