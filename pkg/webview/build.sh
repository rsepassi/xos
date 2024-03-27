url="https://api.github.com/repos/webview/webview/tarball/adbb85d"
hash="7b41b8efa9e2bdebbbec06ce406cb9932fcebb513090edacf9a79f2c987cc219"
file="webview.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "linux" ]
then
  if [ "$HOST_OS" != "linux" ]
  then
    >&2 echo "webview linux builds currently require running on linux"
    exit 1
  fi
  # TODO: use a prepackaged sdk
  cflags="
  $(system pkg-config --cflags gtk+-3.0)
  $(system pkg-config --cflags webkit2gtk-4.0)
  "
  ldflags="
  -L/usr/lib -L/lib
  $(system pkg-config --libs gtk+-3.0)
  $(system pkg-config --libs webkit2gtk-4.0)
  "
  ln -s /usr/include/X11 ./X11
elif [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-Wno-elaborated-enum-base"
  ldflags="-F$sdk/System/Library/Frameworks -L$sdk/usr/lib -framework WebKit -lobjc"
elif [ "$TARGET_OS" = "windows" ]
then
  need winsdk
  sdk="$BUILD_DEPS/winsdk"
  cflags="
  -I$sdk/WebView2/build/native/include
  -I$sdk/sdk-cpp/c/Include/10.0.22621.0/winrt
  "
  if [ "$TARGET_ARCH" = "aarch64" ]
  then
    arch=arm64
  elif [ "$TARGET_ARCH" = "x86_64" ]
  then
    arch=x64
  fi
  ldflags="
  $sdk/WebView2/build/native/$arch/WebView2Loader.dll.lib
  -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion
  "
fi

cat <<EOF > basic.c
#include "webview.h"
#include <stddef.h>

int main() {
  webview_t w = webview_create(0, NULL);
  webview_set_title(w, "Basic Example");
  webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(w, "Welcome to webview!");
  webview_run(w);
  webview_destroy(w);
  return 0;
}
EOF

cc -target $TARGET -O$OPT \
  -DWEBVIEW_STATIC \
  -I. \
  $cflags \
  -c -o webview.o webview.cc \
  -lc++
ar rcs $(zigi lib webview) webview.o

cc -target $TARGET -O$OPT \
  -I. \
  -o $(zigi exe basic) \
  basic.c \
  $(zigi lib webview) \
  $ldflags \
  -lc++

cd "$BUILD_OUT"
mkdir lib include bin pkgconfig
mv "$src/webview.h" include
mv "$src/$(zigi lib webview)" lib
mv "$src/$(zigi exe basic)" bin
cat <<EOF > pkgconfig/webview.pc
Cflags: -I\${rootdir}/include
Libs: \${rootdir}/lib/$(zigi lib webview) $ldflags -lc++
EOF

