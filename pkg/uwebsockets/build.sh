need usockets
need zlib

url="https://github.com/uNetworking/uWebSockets/archive/refs/tags/v20.60.0.tar.gz"
hash="eb72223768f93d40038181653ee5b59a53736448a6ff4e8924fd56b2fcdc00db"
file="uwebsockets.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src


touch uwebsockets.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Isrc \
  $(pkg-config --cflags usockets) \
  $(pkg-config --cflags zlib/z) \
  uwebsockets.c \
  -cflags -std=c++17 -- \
  capi/libuwebsockets.cpp \
  -lc++

# zig build-exe -target $TARGET -O $OPT_ZIG \
#   -Icapi \
#   $(pkg-config --cflags usockets) \
#   -cflags -std=c11 -- \
#   capi/examples/HelloWorld.c \
#   $(zigi lib uwebsockets) \
#   $(pkg-config --libs usockets) \
#   $(pkg-config --libs zlib/z) \
#   -lc++ -lc

cd "$BUILD_OUT"
mkdir lib include
mv "$src/capi/libuwebsockets.h" include
mv "$src/$(zigi lib uwebsockets)" "$BUILD_OUT/lib"
pkg-config --gendefault uwebsockets --deps zlib/z,usockets
