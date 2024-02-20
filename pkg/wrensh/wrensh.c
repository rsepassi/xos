#include <stdio.h>
#include <string.h>
#include <unistd.h>

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


typedef struct {
  int argc;
  char** argv;
} Ctx;

static const int maxpathlen = 1024;

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
  CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.read(n)");
  int n = (int)wrenGetSlotDouble(vm, 1);
  char* buf = malloc(n);
  CHECK(buf != NULL, "could not allocate %d bytes for stdin", n);
  int nread = fread(buf, 1, n, stdin);
  wrenSetSlotBytes(vm, 0, buf, nread);
  free(buf);
}

void wrenWrite(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  CHECK(t == WREN_TYPE_STRING, "must pass a string to io.write");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  fprintf(stdout, "%.*s", len, s);
}

void wrenArg(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.arg");
  int n = (int)wrenGetSlotDouble(vm, 1);
  Ctx* ctx = (Ctx*)wrenGetUserData(vm);
  CHECK(n < ctx->argc, "only %d args", ctx->argc);
  wrenSetSlotBytes(vm, 0, ctx->argv[n], strlen(ctx->argv[n]));
}

void wrenEnv(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  CHECK(t == WREN_TYPE_STRING, "must pass a string to io.env");
  int len = 0;
  const char* s = wrenGetSlotBytes(vm, 1, &len);
  const char* val = getenv(s);
  wrenSetSlotBytes(vm, 0, val, strlen(val));
}

void wrenExit(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  CHECK(t == WREN_TYPE_NUM, "must pass an integer to io.arg");
  int n = (int)wrenGetSlotDouble(vm, 1);
  exit(n);
}

void which(char* exe, char* path) {
  int exelen = strlen(exe);
  if (exe[0] == '/' || exe[0] == '\\') {
    // absolute path
    memcpy(path, exe, exelen);
    return;
  }

  char cmd[128] = "which ";

  int cmdlen = 6 + exelen;
  memcpy(cmd + 6, exe, exelen);
  cmd[cmdlen] = '\0';

  FILE* fp = popen(cmd, "r");
  CHECK(fp != NULL, "which failed to spawn");

  char* out = fgets(path, maxpathlen, fp);
  CHECK(out != NULL, "which stdout read failed");
  int outlen = strlen(out);
  CHECK(out[outlen - 1] == '\n', "path is too long");
  out[outlen - 1] = '\0';

  int returnCode = pclose(fp);
  CHECK(returnCode == 0, "command %s not found", exe);
}

void wrenExec(WrenVM* vm) {
  WrenType t = wrenGetSlotType(vm, 1);
  CHECK(t == WREN_TYPE_LIST, "must pass a list to execv");
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
  which(argv[0], &path);

  if (nslots == 3) {
    // with env
    WrenType t = wrenGetSlotType(vm, 2);
    CHECK(t == WREN_TYPE_LIST, "must pass a list to execv");
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

  CHECK(false, "execv failed");
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
  if (!strcmp(signature, "read()")) return wrenRead;
  if (!strcmp(signature, "read(_)")) return wrenReadN;
  if (!strcmp(signature, "arg(_)")) return wrenArg;
  if (!strcmp(signature, "env(_)")) return wrenEnv;
  if (!strcmp(signature, "exit(_)")) return wrenExit;
  if (!strcmp(signature, "execv(_)")) return wrenExec;
  if (!strcmp(signature, "execv(_,_)")) return wrenExec;
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
    "  io.arg(i): read args[i]\n"
    "  io.env(name): read env var\n"
    "  io.exit(c): exit with code c\n"
    "  io.execv(argv): replace current process with argv\n"
    "  io.execv(argv, env): replace current process with argv and env\n";
  fputs(usage_str, stderr);
  exit(0);
}

int main(int argc, char** argv) {
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))))
    usage();

  Ctx ctx = {.argc = argc, .argv = argv};
  WrenVM* wren = setupWren(&ctx);
  char* io_src = \
      "class io {\n"
      "  foreign static write(s)\n"
      "  foreign static read()\n"
      "  foreign static read(n)\n"
      "  foreign static arg(i)\n"
      "  foreign static env(name)\n"
      "  foreign static exit(c)\n"
      "  foreign static execv(argv)\n"
      "  foreign static execv(argv, env)\n"
      "}\n";
  CHECK(wrenInterpret(wren, "io", io_src) == WREN_RESULT_SUCCESS, "bad io src");
  char* user_src = argv[argc - 1];
  CHECK(wrenInterpret(wren, "main", "import \"io\" for io") == WREN_RESULT_SUCCESS);
  CHECK(wrenInterpret(wren, "main", user_src) == WREN_RESULT_SUCCESS);
}
