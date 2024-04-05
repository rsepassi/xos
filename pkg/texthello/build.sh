need freetype
need harfbuzz
need sokol

zig build-lib -target $TARGET -O $OPT_ZIG \
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
mv "$HOME"/$(zigi exe texthello) bin
ln -s "$BUILD_DEPS/freetype/share/CourierPrime-Regular.ttf" bin/resources
