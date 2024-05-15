need sokol_gfx

src=$(fetch_untar "https://api.github.com/repos/floooh/sokol-zig/tarball/864422a" \
  sokol-zig.tar.gz \
  "a110c5e5f3a9f9a4aabbff31dc75ab70e31ff50e0b0f8f12e7f94dc5ae567d77")
src=$src/src/sokol/c
cd $src

cat <<EOF > sokol_debugtext2.h
#include "sokol_gfx.h"
EOF
cat sokol_debugtext.h >> sokol_debugtext2.h
mv sokol_debugtext2.h sokol_debugtext.h

cat <<EOF > sokol_debugtext.c
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol_debugtext.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags sokol_gfx) \
  sokol_debugtext.c

cd $BUILD_OUT
mkdir include lib
cp $src/sokol_debugtext.h include
cp $src/$(zigi lib sokol_debugtext) lib
pkg-config --gendefault sokol_debugtext --deps sokol_gfx
