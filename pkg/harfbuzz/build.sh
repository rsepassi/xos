fetch_urltxt $BUILD_PKG/url.txt harfbuzz.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/harfbuzz.tar.gz $src
cd $src
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv $PWD/zig-out/include $BUILD_OUT
mv $PWD/zig-out/lib $BUILD_OUT
