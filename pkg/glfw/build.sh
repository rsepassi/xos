src=$(fetch_untar "https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz" \
  glfw.tar.gz \
  "c038d34200234d071fae9345bc455e4a8f2f544ab60150765d7704e08f3dac01")
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdkflags=$(pkg-config --cflags macossdk)
  cflags="-D_GLFW_COCOA $sdkflags"
  cflagsdeps="$sdkflags"
  ldflags="$(pkg-config --libs macossdk) -framework Cocoa -framework IOKit"
  addl_srcs=$(ls src/*.m)
elif [ "$TARGET_OS" = "linux" ]
then
  need linuxsdk -- alpine libx11-dev,libxcursor-dev,libxrandr-dev,libxinerama-dev,libxi-dev X11
  sdkflags=$(pkg-config --cflags linuxsdk)
  cflags="-D_GLFW_X11 $sdkflags"
  cflagsdeps="$sdkflags"
elif [ "$TARGET_OS" = "windows" ]
then
  cflags="-D_GLFW_WIN32"
  ldflags="-lws2_32 -luserenv -lbcrypt -lopengl32 -lgdi32"
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name glfw \
  $cflags \
  src/*.c \
  $addl_srcs \
  -lc

cd $BUILD_OUT
mkdir lib
mv $src/$(zigi lib glfw) lib
mv $src/include include
pkg-config --gendefault glfw --cflags "$cflagsdeps" --ldflags "$ldflags"
