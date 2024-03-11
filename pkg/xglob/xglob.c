#include <stdio.h>
#include "xglob.h"
int main(int argc, char** argv) {
  xglob_ctx* ctx = xglob_init(argv[1], argv[2]);
  if (!ctx) return 1;
  char* match;
  while ((match = xglob_next(ctx))) {
    printf("%s\n", match);
  }
  xglob_free(ctx);
}
