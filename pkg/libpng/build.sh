need zlib

url="https://downloads.sourceforge.net/project/libpng/libpng16/1.6.42/libpng-1.6.42.tar.xz"
hash="c919dbc11f4c03b05aba3f8884d8eb7adfe3572ad228af972bb60057bdb48450"
file="libpng.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

rm pngtest.c
cp "$BUILD_PKG/config.h" .
cp "$BUILD_PKG/pnglibconf.h" .
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -isystem . \
  $(pkg-config --cflags zlib/z) \
  png.c png*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$src/png.h" include
mv "$src/$(zigi lib png)" lib
