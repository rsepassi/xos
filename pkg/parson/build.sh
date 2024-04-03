src=$(fetch_untar "https://api.github.com/repos/kgabis/parson/tarball/ba29f4e" \
  "parson.tar.gz" \
  "9ce418741b318e182b01bf2a7586f92d746a066431073ab2aa6841219570608b")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  parson.c \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp $src/parson.h include
mv $src/$(zigi lib parson) lib
