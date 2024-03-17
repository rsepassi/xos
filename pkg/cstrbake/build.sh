zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/cstrbake.c" \
  -lc

cd "$BUILD_OUT"
mkdir bin
mv "$HOME/$(zigi exe cstrbake)" bin
