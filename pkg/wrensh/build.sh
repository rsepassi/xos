need wren
need libuv
need xglob
need sds

if [ "$DEBUG" = 1 ]
then
  cflags="-DDEBUG"
fi

# Preprocess wrensh.c to bake in io.wren and the usage string
cp "$BUILD_PKG/wrensh.c" wrensh.c
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

zig build-exe -target $TARGET -O $OPT_ZIG \
  wrensh.c \
  $cflags \
  $(pkg-config --cflags --libs wren libuv/uv xglob sds) \
  -lc

cd "$BUILD_OUT"
mkdir bin
mv "$HOME/$(zigi exe wrensh)" bin
