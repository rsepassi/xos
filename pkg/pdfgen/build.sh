url="https://api.github.com/repos/AndreRenaud/PDFGen/tarball/ebcfb9c"
hash="b9a824dd6c71365204c2ea0d175bb97182f6b2c22bf91378a8d1ba16c6316d7f"
file="pdfgen.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  pdfgen.c -lc

cd "$BUILD_OUT"
mkdir lib include
mv "$src/$(zigi lib pdfgen)" lib
cp "$src/pdfgen.h" include
pkg-config --gendefault pdfgen
