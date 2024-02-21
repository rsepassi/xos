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
      wrenError(vm); \
      return; \
    } \
  } while(0)
#define WREN_CHECK(cond, ...) \
  WREN_CHECK_HELPER(cond, __VA_ARGS__, "");


typedef struct {
  int argc;
  char** argv;
  uv_loop_t* loop;
} Ctx;

static const int maxpathlen = 4096;
static char wrenErrorStr[4096];

void wrenError(WrenVM* vm) {
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

// Read stdin in full
void wrenRead(WrenVM* vm) {
  int sz = 1024 * 8;
  char* buf = malloc(sz);
  int c;
  int i = 0;
  while ((c = getchar()) != EOF) {
    buf[i++] = c;
    if (i >= sz) {
      sz *= 2;
      buf = realloc(buf, sz);
      CHECK(buf != NULL, "could not allocate %d bytes for stdin", sz);
    }
  }
  buf[i] = '\0';
  wrenSetSlotBytes(vm, 0, buf, i);
  free(buf);
}

void wrenReadN(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.read(n)");
  int n = (int)wrenGetSlotDouble(vm, 1);
  char* buf = malloc(n);
  CHECK(buf != NULL, "could not allocate %d bytes for stdin", n);
  int nread = fread(buf, 1, n, stdin);
  wrenSetSlotBytes(vm, 0, buf, nread);
  free(buf);
}

void wrenWrite(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to io.write");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  fprintf(stdout, "%.*s", len, s);
}

void wrenFlush(WrenVM* vm) {
  fflush(stdout);
}

void wrenArg(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.arg");
  int n = (int)wrenGetSlotDouble(vm, 1);
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  WREN_CHECK(n < ctx->argc, "only %d args", ctx->argc);
  wrenSetSlotBytes(vm, 0, ctx->argv[n], strlen(ctx->argv[n]));
}

void wrenArgc(WrenVM* vm) {
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  wrenSetSlotDouble(vm, 0, ctx->argc);
}

void wrenEnv(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  WREN_CHECK(t == WREN_TYPE_STRING, "must pass a string to io.env");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  const char* val = getenv(s);
  wrenSetSlotBytes(vm, 0, val, strlen(val));
}

void wrenExit(WrenVM* vm) {
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

void wrenExec(WrenVM* vm) {
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
    wrenError(vm);
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
  if (!strcmp(signature, "write(_)")) return wrenWrite;
  if (!strcmp(signature, "flush()")) return wrenFlush;
  if (!strcmp(signature, "read()")) return wrenRead;
  if (!strcmp(signature, "read(_)")) return wrenReadN;
  if (!strcmp(signature, "arg(_)")) return wrenArg;
  if (!strcmp(signature, "argc()")) return wrenArgc;
  if (!strcmp(signature, "env(_)")) return wrenEnv;
  if (!strcmp(signature, "exit(_)")) return wrenExit;
  if (!strcmp(signature, "exec(_)")) return wrenExec;
  if (!strcmp(signature, "exec(_,_)")) return wrenExec;
  fprintf(stderr, "unexpected foreign method %s", signature);
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
    "  io.read(n): read n bytes from stdin\n"
    "  io.write(s): write to stdout\n"
    "  io.flush(): flush stdout\n"
    "  io.arg(i): read args[i]\n"
    "  io.argc(): num args\n"
    "  io.env(name): read env var\n"
    "  io.exit(c): exit with code c\n"
    "  io.exec(argv): replace current process with argv\n"
    "  io.exec(argv, env): replace current process with argv and env\n";
  fputs(usage_str, stderr);
  exit(0);
}

int main(int argc, char** argv) {
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))))
    usage();

  uv_loop_t loop;
  uv_loop_init(&loop);

  Ctx ctx = {.argc = argc, .argv = argv, .loop = &loop};
  WrenVM* wren = setupWren(&ctx);
  char* io_src = \
      "class io {\n"
      "  foreign static write(s)\n"
      "  foreign static flush()\n"
      "  foreign static read()\n"
      "  foreign static read(n)\n"
      "  foreign static arg(i)\n"
      "  foreign static argc()\n"
      "  foreign static env(name)\n"
      "  foreign static exit(c)\n"
      "  foreign static exec(argv)\n"
      "  foreign static exec(argv, env)\n"
      "}\n";
  CHECK(wrenInterpret(wren, "io", io_src) == WREN_RESULT_SUCCESS, "bad io src");
  char* user_src = argv[argc - 1];
  CHECK(wrenInterpret(wren, "main", "import \"io\" for io") == WREN_RESULT_SUCCESS);
  int res = wrenInterpret(wren, "main", user_src);
  if (res != WREN_RESULT_SUCCESS) return res;
  uv_run(&loop, UV_RUN_DEFAULT);

  // Clean shutdown. Skipped since the OS will clean up for us.
  // wrenFreeVM(wren);
  // uv_loop_close(&loop);
}
