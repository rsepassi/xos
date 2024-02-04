fetch_urltxt $BUILD_PKG/url.txt lmdb.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/lmdb.tar.gz $src 3

# lib
cd $src
cc --target=$ARCH -o mdb.o -c mdb.c -lc
cc --target=$ARCH -o midl.o -c midl.c -lc
ar rcs liblmdb.a mdb.o midl.o

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp $src/lmdb.h $out/include
cp $src/liblmdb.a $out/lib
