set -e

fetch_urltxt $BUILD_PKG/url.txt freetype.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/freetype.tar.gz $src
cd $src
zig build -Dtarget=$ARCH -Doptimize=$OPT_ZIG

ln -s $PWD/zig-out/include $BUILD_OUT
ln -s $PWD/zig-out/lib $BUILD_OUT
