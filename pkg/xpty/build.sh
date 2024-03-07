need libuv

file="unixpty.c"
if [ "$TARGET_OS" = "windows" ]
then
  file="winpty.c"
  need winsdk
  sdk="$BUILD_DEPS/winsdk"
  mkdir include
  cp "$sdk/sdk-cpp/c/Include/10.0.22621.0/um/wincontypes.h" include
  cp "$sdk/sdk-cpp/c/Include/10.0.22621.0/um/consoleapi.h" include
  touch include/wincon.h
  cflags="-Iinclude"
fi

touch xpty.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  $cflags \
  $(pkg-config --cflags libuv/uv) \
  xpty.c "$BUILD_PKG/$file" \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  $cflags \
  "$BUILD_PKG/xpty_test.c" \
  $(zigi lib xpty) \
  $(pkg-config --cflags --libs libuv/uv) \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$BUILD_PKG/xpty.h" include
mv "$HOME/$(zigi lib xpty)" lib
mv "$HOME/$(zigi exe xpty_test)" bin
