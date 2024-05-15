if [ "$TARGET_OS" = "macos" ]
then
  defs="-DSOKOL_METAL -x objective-c"
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
  defs="-DSOKOL_METAL -x objective-c"
  need iossdk
  libs="$(pkg-config --cflags iossdk) -framework Foundation -framework Metal"
elif [ "$TARGET_OS" = "windows" ]
then
  defs="-DSOKOL_D3D11"
  libs="-lc"
elif [ "$TARGET_OS" = "linux" ]
then
  if [ "$TARGET_ABI" = "android" ]
  then
    needtool androidsdk
    . $BUILD_TOOLS/androidsdk/env.sh
    defs="-DSOKOL_GLES3 -I$ANDROID_HOME/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include -I$ANDROID_HOME/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/aarch64-linux-android -lc"
  else
    defs="-DSOKOL_GLES3"
    need linuxsdk -- alpine mesa-dev,libxi-dev,libxcursor-dev GL,EGL,X11,Xi,Xcursor
    libs="$(pkg-config --cflags --libs linuxsdk) -lGL -lEGL -lX11 -lXi -lXcursor -lc"
  fi
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  $defs \
  $BUILD_PKG/sokol_gfx.c \
  $libs

cd $BUILD_OUT
mkdir include lib
cp $BUILD_PKG/sokol_gfx.h include
mv $HOME/$(zigi lib sokol_gfx) lib
pkg-config --gendefault sokol_gfx
