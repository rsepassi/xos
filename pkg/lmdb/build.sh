fetch \
  "https://github.com/LMDB/lmdb/archive/refs/tags/LMDB_0.9.31.tar.gz" \
  lmdb.tar.gz \
  "dd70a8c67807b3b8532b3e987b0a4e998962ecc28643e1af5ec77696b081c9b0"
src=$(mktemp -d)
untar $BUILD_DEPS/lmdb.tar.gz $src 3

# lib
cd $src
touch lmdb.c
zig build-lib -target $TARGET -O $OPT_ZIG lmdb.c mdb.c midl.c -lc

# install
cd "$BUILD_OUT"
mkdir lib include
cp $src/lmdb.h include
cp $src/$(zigi lib lmdb) lib
pkg-config --gendefault lmdb
