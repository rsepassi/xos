need zlib

url="https://libssh2.org/download/libssh2-1.11.0.tar.gz"
hash="3736161e41e2693324deb38c26cfdc3efe6209d634ba4258db1cecff6a5ad461"
file="libssh2.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src


if [ "$TARGET_OS" = "windows" ]
then
  ldflags="-lws2_32 -lbcrypt"
  cp "$BUILD_PKG/config-windows.h" src/libssh2_config.h
else
  need mbedtls
  cflags="$(pkg-config --cflags mbedtls/mbedcrypto)"
  cp "$BUILD_PKG/config.h" src/libssh2_config.h
fi

touch ssh2.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -Iinclude -Isrc \
  ssh2.c src/*.c \
  $(pkg-config --cflags zlib/z) \
  $cflags \
  $ldflags \
  -lc

cd "$BUILD_OUT"
mkdir lib
mv "$src/$(zigi lib ssh2)" lib
cp -r "$src/include" .
