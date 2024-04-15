need zlib

url="https://downloads.sourceforge.net/project/libpng/libpng16/1.6.42/libpng-1.6.42.tar.xz"
hash="c919dbc11f4c03b05aba3f8884d8eb7adfe3572ad228af972bb60057bdb48450"
file="libpng.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_ARCH" = "aarch64" ]
then
  extras="$(ls arm/*)"
elif [ "$TARGET_ARCH" = "x86_64" ]
then
  extras="$(ls intel/*)"
fi

rm pngtest.c

cp "$BUILD_PKG/config.h" .
cp "$BUILD_PKG/pnglibconf.h" .
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -isystem . \
  $(pkg-config --cflags zlib/z) \
  png.c png*.c $extras \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. \
  "$BUILD_PKG/demo.c" \
  $(zigi lib png) \
  $(pkg-config --libs zlib/z) \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/png.h" include
cp "$src/pngconf.h" include
cp "$BUILD_PKG/pnglibconf.h" include
mv "$src/$(zigi lib png)" lib
mv "$src/$(zigi exe demo)" bin
pkg-config --gendefault png --deps zlib/z
