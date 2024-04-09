version="0bc88af"
fetch "https://github.com/nothings/stb/raw/$version/stb_image.h" \
  stb_image.h \
  "c54b15a689e6a1f32c75e2ec23afa442e3e0e37e894b73c1974d08679b20dd5c"

zig build-lib -target $TARGET -O $OPT_ZIG \
  -DSTB_IMAGE_IMPLEMENTATION \
  -x c "$BUILD_DEPS/stb_image.h" \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$BUILD_DEPS/stb_image.h" include
mv "$HOME/$(zigi lib stb_image)" lib
pkg-config --gendefault stb_image
