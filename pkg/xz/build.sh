file=xz.tar.xz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src
cd $src
cp $BUILD_PKG/build.zig .
cp $BUILD_PKG/arch/config-$ARCH_OS.h config.h
zig build -Dtarget=$ARCH -Doptimize=$OPT_ZIG
ln -s $PWD/zig-out/bin $BUILD_OUT
ln -s $PWD/zig-out/lib $BUILD_OUT
ln -s $PWD/zig-out/include $BUILD_OUT
