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

echo "#define $backend 1" > sokol_gfx.h
cat $BUILD_PKG/sokol_gfx.h >> sokol_gfx.h

cp $BUILD_PKG/sokol_gfx.c .

zig build-lib -target $TARGET -O $OPT_ZIG \
  -D${backend} \
  -I $BUILD_PKG \
  $defs \
  sokol_gfx.c \
  $libs

cd $BUILD_OUT
mkdir include lib pkgconfig
cp $HOME/sokol_gfx.h include
mv $HOME/$(zigi lib sokol_gfx) lib
cat <<EOF > pkgconfig/sokol-gfx.pc
Cflags: -I\${rootdir}/include $(echo $libs)
Libs: \${rootdir}/libsokol_gfx.a $(echo $libs)
EOF
