url="https://github.com/vstakhov/libucl/archive/refs/tags/0.9.2.tar.gz"
hash="f63ddee1d7f5217cac4f9cdf72b9c5e8fe43cfe5725db13f1414b0d8a369bbe0"
file="ucl.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch ucl.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude -Isrc -Iklib -Iuthash \
  ucl.c src/*.c \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  ucl.c utils/ucl-tool.c \
  $(zigi lib ucl) \
  -lc

touch ucl-objdump.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  ucl-objdump.c utils/objdump.c \
  $(zigi lib ucl) \
  -lc

cd "$BUILD_OUT"
mkdir include lib bin
cp "$src/include/ucl.h" include
mv "$src/$(zigi lib ucl)" lib
mv "$src/$(zigi exe ucl)" bin
mv "$src/$(zigi exe ucl-objdump)" bin
