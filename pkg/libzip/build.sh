need zlib
need zstd
need xz
need mbedtls

url="http://slackware.cs.utah.edu/pub/slackware/slackware-current/source/l/libzip/libzip-1.10.1.tar.xz"
hash="dc3c8d5b4c8bbd09626864f6bcf93de701540f761d76b85d7c7d710f4bd90318"
file="libzip.tar.xz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

windows_srcs="
lib/zip_crypto_win.c
lib/zip_random_win32.c
lib/zip_source_file_win32.c
lib/zip_source_file_win32_utf8.c
lib/zip_source_file_win32_ansi.c
lib/zip_source_file_win32_named.c
lib/zip_source_file_win32_utf16.c
"

if [ "$TARGET_OS" = "macos" ]
then
  cflags="
  -DHAVE_ARC4RANDOM
  -DHAVE_CLONEFILE 
  -DHAVE_SECURE_RANDOM
  "
  echo $windows_srcs | xargs rm
  cp "$BUILD_PKG/config.h" .
elif [ "$TARGET_OS" = "linux" ]
then
  cflags="
  -DHAVE_SECURE_RANDOM
  "
  echo $windows_srcs | xargs rm
  cp "$BUILD_PKG/config.h" .
elif [ "$TARGET_OS" = "windows" ]
then
  ldflags="-lbcrypt"
  cp "$BUILD_PKG/config-windows.h" config.h
  rm lib/zip_crypto_mbedtls.c
fi

rm lib/zip_algorithm_bzip2.c
rm lib/zip_crypto_gnutls.c
rm lib/zip_crypto_commoncrypto.c
rm lib/zip_crypto_openssl.c
rm lib/zip_random_uwp.c

cp "$BUILD_PKG/zipconf.h" .
cp "$BUILD_PKG/zip_err_str.c" lib
touch zip.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. -Ilib \
  zip.c lib/*.c \
  $cflags \
  $(pkg-config --cflags xz/lzma mbedtls/mbedcrypto zstd zlib/z) \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. -Ilib \
  zip.c src/ziptool.c \
  $(zigi lib zip) \
  $(pkg-config --libs xz/lzma mbedtls/mbedcrypto zstd zlib/z) \
  $ldflags -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/lib/zip.h" include
mv "$src/$(zigi lib zip)" lib
mv "$src/$(zigi exe zip)" bin
