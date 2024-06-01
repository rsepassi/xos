src=$(fetch_untar https://github.com/BinomialLLC/basis_universal/archive/refs/tags/1.16.4.tar.gz \
  basisu.tar.gz \
  "e5740fd623a2f8472c9700b9447a8725a6f27d65b0b47c3e3926a60db41b8a64")
cd $src

cp $BUILD_PKG/build.zig .

zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG \
  -Dencoder -Dtranscoder

mv zig-out/lib $BUILD_OUT
mv zig-out/include $BUILD_OUT
