url="https://raw.githubusercontent.com/edubart/minicoro/ff5321d/minicoro.h"
hash="c4205e8db0a95456dfde9f73f071609c6d2cad2ebfd1d74ed0a9254f121caa2f"
file="minicoro.h"

fetch "$url" "$file" "$hash"

cat <<EOF > minicoro.c
#define MINICORO_IMPL
#include "minicoro.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  -I "$BUILD_DEPS" \
  minicoro.c -lc

mkdir "$BUILD_OUT/include" "$BUILD_OUT/lib"
cp "$BUILD_DEPS/minicoro.h" "$BUILD_OUT/include"
cp $(zigi lib minicoro) "$BUILD_OUT/lib"
