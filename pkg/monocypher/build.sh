file=monocypher.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src

cd $src/src

cc --target=$ARCH -O$OPT -c monocypher.c -o monocypher.o
cc --target=$ARCH -O$OPT -c optional/monocypher-ed25519.c -o monocypher-ed25519.o -I.
ar rcs libmonocypher.a monocypher.o monocypher-ed25519.o

mkdir -p $BUILD_OUT/lib $BUILD_OUT/include
cp libmonocypher.a $BUILD_OUT/lib
cp monocypher.h $BUILD_OUT/include
cp optional/monocypher-ed25519.h $BUILD_OUT/include
