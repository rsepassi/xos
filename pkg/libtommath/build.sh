url="https://github.com/libtom/libtommath/releases/download/v1.2.1/ltm-1.2.1.tar.xz"
hash="986025d7b374276fee2e30e99f3649e4ac0db8a02257a37ee10eae72abed0d1f"
file="ltm.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch tommath.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  tommath.c bn_*.c -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$src/tommath.h" include
mv "$src/$(zigi lib tommath)" lib
pkg-config --gendefault tommath
