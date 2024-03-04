need randombytes

url="https://api.github.com/repos/dsprenkels/sss/tarball/16c3fdb"
hash="dd3a0c9f2871efd096ac187cfd8b74e3dd1dc562783e9f447cabe7266b89255f"
file="sss.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags randombytes) \
  sss.c hazmat.c tweetnacl.c \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  test_sss.c \
  $(zigi lib sss) \
  $(pkg-config --cflags --libs randombytes) \
  -lc

cd "$BUILD_OUT"
mkdir lib include
mv "$src/sss.h" "$src/hazmat.h" "$src/tweetnacl.h" include
mv "$src/$(zigi lib sss)" "$BUILD_OUT/lib"
