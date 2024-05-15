need sokol_app
need sokol_debugtext
need sokol_zig

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name app \
  $(pkg-config --cflags sokol_debugtext) \
  --dep sokol_zig \
  -Mmain="$BUILD_PKG/app.zig" \
  $(pkg-config --zig sokol_zig)

touch hello.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  hello.c \
  $(zigi lib app) \
  $(pkg-config --cflags --libs sokol_debugtext sokol_app) \
  -lc

cd "$BUILD_OUT"
mkdir -p bin
mv "$HOME"/$(zigi exe hello) bin
