fetch_urltxt $BUILD_PKG/url.txt lmdb.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/lmdb.tar.gz $src 3

# lib
cd $src
touch lmdb.c
zig build-lib -target $ARCH -O $OPT_ZIG lmdb.c mdb.c midl.c -lc

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp $src/lmdb.h $out/include
cp $src/$(zigi lib lmdb) $out/lib
