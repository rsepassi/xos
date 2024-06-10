need blend2d

if [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ABI" = "android" ]
then
  needtool androidsdk
  cflags="
    $(BUILD_DEPS=$BUILD_TOOLS pkg-config --cflags androidsdk)
    $(BUILD_DEPS=$BUILD_TOOLS zigi libc androidsdk)
  "
elif [ "$TARGET_OS" = "ios" ]
then
  need iossdk
  cflags="
    $(pkg-config --cflags iossdk) \
    $(zigi libc iossdk) \
  "
fi

>&2 echo hi

zig build-lib -dynamic -target $TARGET -O $OPT_ZIG \
  --name app \
  $cflags \
  $(pkg-config --cflags --libs blend2d) \
  $BUILD_PKG/main.zig \
  -lc++ \
  -lc

cd $BUILD_OUT
mkdir bin
#mv $HOME/$(zigi exe app) bin
