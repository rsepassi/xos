need wren
need wren-zig
need libuv
need xglob
need sds
need cjson
need lmdb
need lmdb-zig
needtool cstrbake

cstrbake="$BUILD_TOOLS/cstrbake/bin/cstrbake"

if [ "$OPT_ZIG" = "Debug" ]
then
  cflags="-DDEBUG"
fi

cat "$BUILD_PKG/usage.txt" | $cstrbake wrensh_src_usage > usage_src.c
cat "$BUILD_PKG/io.wren" | $cstrbake wrensh_src_io > io_src.c
echo "const char* wrensh_src_user = 0;" > user_src.c

zig build-lib -target $TARGET -O $OPT_ZIG \
  -I "$BUILD_PKG" \
  "$BUILD_PKG/wrensh.c" \
  usage_src.c \
  io_src.c \
  $cflags \
  $(pkg-config --cflags wren libuv/uv xglob sds cjson/json) \
  --dep lmdb \
  --dep wren \
  -Mwrensh="$BUILD_PKG/wrensh.zig" \
  $(pkg-config --cflags lmdb) \
  -Mlmdb=$BUILD_DEPS/lmdb-zig/zig/lib.zig \
  $(pkg-config --cflags wren) \
  -Mwren=$BUILD_DEPS/wren-zig/zig/wren.zig \
  -lc

touch wrensh.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  wrensh.c \
  user_src.c \
  $cflags \
  $(zigi lib wrensh) \
  $(pkg-config --cflags --libs wren libuv/uv xglob sds cjson/json lmdb) \
  -lc

cd "$BUILD_OUT"
mkdir bin lib
mv "$HOME/$(zigi exe wrensh)" bin
mv "$HOME/$(zigi lib wrensh)" lib
pkg-config --gendefault wrensh --deps "wren,libuv/uv,xglob,sds,cjson/json"
