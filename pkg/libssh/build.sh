need mbedtls
need zlib

url="https://www.libssh.org/files/0.10/libssh-0.10.6.tar.xz"
hash="1861d498f5b6f1741b6abc73e608478491edcf9c9d4b6630eef6e74596de9dc1"
file="libssh.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "windows" ]
then
  os_srcs="src/threads/winlocks.c"
else
  os_srcs="src/threads/pthread.c"
fi

# rm alternate crypto impls
rm src/gssapi.c
rm src/getrandom_gcrypt.c
rm src/getrandom_crypto.c
rm src/libcrypto.c
rm src/libcrypto-compat.c
rm src/dh_crypto.c
rm src/ecdh_crypto.c
rm src/ecdh_gcrypt.c
rm src/md_crypto.c
rm src/md_gcrypt.c
rm src/pki_crypto.c

cp "$BUILD_PKG/config-$TARGET_OS.h" config.h
cp "$BUILD_PKG/libssh_version.h" include/libssh
touch ssh.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude -I. \
  ssh.c src/*.c src/threads/mbedtls.c $os_srcs \
  -DLIBSSH_STATIC -DMBEDTLS_ALLOW_PRIVATE_ACCESS \
  $(pkg-config --cflags mbedtls/mbedcrypto) \
  $(pkg-config --cflags zlib/z) \
  -lc

cd "$BUILD_OUT"
mkdir lib
mv "$src/$(zigi lib ssh)" lib
cp -r "$src/include" .
