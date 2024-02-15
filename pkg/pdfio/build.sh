need zlib

url="https://github.com/michaelrsweet/pdfio/releases/download/v1.2.0/pdfio-1.2.0.tar.gz"
hash="28e6b76662ad573ae44d61b1214e56839eea0b13a12701872101a70f6b597db7"
file="pdfio.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src


touch pdfio.c
zig build-lib -target $ARCH -O $OPT_ZIG \
  -I "$BUILD_DEPS/zlib/include" \
  pdfio.c pdfio-*.c ttf.c -lc

mkdir "$BUILD_OUT/lib" "$BUILD_OUT/include"
mv $(zigi lib pdfio) "$BUILD_OUT/lib"
cp pdfio.h "$BUILD_OUT/include"