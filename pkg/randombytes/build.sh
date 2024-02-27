zig build-lib -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/randombytes.zig"
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/randombytes.c" $(zigi lib randombytes) -lc

cd "$BUILD_OUT"
mkdir bin lib include
mv "$HOME/$(zigi exe randombytes)" bin
mv "$HOME/$(zigi lib randombytes)" lib
cp "$BUILD_PKG/randombytes.h" include
pkg-config --gendefault randombytes
