url="https://api.github.com/repos/floooh/sokol/tarball/55dff3d"
hash="8ab0cf0fbe1579002f6eb04a0612e628c6bdf7ab6416b092134012ba9e73717d"
file="sokol.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

url="https://raw.githubusercontent.com/Immediate-Mode-UI/Nuklear/8e5c9f7/nuklear.h"
hash="6e2341fbdc9aff0d99e9938bcdba8bb8a8b3c51207846d20d6a2668d5d718a5d"
file="nuklear.h"
fetch "$url" "$file" "$hash"

mkdir -p include/nuklear
ln -s "$BUILD_DEPS/nuklear.h" include/nuklear

sokol_file="sokol.c"
if [ "$TARGET_OS" = "linux" ]
then
  # TODO: package up
  ln -s /usr/include/GLES3 include
  ln -s /usr/include/KHR include
  ln -s /usr/include/X11 include
  ln -s /usr/include/EGL include
  cflags="-DSOKOL_GLES3"
  ldflags="
  -L/usr/lib
  -lGL -lglfw -lEGL -lX11 -lXi -lXcursor
  "
elif [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  ln -s $sdk/usr/include/libDER include
  ln -s $sdk/usr/include/cups include
  cflags="-DSOKOL_METAL -F$sdk/System/Library/Frameworks -L$sdk/usr/lib"
  sokol_file="sokol.m"
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
elif [ "$TARGET_OS" = "windows" ]
then
  cflags="-DSOKOL_D3D11"
  ldflags="-lkernel32 -luser32 -lshell32 -ldxgi -ld3d11 -lole32 -lgdi32"
fi

cat <<EOF > $sokol_file
#define SOKOL_IMPL
#define SOKOL_WIN32_FORCE_MAIN
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#include "sokol_args.h"
#include "util/sokol_debugtext.h"
#include "util/sokol_shape.h"
#include "util/sokol_color.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_ZERO_COMMAND_MEMORY

#define NK_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "util/sokol_nuklear.h"
EOF

zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  $cflags \
  $sokol_file \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. -Iutil -Iinclude \
  $cflags \
  $BUILD_PKG/demo.c \
  $(zigi lib sokol) \
  $ldflags \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$BUILD_DEPS/nuklear.h" include
mv \
  "$src/sokol_app.h" \
  "$src/sokol_gfx.h" \
  "$src/sokol_log.h" \
  "$src/sokol_glue.h" \
  "$src/sokol_time.h" \
  "$src/sokol_args.h" \
  "$src/util/sokol_debugtext.h" \
  "$src/util/sokol_shape.h" \
  "$src/util/sokol_color.h" \
  include
mv "$src/$(zigi lib sokol)" lib
mv "$src/$(zigi exe demo)" bin
