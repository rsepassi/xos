fetch \
  "https://api.github.com/repos/hexops/brotli/tarball/62ab0d6"
  brotli.tar.gz \
  "b16f3ce6de16e3608048511f190179d316f52f72639654c8007ac0e65ad6ad4a"
src=$(mktemp -d)
untar $BUILD_DEPS/brotli.tar.gz $src
cd $src
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

ln -s $PWD/zig-out/include $BUILD_OUT
ln -s $PWD/zig-out/lib $BUILD_OUT
