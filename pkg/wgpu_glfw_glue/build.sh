need glfw
need wgpu

if [ "$TARGET_OS" = "windows" ]
then
  cflags="-DGLFW_EXPOSE_NATIVE_WIN32"
elif [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  cflags="-DGLFW_EXPOSE_NATIVE_COCOA $(pkg-config --cflags macossdk) -x objective-c"
elif [ "$TARGET_OS" = "linux" ]
then
  need linuxsdk -- alpine mesa-dev GL
  cflags="-DGLFW_EXPOSE_NATIVE_X11 $(pkg-config --cflags linuxsdk)"
fi

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name wgpu_glfw_glue \
  $cflags \
  $BUILD_PKG/glue.c \
  $(pkg-config --cflags glfw wgpu) \
  -lc

cd $BUILD_OUT
mkdir lib
mv $HOME/$(zigi lib wgpu_glfw_glue) lib
pkg-config --gendefault wgpu_glfw_glue
