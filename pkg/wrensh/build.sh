need wren
need libuv

mkdir -p $BUILD_OUT/bin

zig build-exe -target $ARCH -O $OPT_ZIG \
  "$BUILD_PKG/wrensh.c" \
  -I "$BUILD_DEPS/wren/include" \
  "$BUILD_DEPS"/wren/lib/$(zigi lib wren) \
  -I "$BUILD_DEPS/libuv/include" \
  "$BUILD_DEPS"/libuv/lib/$(zigi lib uv) \
  -lc

mv $(zigi exe wrensh) "$BUILD_OUT/bin"
