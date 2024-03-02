fetch \
  "https://api.github.com/repos/hexops/freetype/tarball/398638f" \
  freetype.tar.gz \
  "efea9e183151834c64944b224233601b64847f9232c04f33f2b7de4f15a1ea18"
src=$(mktemp -d)
untar $BUILD_DEPS/freetype.tar.gz $src
cd $src
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv $PWD/zig-out/include $BUILD_OUT
mv $PWD/zig-out/lib $BUILD_OUT
