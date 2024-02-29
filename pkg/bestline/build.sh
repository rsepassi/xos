[ "$TARGET_OS" = "windows" ] && { >&2 echo "unsupported on windows"; exit 1; }

url="https://api.github.com/repos/jart/bestline/tarball/23c7046"
hash="c55bd77886ee0c5d53e32c0d73460615c20f881b19f511575e3df4d1e29fd884"
file="bestline.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cc -target $TARGET -O$OPT \
  -c -o bestline.o bestline.c -lc
ar rcs $(zigi lib bestline) bestline.o

cd "$BUILD_OUT"
mkdir lib include
cp "$src/bestline.h" include
mv "$src/$(zigi lib bestline)" lib
pkg-config --gendefault bestline
