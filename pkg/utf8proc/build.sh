url="https://github.com/JuliaStrings/utf8proc/releases/download/v2.9.0/utf8proc-2.9.0.tar.gz"
hash="bd215d04313b5bc42c1abedbcb0a6574667e31acee1085543a232204e36384c4"
file="utf8proc.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  -DUTF8PROC_STATIC \
  utf8proc.c -lc

cd "$BUILD_OUT"
mkdir -p lib include
mv "$src/$(zigi lib utf8proc)" lib
cp "$src/utf8proc.h" include
pkg-config --gendefault utf8proc
