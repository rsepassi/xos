if [ "$TARGET_OS" = "macos" ]
then
  defs="-DSOKOL_METAL -x objective-c"
  need macossdk
  libs="$(pkg-config --cflags macossdk) -framework Foundation -framework Metal"
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
    defs="-DSOKOL_GLES3 $(PC_DEPS_DIR=$BUILD_TOOLS pkg-config --cflags androidsdk)"
  else
    defs="-DSOKOL_GLES3"
    need linuxsdk -- alpine mesa-dev,libxi-dev,libxcursor-dev GL,EGL,X11,Xi,Xcursor
    libs="$(pkg-config --cflags --libs linuxsdk) -lGL -lEGL -lX11 -lXi -lXcursor -lc"
  fi
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  $defs \
  $BUILD_PKG/sokol_app.c \
  $libs

cd $BUILD_OUT
mkdir include lib
cp $BUILD_PKG/sokol_app.h include
mv $HOME/$(zigi lib sokol_app) lib
pkg-config --gendefault sokol_app
