need nuklear  # for demo.c

url="https://api.github.com/repos/floooh/sokol/tarball/55dff3d"
hash="8ab0cf0fbe1579002f6eb04a0612e628c6bdf7ab6416b092134012ba9e73717d"
file="sokol.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

fetch "https://raw.githubusercontent.com/edubart/sokol_gp/master/sokol_gp.h" \
  sokol_gp.h \
  "91a092b7b103f55b7a26aa1098d2d35155b6924298d5516dc281ac1d75cc4472"

sokol_file=sokol.c
if [ "$TARGET_OS" = "linux" ]
then
  need linuxsdk -- alpine mesa-dev,libxi-dev,libxcursor-dev GL,EGL,X11,Xi,Xcursor
  sdk="$BUILD_DEPS/linuxsdk"
  cflags="-DSOKOL_GLES3 -I$sdk/include"
  ldflags="
  -L$sdk/lib
  -lGL -lEGL -lX11 -lXi -lXcursor
  "
elif [ "$TARGET_OS" = "macos" ]
then
  sokol_file=sokol.m
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-DSOKOL_METAL $(pkg-config --cflags macossdk)"
  frameworks="
  Metal
  MetalKit
  Cocoa
  "
  frameworks_plus="
  AppKit
  CoreData
  ApplicationServices
  Foundation
  ColorSync
  CoreGraphics
  CoreServices
  CoreText
  CoreFoundation
  CFNetwork
  ImageIO
  QuartzCore
  CoreImage
  CoreVideo
  "
  all_frameworks="
  $frameworks
  $frameworks_plus
  "
  ldflags="-lobjc"
  for f in $all_frameworks
  do
    ldflags="$ldflags -framework $f"
  done
elif [ "$TARGET_OS" = "ios" ]
then
  zigsdk=iossdk
	sokol_file=sokol.m
  need iossdk
  sdk="$BUILD_DEPS/iossdk/sdk"
  cflags="-DSOKOL_METAL $(pkg-config --cflags iossdk)"
  all_frameworks="
  Metal
  MetalKit
  Foundation
  AudioToolbox
  UIKit
  "
  ldflags=""
  for f in $all_frameworks
  do
    ldflags="$ldflags -framework $f"
  done
elif [ "$TARGET_OS" = "windows" ]
then
  cflags="-DSOKOL_D3D11"
  ldflags="-lkernel32 -luser32 -ldxgi -ld3d11 -lole32 -lgdi32"
fi

cat <<EOF > $sokol_file
#define SOKOL_IMPL
#define SOKOL_DEBUG
#define SOKOL_WIN32_FORCE_MAIN
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_gp.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "util/sokol_debugtext.h"
#include "util/sokol_shape.h"
#include "util/sokol_color.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  -I $BUILD_DEPS \
  $cflags \
  $sokol_file \
  -lc

cd "$BUILD_OUT"
mkdir lib include
mv \
  "$src/sokol_app.h" \
  "$src/sokol_gfx.h" \
  "$src/sokol_log.h" \
  "$src/sokol_glue.h" \
  "$src/util/sokol_debugtext.h" \
  "$src/util/sokol_shape.h" \
  "$src/util/sokol_color.h" \
  "$src/util/sokol_nuklear.h" \
  "$BUILD_DEPS/sokol_gp.h" \
  include
mv "$src/$(zigi lib sokol)" lib
pkg-config --gendefault sokol --cflags "$cflags" --ldflags "$(echo $ldflags)"

# demo app
cd "$HOME"
zig build-exe -target $TARGET -O $OPT_ZIG \
  $(zigi libc $zigsdk) \
  -I"$BUILD_OUT"/include \
  -I"$BUILD_DEPS/nuklear/include" \
  $cflags \
  $BUILD_PKG/demo.c \
  "$BUILD_OUT/lib/$(zigi lib sokol)" \
  $ldflags \
  -lc
mkdir "$BUILD_OUT/bin"
mv $(zigi exe demo) "$BUILD_OUT/bin"
