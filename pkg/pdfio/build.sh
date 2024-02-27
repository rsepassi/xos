need zlib

url="https://github.com/michaelrsweet/pdfio/releases/download/v1.2.0/pdfio-1.2.0.tar.gz"
hash="28e6b76662ad573ae44d61b1214e56839eea0b13a12701872101a70f6b597db7"
file="pdfio.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src


touch pdfio.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags zlib/z) \
  pdfio.c pdfio-*.c ttf.c -lc

mkdir "$BUILD_OUT/lib" "$BUILD_OUT/include"
mv $(zigi lib pdfio) "$BUILD_OUT/lib"
cp pdfio.h "$BUILD_OUT/include"
