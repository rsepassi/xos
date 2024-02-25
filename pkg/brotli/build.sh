fetch_urltxt $BUILD_PKG/url.txt brotli.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/brotli.tar.gz $src
cd $src
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

ln -s $PWD/zig-out/include $BUILD_OUT
ln -s $PWD/zig-out/lib $BUILD_OUT
