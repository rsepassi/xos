need freetype
need harfbuzz
need sokol
needtool sokol-shdc

slang=$(wrensh -c "$(cat <<EOF
var slangs = {
  "macos": "metal_macos",
  "linux": "glsl300es",
  "windows": "hlsl5",
  "ios": "metal_ios",
}
IO.write(slangs[IO.env("TARGET_OS")])
EOF
)")

"$BUILD_TOOLS/sokol-shdc/bin/sokol-shdc" \
  --input "$BUILD_PKG/spritealpha.glsl" \
  --output "spritealpha_shader.h" \
  --slang $slang

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
