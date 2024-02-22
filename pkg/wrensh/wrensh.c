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

#define WREN_CHECK_HELPER(cond, fmt, ...) do { \
    if (!(cond)) { \
      sprintf(wrenErrorStr, fmt "%s", __VA_ARGS__); \
      wrenshError(vm); \
      return; \
    } \
  } while(0)
#define WREN_CHECK(cond, ...) WREN_CHECK_HELPER(cond, __VA_ARGS__, "");

static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

typedef struct {
  int argc;
  char** argv;
  uv_loop_t* loop;
  uv_pipe_t stdin_pipe;
  uv_pipe_t stdout_pipe;
  uv_pipe_t stderr_pipe;
  WrenHandle* wren_tx_val;
  WrenHandle* wren_tx;
} Ctx;

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
  wrenCall(vm, ctx->wren_tx_val);
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
  CHECK(uv_read_start((uv_stream_t*)&ctx->stdin_pipe, wrenshReadAlloc, wrenshReadCb) == 0);
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
    wrenCall(vm, ctx->wren_tx);
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
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to io.write");
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
  CHECK(uv_write(&state->req, (uv_stream_t*)&ctx->stdout_pipe, state->bufs, 1, wrenshWriteCb) == 0);
}

void wrenshFlush(WrenVM* vm) {
  fflush(stdout);
}

void wrenshArg(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.arg");
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
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to io.env");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  const char* val = getenv(s);
  wrenSetSlotBytes(vm, 0, val, strlen(val));
}

void wrenshExit(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.exit");
  int n = (int)wrenGetSlotDouble(vm, 1);
  exit(n);
}

int findexe(char* exe, char* exepath) {
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

  for (char *z = path, *p = path; *z != NULL; p = z+1) {
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


WrenForeignMethodFn bindForeignMethod(
    WrenVM* vm,
    const char* module,
    const char* className,
    bool isStatic,
    const char* signature) {
  CHECK(!strcmp(module, "io") &&
        !strcmp(className, "io") &&
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
  static char* usage_str = \
    "  wrensh command line utility\n"
    "    wrensh [args...] wren_src\n"
    "  \n"
    "  https://wren.io\n"
    "  \n"
    "  io.read(): read stdin in full\n"
    "  io.write(s): write to stdout\n"
    "  io.flush(): flush stdout\n"
    "  io.arg(i): read args[i]\n"
    "  io.argc(): num args\n"
    "  io.env(name): read env var\n"
    "  io.exit(c): exit with code c\n"
    "  io.exec(argv): replace current process with argv\n"
    "  io.exec(argv, env): replace current process with argv and env\n";
  fputs(usage_str, stderr);
}

static const char* io_src = \
  "class io {\n"
  "  static write(s) {\n"
  "    write_(Fiber.current, s)\n"
  "    Fiber.yield()\n"
  "  }\n"
  "  static read() {\n"
  "    read_(Fiber.current)\n"
  "    return Fiber.yield()\n"
  "  }\n"
  "  foreign static flush()\n"
  "  foreign static arg(i)\n"
  "  foreign static argc()\n"
  "  foreign static env(name)\n"
  "  foreign static exit(c)\n"
  "  foreign static exec(argv)\n"
  "  foreign static exec(argv, env)\n"
  "  foreign static read_(f)\n"
  "  foreign static write_(f, s)\n"
  "}\n";

int main(int argc, char** argv) {
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
    usage();
    exit(1);
  }

  // uv setup
  uv_loop_t loop;
  uv_loop_init(&loop);

  // process context
  Ctx ctx = {.argc = argc, .argv = argv, .loop = &loop};

  // wren setup
  WrenVM* wren = setupWren(&ctx);
  CHECK(wrenInterpret(wren, "io", io_src) == WREN_RESULT_SUCCESS, "bad io src");
  CHECK(wrenInterpret(wren, "main", "import \"io\" for io") == WREN_RESULT_SUCCESS);
  ctx.wren_tx_val = wrenMakeCallHandle(wren, "transfer(_)");
  ctx.wren_tx = wrenMakeCallHandle(wren, "transfer()");

  // setup std{in,out,err}
  CHECK(uv_pipe_init(&loop, &ctx.stdin_pipe, 0) == 0);
  CHECK(uv_pipe_open(&ctx.stdin_pipe, STDIN_FILENO) == 0);
  CHECK(uv_pipe_init(&loop, &ctx.stdout_pipe, 0) == 0);
  CHECK(uv_pipe_open(&ctx.stdout_pipe, STDOUT_FILENO) == 0);
  CHECK(uv_pipe_init(&loop, &ctx.stderr_pipe, 0) == 0);
  CHECK(uv_pipe_open(&ctx.stderr_pipe, STDERR_FILENO) == 0);

  // user script
  char* user_src = argv[argc - 1];
  int res = wrenInterpret(wren, "main", user_src);
  if (res != WREN_RESULT_SUCCESS) return res;

  // io loop run
  uv_run(&loop, UV_RUN_DEFAULT);

  // Clean shutdown. Skipped since the OS will clean up for us.
  // wrenFreeVM(wren);
  // uv_loop_close(&loop);
}
