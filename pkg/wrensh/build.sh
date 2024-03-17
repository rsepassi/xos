need wren
need libuv
need xglob
need sds

if [ "$OPT_ZIG" = "Debug" ]
then
  cflags="-DDEBUG"
fi

# Preprocess wrensh.c to bake in io.wren and the usage string
cp "$BUILD_PKG/wrensh.c" .
cstr() {
  clines=""
  while IFS='' read -r line; do
    line="$(echo "$line" | sed -e 's/\\/\\\\/g' | sed -e 's/"/\\\\"/g')"
    clines="$clines\n\"$line\\\\n\""
  done < "$1"
  echo "$clines"
}
iowren="$(cstr "$BUILD_PKG/io.wren")"
usagestr="$(cstr "$BUILD_PKG/usage.txt")"

sed -i "s^@@IOWREN@@^$iowren^" wrensh.c
sed -i "s^@@WRENSHUSAGE@@^$usagestr^" wrensh.c

echo "const char* baked_user_src = 0;" > user_src.c

zig build-exe -target $TARGET -O $OPT_ZIG \
  wrensh.c \
  user_src.c \
  $cflags \
  $(pkg-config --cflags --libs wren libuv/uv xglob sds) \
  -lc

zig build-lib -target $TARGET -O $OPT_ZIG \
  wrensh.c \
  $(pkg-config --cflags wren libuv/uv xglob sds) \
  -lc

cd "$BUILD_OUT"
mkdir bin lib
mv "$HOME/$(zigi exe wrensh)" bin
mv "$HOME/$(zigi lib wrensh)" lib
pkg-config --gendefault wrensh --deps "wren,libuv/uv,xglob,sds"
