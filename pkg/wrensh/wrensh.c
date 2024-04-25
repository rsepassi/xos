#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "uv.h"
#include "wren.h"
#include "xglob.h"
#include "sds.h"

#include "wrensh.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_HELPER(prefix, fmt, ...) do { \
	fprintf(stderr, "[" prefix " %s:%d] " fmt "%s\n", &__FILENAME__[0], __LINE__, __VA_ARGS__); \
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

static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

extern Ctx* wrenshGetCtx(WrenVM*);

extern void wrenshArgs(WrenVM*);
extern void wrenshArg(WrenVM*);
extern void wrenshEnvMap(WrenVM* vm);

typedef union {
  uv_pipe_t pipe;
  uv_tty_t tty;
} uv_stdio_stream_t;

typedef struct {
  uv_loop_t* loop;
  uv_fs_t req;
  uv_file file;
} intpipe_state;

typedef struct {
  uv_stdio_stream_t stdin_stream;
  uv_stdio_stream_t stdout_stream;
  uv_pipe_t intpipe_read;
  intpipe_state intpipe_state;
} stdio_state;

void dbgEnv(const char** env) {
#ifdef DEBUG
  const char* val;
  int i = 0;
  while ((val = env[i++])) LOG("- %s", val);
#endif
}

void dbgArgs(int argc, const char* const* argv) {
#ifdef DEBUG
  DLOG("argc=%d", argc);
  for (int i = 0; i < argc; ++i) {
    DLOG("argv[%d]=%s", i, argv[i]);
  }
#endif
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
  Ctx* ctx = wrenshGetCtx(vm);
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
    DLOG("wrenshReadCb done");
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

  Ctx* ctx = wrenshGetCtx(vm);
  stdio_state* stdio = (stdio_state*)ctx->stdio;
  uv_handle_set_data((uv_handle_t*)&stdio->stdin_stream, state);
  UV_CHECK(uv_read_start((uv_stream_t*)&stdio->stdin_stream, wrenshReadAlloc, wrenshReadCb));
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
    Ctx* ctx = wrenshGetCtx(vm);
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

  Ctx* ctx = wrenshGetCtx(vm);
  uv_req_set_data((uv_req_t*)&state->req, state);
  stdio_state* stdio = (stdio_state*)ctx->stdio;
  UV_CHECK(uv_write(&state->req, (uv_stream_t*)&stdio->stdout_stream, state->bufs, 1, wrenshWriteCb));
}

void wrenshFlush(WrenVM* vm) {
  DLOG("wrenshFlush");
  fflush(stdout);
}

void wrenshArgc(WrenVM* vm) {
  Ctx* ctx = wrenshGetCtx(vm);
  wrenSetSlotDouble(vm, 0, ctx->argc);
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
    dbgEnv(env);
    execve(argv[0], (char* const*)argv, (char* const*)env);
  } else {
    DLOG("execvp");
    dbgArgs(argc, argv);
    execvp(argv[0], (char* const*)argv);
  }

  WREN_CHECK(false, "exec failed");
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
  uv_fs_event_t handle;
  WrenVM* vm;
  WrenHandle* fn;
  bool cancelled;
} watchstate;

void fswatcherCb(uv_fs_event_t* handle, const char* filename, int events, int status) {
  watchstate* state = (watchstate*)uv_handle_get_data((uv_handle_t*)handle);
  wrenEnsureSlots(state->vm, 3);
  // fn(filename, events)
  wrenSetSlotHandle(state->vm, 0, state->fn);
  if (filename) {
    wrenSetSlotString(state->vm, 1, filename);
  } else {
    wrenSetSlotNull(state->vm, 1);
  }
  wrenSetSlotDouble(state->vm, 2, events);
  Ctx* ctx = wrenshGetCtx(state->vm);
  QCHECK(wrenCall(state->vm, ctx->wren_call2_val) == WREN_RESULT_SUCCESS);
}

void fswatcherCancel(WrenVM* vm) {
  DLOG("fswatcherCancel");

  watchstate* state = (watchstate*)wrenGetSlotForeign(vm, 0);
  if (!state->cancelled) UV_CHECK(uv_fs_event_stop(&state->handle));
}

void fswatcherAlloc(WrenVM* vm) {
  DLOG("fswatcherAlloc");

  const char* path = wrenGetSlotString(vm, 1);
  WrenHandle* fn = wrenGetSlotHandle(vm, 2);

  Ctx* ctx = wrenshGetCtx(vm);

  watchstate* state = (watchstate*)wrenSetSlotNewForeign(
      vm, 0, 0, sizeof(watchstate));
  state->vm = vm;
  state->fn = fn;
  state->cancelled = false;
  UV_CHECK(uv_fs_event_init(ctx->loop, &state->handle));
  uv_handle_set_data((uv_handle_t*)&state->handle, state);
  UV_CHECK(uv_fs_event_start(&state->handle, fswatcherCb, path, UV_FS_EVENT_RECURSIVE));
}

void fswatcherFinal(void* data) {
  DLOG("fswatcherFinal");
  watchstate* state = (watchstate*)data;
  wrenReleaseHandle(state->vm, state->fn);
  if (!state->cancelled) UV_CHECK(uv_fs_event_stop(&state->handle));
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
  Ctx* ctx = wrenshGetCtx(state->vm);
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
  Ctx* ctx = wrenshGetCtx(vm);
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
  DLOG("uv_cleanup_cb type=%s(%d)", uv_handle_type_name(handle->type), handle->type);
  if (handle->type != UV_UNKNOWN_HANDLE) uv_close(handle, 0);
}

void cleanupUV(Ctx* ctx) {
  DLOG("cleanupUV");
  uv_close((uv_handle_t*)&((stdio_state*)ctx->stdio)->stdout_stream, NULL);
  uv_close((uv_handle_t*)&((stdio_state*)ctx->stdio)->stdin_stream, NULL);
  uv_run(ctx->loop, UV_RUN_DEFAULT);
  // TODO: sometimes segfaults, not really an issue bc we're exiting, but would
  // be nice to have a clean shutdown.
  // uv_walk(ctx->loop, uv_cleanup_cb, 0);
  uv_run(ctx->loop, UV_RUN_DEFAULT);
  uv_loop_close(ctx->loop);
  DLOG("cleanupUV done");
}

void intpipeAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
  DLOG("intpipeAlloc");
  static char readbuf[1 << 16];
  *buf = uv_buf_init(readbuf, sizeof(readbuf));
}

void dummyFsWriteCb(uv_fs_t *req) {}

void intpipeRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t* buf) {
  DLOG("intpipeRead");
  intpipe_state* state = (intpipe_state*)uv_handle_get_data((uv_handle_t*)stream);

  if (nread < 0) {
    DLOG("intpipeRead done");
    uv_read_stop(stream);
    uv_close((uv_handle_t*)stream, NULL);
    return;
  }

  UV_CHECK(uv_fs_write(state->loop, &state->req, state->file, buf, 1, -1, dummyFsWriteCb));
}

bool cBindForeignClass(
    WrenVM* vm, const char* module, const char* className, WrenForeignClassMethods* m) {
  if (!strcmp(className, "Trap")) {
    m->allocate = trapAlloc;
    m->finalize = trapFinal;
    return true;
  }
  if (!strcmp(className, "Watcher")) {
    m->allocate = fswatcherAlloc;
    m->finalize = fswatcherFinal;
    return true;
  }

  return false;
}

WrenForeignMethodFn cBindForeignMethod(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature) {
  // Trap
  if (!strcmp(className, "Trap")
      && !isStatic
      && !strcmp(signature, "cancel()")) return trapCancel;

  // Watcher
  if (!strcmp(className, "Watcher")
      && !isStatic
      && !strcmp(signature, "cancel()")) return fswatcherCancel;

  // IO
  if (!strcmp(className, "IO") && isStatic) {
    if (!strcmp(signature, "write_(_,_)")) return wrenshWrite;
    if (!strcmp(signature, "flush()")) return wrenshFlush;
    if (!strcmp(signature, "read_(_)")) return wrenshRead;
    if (!strcmp(signature, "arg(_)")) return wrenshArg;
    if (!strcmp(signature, "argc()")) return wrenshArgc;
    if (!strcmp(signature, "args()")) return wrenshArgs;
    if (!strcmp(signature, "env()")) return wrenshEnvMap;
    if (!strcmp(signature, "env(_)")) return wrenshEnv;
    if (!strcmp(signature, "exit(_)")) return wrenshExit;
    if (!strcmp(signature, "exec_(_,_)")) return wrenshExec;
    if (!strcmp(signature, "glob(_)")) return wrenshGlob;
    if (!strcmp(signature, "glob(_,_)")) return wrenshGlob;
    if (!strcmp(signature, "chdir(_)")) return wrenshChdir;
    if (!strcmp(signature, "cwd()")) return wrenshCwd;
  }

  return NULL;
}

void cleanupStdio(void* state) {
  DLOG("cleanupStdio");
  free((stdio_state*)state);
}

void* setupStdio(uv_loop_t* loop) {
  stdio_state* ctx = malloc(sizeof(stdio_state));
  uv_disable_stdio_inheritance();

  // setup std{in,out}
  uv_handle_type stdin_type = uv_guess_handle(fileno(stdin));
  uv_handle_type stdout_type = uv_guess_handle(fileno(stdout));
  DLOG("uv stdin=%s stdout=%s", uv_handle_type_name(stdin_type), uv_handle_type_name(stdout_type));

  uv_stdio_stream_t* stdin_stream = &ctx->stdin_stream;
  uv_stdio_stream_t* stdout_stream = &ctx->stdout_stream;

  if (stdin_type == UV_TTY) {
    UV_CHECK(uv_tty_init(loop, &stdin_stream->tty, fileno(stdin), 0));
  } else {
    if (stdin_type != UV_NAMED_PIPE) {
      DLOG("treating stdin as pipe but type=%s", uv_handle_type_name(stdin_type));
    }
    UV_CHECK(uv_pipe_init(loop, &stdin_stream->pipe, 0));
    UV_CHECK(uv_pipe_open(&stdin_stream->pipe, fileno(stdin)));
  }

  // Used only if stdout is a file
  // we'll read from this pipe and on data we'll write to the actual file
  uv_pipe_t* intpipe_read = &ctx->intpipe_read;
  intpipe_state* intpipe_state_ = &ctx->intpipe_state;
  if (stdout_type == UV_FILE) {
    uv_file fds[2];
    UV_CHECK(uv_pipe(fds, UV_NONBLOCK_PIPE, UV_NONBLOCK_PIPE));

    UV_CHECK(uv_pipe_init(loop, intpipe_read, 0));
    UV_CHECK(uv_pipe_open(intpipe_read, fds[0]));
    intpipe_state_->loop = loop;
    intpipe_state_->file = fileno(stdout);
    uv_handle_set_data((uv_handle_t*)intpipe_read, intpipe_state_);
    UV_CHECK(uv_read_start((uv_stream_t*)intpipe_read, intpipeAlloc, intpipeRead));
    uv_unref((uv_handle_t*)intpipe_read);

    UV_CHECK(uv_pipe_init(loop, &stdout_stream->pipe, 0));
    UV_CHECK(uv_pipe_open(&stdout_stream->pipe, fds[1]));
  } else if (stdout_type == UV_NAMED_PIPE) {
    UV_CHECK(uv_pipe_init(loop, &stdout_stream->pipe, 0));
    UV_CHECK(uv_pipe_open(&stdout_stream->pipe, fileno(stdout)));
  } else if (stdout_type == UV_TTY) {
    UV_CHECK(uv_tty_init(loop, &stdout_stream->tty, fileno(stdout), 0));
  } else {
    CHECK(false, "unsupported stdout type %s", uv_handle_type_name(stdout_type));
  }

  DLOG("stdio setup");
  return ctx;
}
