#include <stdio.h>
#include <string.h>

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
  char c;
  int i = 0;
  while ((c = getchar()) != EOF) {
    buf[i++] = c;
    if (i >= sz) {
      int newsz = sz * 2;
      char* newbuf = malloc(newsz);
      CHECK(newbuf != NULL, "could not allocate %d bytes for stdin", newsz);
      memcpy(newbuf, buf, sz);
      sz = newsz;
      buf = newbuf;
    }
  }
  buf[i] = '\0';
  wrenSetSlotBytes(vm, 0, buf, i);
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
  if (!strcmp(signature, "arg(_)")) return wrenArg;
  fprintf(stderr, "unexpected foreign method");
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
    "    wrensh <wren src> [args...]\n"
    "  \n"
    "  https://wren.io\n"
    "  \n"
    "  io.read: read stdin in full\n"
    "  io.write: write to stdout\n"
    "  io.arg(i): read args[i]\n";
  fputs(usage_str, stderr);
  exit(0);
}

int main(int argc, char** argv) {
  if (argc == 1 ||
      (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))))
    usage();

  Ctx ctx = {.argc = argc, .argv = argv};
  WrenVM* wren = setupWren(&ctx);
  char* io_src = "class io {\n  foreign static write(s)\n  foreign static read()\n  foreign static arg(i)\n}";
  CHECK(wrenInterpret(wren, "io", io_src) == WREN_RESULT_SUCCESS);
  char* user_src = argv[1];
  CHECK(wrenInterpret(wren, "main", "import \"io\" for io") == WREN_RESULT_SUCCESS);
  CHECK(wrenInterpret(wren, "main", user_src) == WREN_RESULT_SUCCESS);
}
