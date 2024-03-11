need libuv
need zlib
need mbedtls
need llhttp

url="https://github.com/openziti/tlsuv/archive/refs/tags/v0.28.3.tar.gz"
hash="0c64755453a6f0c94851c90567b4ed0002212c8c7be453a9f5a3b713d147f88a"
file="tlsuv.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch tlsuv.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude -Isrc \
  -Ideps/uv_link_t \
  -Ideps/uv_link_t/include \
  $(pkg-config --cflags zlib/z mbedtls libuv/uv llhttp) \
  tlsuv.c src/*.c src/mbedtls/*.c \
  deps/uv_link_t/src/*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib
mv "$src/$(zigi lib tlsuv)" lib
mv "$src/include" .
