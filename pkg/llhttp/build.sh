
url="https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.0.tar.gz"
hash="c5bce80c5bbc5fa8b978ea57ef074bf9b22e39b077f94d30f72367681817afbd"
file="llhttp.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch llhttp.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  llhttp.c src/*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib
mv "$src/$(zigi lib llhttp)" lib
mv "$src/include" .
pkg-config --gendefault llhttp
