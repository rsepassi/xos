need monocypher
need blake3

zig build-exe -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags --libs monocypher blake3) \
  $BUILD_PKG/crypt.zig \
  -lc

cd $BUILD_OUT
mkdir bin
mv $HOME/$(zigi exe crypt) bin
