src=$(fetch_untar https://github.com/xiph/opus/archive/refs/tags/v1.4.tar.gz \
  opus.tar.gz \
  659e6b223e42a51b0a898632b9a5f406ccd5c2e00aa526ddd1264789774b94e5)
cd $src

cp $BUILD_PKG/build.zig .
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv zig-out/lib $BUILD_OUT
mv zig-out/include $BUILD_OUT
pkg-config --gendefault opus
