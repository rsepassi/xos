src=$(fetch_untar "https://api.github.com/repos/erincatto/box2c/tarball/0f85bc1" \
  "box2c.tar.gz" \
  "4b3f34796195406ea7a6986c76f25c452dae424718d347da601190d4f7676910")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name box2c \
  -Iinclude -Iextern/simde \
  src/*.c

cd $BUILD_OUT
mkdir include lib
cp -r $src/include include
mv $src/$(zigi lib box2c) lib
pkg-config --gendefault box2c
