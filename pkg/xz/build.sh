file=xz.tar.xz
src=$(fetch_untar \
  "https://github.com/tukaani-project/xz/releases/download/v5.4.6/xz-5.4.6.tar.xz" \
  "xz.tar.xz" \
  "b92d4e3a438affcf13362a1305cd9d94ed47ddda22e456a42791e630a5644f5c")
cd $src
cp $BUILD_PKG/build.zig .
cp $BUILD_PKG/arch/config-$TARGET_OS.h config.h
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG
mv $PWD/zig-out/bin $BUILD_OUT
mv $PWD/zig-out/lib $BUILD_OUT
mv $PWD/zig-out/include $BUILD_OUT
