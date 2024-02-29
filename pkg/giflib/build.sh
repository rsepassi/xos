url="https://downloads.sourceforge.net/project/giflib/giflib-5.2.1.tar.gz"
hash="31da5562f44c5f15d63340a09a4fd62b48c45620cd302f77a6d9acf0077879bd"
file="giflib.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch gif.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  gif.c *.c -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$src/gif_lib.h" include
mv "$src/$(zigi lib gif)" lib
