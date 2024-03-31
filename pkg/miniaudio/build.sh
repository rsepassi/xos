url="https://github.com/mackron/miniaudio/raw/4a5b74b/miniaudio.h"
hash="6b2029714f8634c4d7c70cc042f45074e0565766113fc064f20cd27c986be9c9"
file="miniaudio.h"
fetch "$url" "$file" "$hash"

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  cflags=$(pkg-config --cflags macossdk)
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  -DMINIAUDIO_IMPLEMENTATION \
  -cflags $cflags -- \
  -x c "$BUILD_DEPS/miniaudio.h" \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$BUILD_DEPS/miniaudio.h" include
cp "$HOME/$(zigi lib miniaudio)" lib
