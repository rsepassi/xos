src=$(fetch_untar "https://github.com/dlbeer/quirc/archive/refs/tags/v1.2.tar.gz" \
  quirc.tar.gz \
  "73c12ea33d337ec38fb81218c7674f57dba7ec0570bddd5c7f7a977c0deb64c5")
cd $src

touch quirc.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  quirc.c \
  lib/*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp $src/lib/quirc.h include
cp $src/$(zigi lib quirc) lib
