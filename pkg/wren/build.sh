fetch_urltxt $BUILD_PKG/url.txt wren.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/wren.tar.gz $src

# lib
cd $src/src/vm
touch wren.c
zig build-lib -target $TARGET -O $OPT_ZIG wren.c *.c \
  -I "$src"/src/include -DWREN_OPT_META=0 -DWREN_OPT_RANDOM=0 \
  -lc

# install
out="$BUILD_OUT"
mkdir -p $out/lib $out/include $out/pkgconfig
cp "$src"/src/include/wren.h $out/include
cp "$src"/src/vm/$(zigi lib wren) $out/lib
pkg-config --gendefault wren
