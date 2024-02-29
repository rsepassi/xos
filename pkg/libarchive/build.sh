need mbedtls
need zlib
need zstd

url="https://www.libarchive.org/downloads/libarchive-3.7.2.tar.xz"
hash="04357661e6717b6941682cde02ad741ae4819c67a260593dfb2431861b251acb"
file="libarchive.tar.xz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-DTARGET_OS_OSX --sysroot=$sdk -I$sdk/usr/include -F$sdk/System/Library/Frameworks"
fi

cd "$src/libarchive"
cp "$BUILD_PKG/config-$TARGET_OS.h" config.h
touch archive.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DLIBARCHIVE_STATIC -D__LIBARCHIVE_ENABLE_VISIBILITY \
  $(pkg-config --cflags mbedtls/mbedcrypto) \
  $(pkg-config --cflags zlib/z) \
  $(pkg-config --cflags zstd) \
  -cflags $cflags -include "$BUILD_DEPS/mbedtls/include/mbedtls/compat-2.x.h" -- \
  archive.c *.c -lc
lib="$PWD/$(zigi lib archive)"

cd "$src/libarchive_fe"
touch archive_fe.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DLIBARCHIVE_STATIC -D__LIBARCHIVE_ENABLE_VISIBILITY \
  -I "$src/libarchive" \
  archive_fe.c *.c \
  -lc
libfe="$PWD/$(zigi lib archive_fe)"

ldflags="
$libfe
$lib
$(pkg-config --libs mbedtls/mbedcrypto)
$(pkg-config --libs zlib/z)
$(pkg-config --libs zstd)
-lc
"

cd "$src/tar"
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -I "$src/libarchive" \
  -I "$src/libarchive_fe" \
  bsdtar.c cmdline.c creation_set.c read.c subst.c util.c write.c bsdtar_windows.c \
  $ldflags
btar="$PWD/$(zigi exe bsdtar)"

cd "$src/cpio"
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -I "$src/libarchive" \
  -I "$src/libarchive_fe" \
  cpio.c cmdline.c cpio_windows.c \
  $ldflags
bcpio="$PWD/$(zigi exe cpio)"

cd "$src/cat"
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -I "$src/libarchive" \
  -I "$src/libarchive_fe" \
  bsdcat.c cmdline.c \
  $ldflags
bcat="$PWD/$(zigi exe bsdcat)"

if [ "$TARGET_OS" != "windows" ]
then
cd "$src/unzip"
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -I "$src/libarchive" \
  -I "$src/libarchive_fe" \
  bsdunzip.c cmdline.c la_getline.c \
  $ldflags
bunzip="$PWD/$(zigi exe bsdunzip)"
fi

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/libarchive/archive.h" include
mv $lib lib
mv $bunzip $btar $bcpio $bcat bin
