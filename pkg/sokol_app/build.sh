src=$(fetch_untar "https://api.github.com/repos/floooh/sokol-zig/tarball/864422a" \
  sokol-zig.tar.gz \
  "a110c5e5f3a9f9a4aabbff31dc75ab70e31ff50e0b0f8f12e7f94dc5ae567d77")
src=$src/src/sokol/c
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  backend="SOKOL_METAL"
  defs="-x objective-c"
  need macossdk
  libs="$(pkg-config --cflags macossdk) -framework Foundation -framework Metal"
elif [ "$TARGET_OS" = "ios" ]
then
  backend="SOKOL_METAL"
  defs="-x objective-c"
  need iossdk
  libs="$(pkg-config --cflags iossdk) -framework Foundation -framework Metal"
elif [ "$TARGET_OS" = "windows" ]
then
  backend="SOKOL_D3D11"
  libs="-lc"
elif [ "$TARGET_OS" = "linux" ]
then
  backend="SOKOL_GLES3"
  if [ "$TARGET_ABI" = "android" ]
  then
    needtool androidsdk
    defs="$(BUILD_DEPS=$BUILD_TOOLS pkg-config --cflags androidsdk)"
  else
    need linuxsdk -- alpine mesa-dev,libxi-dev,libxcursor-dev GL,EGL,X11,Xi,Xcursor
    libs="$(pkg-config --cflags --libs linuxsdk) -lGL -lEGL -lX11 -lXi -lXcursor -lc"
  fi
fi

echo "#define $backend" > sokol_app2.h
cat sokol_app.h >> sokol_app2.h
mv sokol_app2.h sokol_app.h

cat <<EOF > sokol_app.c
#define SOKOL_APP_IMPL
#include "sokol_app.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG -fPIC \
  $defs \
  sokol_app.c \
  $libs

cd $BUILD_OUT
mkdir include lib
cp $src/sokol_app.h include
mv $src/$(zigi lib sokol_app) lib
pkg-config --gendefault sokol_app
