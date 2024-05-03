file=blake3.tar.gz
fetch \
  "https://github.com/BLAKE3-team/BLAKE3/archive/refs/tags/1.5.0.tar.gz" \
  $file \
  "f506140bc3af41d3432a4ce18b3b83b08eaa240e94ef161eb72b2e57cdc94c69"
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src
cd $src

cp $BUILD_PKG/build.zig .
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG
mv $PWD/zig-out/include $BUILD_OUT
mv $PWD/zig-out/lib $BUILD_OUT
pkg-config --gendefault blake3
