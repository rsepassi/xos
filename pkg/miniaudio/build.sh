url="https://github.com/mackron/miniaudio/raw/4a5b74b/miniaudio.h"
hash="6b2029714f8634c4d7c70cc042f45074e0565766113fc064f20cd27c986be9c9"
file="miniaudio.h"
fetch "$url" "$file" "$hash"

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  cflags="$(pkg-config --cflags macossdk) -framework CoreFoundation -framework CoreAudio -framework AudioToolbox"
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  -DMINIAUDIO_IMPLEMENTATION \
  -DMA_NO_RUNTIME_LINKING \
  -DMA_ENABLE_ONLY_SPECIFIC_BACKENDS \
  -DMA_ENABLE_COREAUDIO \
  $cflags \
  -x c "$BUILD_DEPS/miniaudio.h" \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
	-I "$BUILD_DEPS" \
  $cflags \
  "$BUILD_PKG/demo.c" \
  $(zigi lib miniaudio) \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin share
cp "$BUILD_DEPS/miniaudio.h" include
cp "$HOME/$(zigi lib miniaudio)" lib
cp "$HOME/$(zigi exe demo)" bin
ln -s "$BUILD_PKG/CantinaBand3.wav" share
pkg-config --gendefault miniaudio --cflags "$cflags"
