file=xz.tar.xz
src=$(fetch_untar \
  "https://github.com/tukaani-project/xz/archive/refs/tags/v5.2.2.tar.gz" \
  "xz.tar.xz" \
  "578694987c14d73b2d075f477d89006522b91c88890f45a1d231cd29d555e00e")
cd $src
cp $BUILD_PKG/build.zig .
cp $BUILD_PKG/arch/config-$TARGET_OS.h config.h
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG
mv $PWD/zig-out/bin $BUILD_OUT
mv $PWD/zig-out/lib $BUILD_OUT
mv $PWD/zig-out/include $BUILD_OUT
pkg-config --gendefault lzma
