file=monocypher.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src

cd $src/src

zig build-lib -target $ARCH -O $OPT_ZIG monocypher.c optional/monocypher-ed25519.c -I.

mkdir -p $BUILD_OUT/lib $BUILD_OUT/include
cp $(zigi lib monocypher) $BUILD_OUT/lib
cp monocypher.h $BUILD_OUT/include
cp optional/monocypher-ed25519.h $BUILD_OUT/include
