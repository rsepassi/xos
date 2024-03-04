need libuv

url="https://github.com/uNetworking/uSockets/archive/refs/tags/v0.8.8.tar.gz"
hash="d14d2efe1df767dbebfb8d6f5b52aa952faf66b30c822fbe464debaa0c5c0b17"
file="usockets.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch usockets.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DLIBUS_USE_LIBUV \
  -DLIBUS_NO_SSL \
  -Isrc \
  $(pkg-config --cflags libuv/uv) \
  usockets.c src/*.c src/eventing/*.c src/crypto/*.c \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -Isrc \
  examples/echo_server.c \
  $(zigi lib usockets) \
  $(pkg-config --libs libuv/uv) \
  -lc

cd "$BUILD_OUT"
mkdir lib include
mv "$src/src/libusockets.h" include
mv "$src/$(zigi lib usockets)" "$BUILD_OUT/lib"
pkg-config --gendefault usockets --deps libuv/uv
