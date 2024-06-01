src=$(fetch_untar https://github.com/xiph/flac/archive/refs/tags/1.4.3.tar.gz \
  flac.tar.gz \
  0a4bb82a30609b606650d538a804a7b40205366ce8fc98871b0ecf3fbb0611ee)
cd $src

cp $BUILD_PKG/build.zig .

zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv zig-out/lib $BUILD_OUT
mv zig-out/include $BUILD_OUT
pkg-config --gendefault flac
