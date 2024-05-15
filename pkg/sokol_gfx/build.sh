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
  libs="$(pkg-config --cflags macossdk)
  -framework Metal
  -framework MetalKit
  -framework Cocoa
  -framework AppKit
  -framework CoreData
  -framework ApplicationServices
  -framework Foundation
  -framework ColorSync
  -framework CoreGraphics
  -framework CoreServices
  -framework CoreText
  -framework CoreFoundation
  -framework CFNetwork
  -framework ImageIO
  -framework QuartzCore
  -framework CoreImage
  -framework CoreVideo
  "
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
  if [ "$TARGET_ABI" = "android" ]
  then
    backend="SOKOL_GLES3"
    needtool androidsdk
    libs="$(PC_DEPS_DIR=$BUILD_TOOLS pkg-config --cflags androidsdk) -lc"
  else
    backend="SOKOL_GLES3"
    need linuxsdk -- alpine mesa-dev,libxi-dev,libxcursor-dev GL,EGL,X11,Xi,Xcursor
    libs="$(pkg-config --cflags --libs linuxsdk) -lGL -lEGL -lX11 -lXi -lXcursor -lc"
  fi
fi

echo "#define $backend" > sokol_gfx2.h
cat sokol_gfx.h >> sokol_gfx2.h
mv sokol_gfx2.h sokol_gfx.h

cat <<EOF > sokol_gfx.c
#define SOKOL_GFX_IMPL
#include "sokol_gfx.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  -D${backend} \
  -I $BUILD_PKG \
  $defs \
  sokol_gfx.c \
  $libs

cd $BUILD_OUT
mkdir include lib pkgconfig
cp $src/sokol_gfx.h include
mv $src/$(zigi lib sokol_gfx) lib
cat <<EOF > pkgconfig/sokol_gfx.pc
Cflags: -I\${rootdir}/include -D$backend $(echo $libs)
Libs: \${rootdir}/lib/libsokol_gfx.a $(echo $libs)
EOF
