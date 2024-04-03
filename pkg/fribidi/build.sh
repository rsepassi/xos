src=$(fetch_untar "https://github.com/fribidi/fribidi/releases/download/v1.0.13/fribidi-1.0.13.tar.xz" \
  "fribidi.tar.gz" \
  "7fa16c80c81bd622f7b198d31356da139cc318a63fc7761217af4130903f54a2")
cd $src

cp "$BUILD_PKG/config.h" .
cp "$BUILD_PKG/fribidi-config.h" lib
touch fribidi.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. -I lib \
  -DHAVE_CONFIG_H \
  fribidi.c \
  lib/*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp $src/lib/*.h include
mv $src/$(zigi lib fribidi) lib
