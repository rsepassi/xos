zig build-lib -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/xglob.zig" -lc
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/xglob.c" $(zigi lib xglob) -lc

cd "$BUILD_OUT"
mkdir bin lib include
cp "$BUILD_PKG/xglob.h" include
mv "$HOME/$(zigi lib xglob)" lib
mv "$HOME/$(zigi exe xglob)" bin
pkg-config --gendefault xglob
