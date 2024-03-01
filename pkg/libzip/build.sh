[ "$TARGET_OS" = "windows" ] && { >&2 echo "windows build incomplete"; exit 1; }

need zlib
need zstd
need xz
need mbedtls

url="https://libzip.org/download/libzip-1.10.1.tar.xz"
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
  "
  echo $windows_srcs | xargs rm
elif [ "$TARGET_OS" = "linux" ]
then
  echo $windows_srcs | xargs rm
elif [ "$TARGET_OS" = "windows" ]
then
  :
fi

rm lib/zip_algorithm_bzip2.c
rm lib/zip_crypto_gnutls.c
rm lib/zip_crypto_commoncrypto.c
rm lib/zip_crypto_openssl.c
rm lib/zip_random_uwp.c

cp "$BUILD_PKG/config.h" .
cp "$BUILD_PKG/zipconf.h" .
cp "$BUILD_PKG/zip_err_str.c" lib
touch zip.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. -Ilib -DHAVE_SECURE_RANDOM \
  zip.c lib/*.c \
  $cflags \
  $(pkg-config --cflags xz/lzma) \
  $(pkg-config --cflags mbedtls) \
  $(pkg-config --cflags zstd) \
  $(pkg-config --cflags zlib/z) \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. -Ilib \
  zip.c src/ziptool.c \
  $(zigi lib zip) \
  $(pkg-config --libs xz/lzma) \
  $(pkg-config --libs mbedtls/mbedcrypto) \
  $(pkg-config --libs zstd) \
  $(pkg-config --libs zlib/z) \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/lib/zip.h" include
mv "$src/$(zigi lib zip)" lib
mv "$src/$(zigi exe zip)" bin
