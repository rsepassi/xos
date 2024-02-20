mkdir -p $BUILD_OUT/bin

zig build-exe -target $ARCH -O $OPT_ZIG \
  "$BUILD_PKG/wrensh.c" \
  -I $BUILD_DEPS/wren/include \
  "$BUILD_DEPS"/wren/lib/$(zigi lib wren) \
  -lc

mv $(zigi exe wrensh) "$BUILD_OUT/bin"
