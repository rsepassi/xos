need zlib

url="https://github.com/facebook/zstd/releases/download/v1.5.5/zstd-1.5.5.tar.gz"
hash="9c4396cc829cfae319a6e2615202e82aad41372073482fce286fac78646d3ee4"
file="zstd.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cd $src/lib
touch zstd.c
([ "$TARGET_OS" = "linux" ] || [ "$TARGET_OS" = "macos" ]) \
  && [ "$TARGET_ARCH" = "x86_64" ] \
  && lib_addl_srcs=decompress/huf_decompress_amd64.S
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DXXH_NAMESPACE=ZSTD_ -DDEBUGLEVEL=0 -DZSTD_LEGACY_SUPPORT=5 \
  zstd.c common/*.c compress/*.c decompress/*.c dictBuilder/*.c legacy/*.c \
  $lib_addl_srcs \
  -lc
lib=$src/lib/$(zigi lib zstd)

cd $src/programs
[ "$TARGET_OS" = "windows" ] && [ "$TARGET_ARCH" = "x86_64" ] \
  && exe_addl_srcs=windres/zstd64.res
zig build-exe -target $TARGET -O $OPT_ZIG --name zstd \
  -DZSTD_MULTITHREAD -DZSTD_GZCOMPRESS -DZSTD_GZDECOMPRESS \
  *.c $exe_addl_srcs \
  -Ilib \
  $lib \
  $(pkg-config --cflags --libs zlib/z) \
  -lc

cd $src
mkdir "$BUILD_OUT/bin" "$BUILD_OUT/lib" "$BUILD_OUT/include"
cp lib/*.h "$BUILD_OUT/include"
mv $lib "$BUILD_OUT/lib"
mv $src/programs/$(zigi exe zstd) "$BUILD_OUT/bin"
pkg-config --gendefault zstd
