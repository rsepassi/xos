url="https://github.com/libexpat/libexpat/releases/download/R_2_6_1/expat-2.6.1.tar.gz"
hash="86943cd33f952077f4a50aaa540333d61c49d980c435f551c63640833e2229d1"
file="expat.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  cflags="
  -DHAVE_SYSCALL_GETRANDOM 
  -DHAVE_GETRANDOM 
  -DHAVE_ARC4RANDOM_BUF
  -DHAVE_ARC4RANDOM
  "
elif [ "$TARGET_OS" = "windows" ]
then
  cflags=""
elif [ "$TARGET_OS" = "linux" ]
then
  cflags="
  -DHAVE_SYSCALL_GETRANDOM 
  -DHAVE_GETRANDOM 
  "
fi

cp "$BUILD_PKG/expat_config.h" .
touch expat.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. -Ilib $cflags \
  expat.c lib/*.c \
  -lc

cd "$BUILD_OUT"
mkdir -p lib include
mv "$src/$(zigi lib expat)" lib
cp "$src/lib/expat.h" "$src/lib/expat_external.h" include
pkg-config --gendefault expat
