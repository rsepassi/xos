url="https://api.github.com/repos/antirez/sds/tarball/a9a03bb"
hash="e73593fb0f5c60cb7264e0a0e9aeb494523cfc4eceda560b290d5484a7ba485b"
file="sds.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  sds.c -lc

cd "$BUILD_OUT"
mkdir include lib
mv "$src/sds.h" "$src/sdsalloc.h" include
mv "$src/$(zigi lib sds)" lib
