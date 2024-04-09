need freetype
need harfbuzz
need sokol
needtool sokol-shdc

"$BUILD_TOOLS/sokol-shdc/bin/sokol-shdc" \
  --input "$BUILD_PKG/loadchar.glsl" \
  --output "loadchar_shader.h" \
  --slang metal_macos

zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. \
  $(pkg-config --cflags harfbuzz freetype sokol) \
  "$BUILD_PKG/texthello.zig" \
  -lc

touch texthello.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  texthello.c \
  $(zigi lib texthello) \
  $(pkg-config --cflags --libs harfbuzz freetype sokol) \
  -lc

cd "$BUILD_OUT"
mkdir -p bin/resources
mv "$HOME"/$(zigi exe texthello) bin/demo
ln -s "$BUILD_PKG/CourierPrime-Regular.ttf" bin/resources
