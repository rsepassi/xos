url="https://monocypher.org/download/monocypher-4.0.2.tar.gz"
hash="38d07179738c0c90677dba3ceb7a7b8496bcfea758ba1a53e803fed30ae0879c"
file="monocypher.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  -Isrc \
  src/monocypher.c \
  src/optional/monocypher-ed25519.c

cd "$BUILD_OUT"
mkdir lib include
cp "$src"/$(zigi lib monocypher) lib
cp "$src/src/monocypher.h" include
cp "$src/src/optional/monocypher-ed25519.h" include
pkg-config --gendefault monocypher
