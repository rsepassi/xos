# fetch source
file="zlib.tar.gz"
fetch "https://zlib.net/zlib-1.3.1.tar.gz" \
  $file \
  "9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cp "$BUILD_PKG"/zconf.h .
touch z.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_HIDDEN -D_LARGEFILE64_SOURCE=1 \
  z.c *.c -lc

mkdir -p "$BUILD_OUT/lib" "$BUILD_OUT/include"
mv $(zigi lib z) "$BUILD_OUT/lib"
cp zconf.h zlib.h "$BUILD_OUT/include"
