need libuv
need zlib
need mbedtls
need llhttp

url="https://github.com/openziti/tlsuv/archive/refs/tags/v0.28.5.tar.gz"
hash="eb0bb18f4b8cd12e4efccc973e5ae0d27d951db7ac61114c21da03b327d983f2"
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
