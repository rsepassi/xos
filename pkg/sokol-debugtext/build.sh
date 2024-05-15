need sokol-gfx

fetch "https://raw.githubusercontent.com/floooh/sokol/4fac1bb/util/sokol_debugtext.h" \
  "sokol_debugtext.h" \
  "a69344a6da5e480f23313ef22be4b8b6517246006ed75926d747ece76b6222ad"

cat <<EOF > sokol_debugtext.h
#include "sokol_gfx.h"
EOF
cat $BUILD_DEPS/sokol_debugtext.h >> sokol_debugtext.h

cat <<EOF > sokol_debugtext.c
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol_debugtext.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags sokol-gfx) \
  sokol_debugtext.c

cd $BUILD_OUT
mkdir include lib
cp $HOME/sokol_debugtext.h include
cp $HOME/$(zigi lib sokol_debugtext) lib
pkg-config --gendefault sokol_debugtext
