set -e

fetch_urltxt $BUILD_PKG/url.txt sdl.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/sdl.tar.gz $src
cd $src
cp $BUILD_PKG/build.zig .

if [ "$ARCH_OS" = "macos" ]
then
  macsdk=$(system xcrun --sdk macosx --show-sdk-path)
  flags="--sysroot $macsdk"
fi

zig build -Dtarget=$ARCH -Doptimize=$OPT_ZIG $flags

ln -s $PWD/zig-out/include $BUILD_OUT
ln -s $PWD/zig-out/lib $BUILD_OUT
