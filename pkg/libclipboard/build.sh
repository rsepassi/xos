src=$(fetch_untar "https://api.github.com/repos/jtanx/libclipboard/tarball/3d2cb08" \
  libclipboard.tar.gz \
  "5b52244d5c7ab7d6fdad62a784a6d605c4fe5cdaa2eeb62c5f3646d1f08fa205")
cd $src

suffix=$(wrensh -c "$(cat <<EOF
var srcs = {
  "macos": "cocoa",
  "linux": "x11",
  "windows": "win32",
}
IO.write(srcs[IO.env("TARGET_OS")])
EOF
)")

def=$(wrensh -c "$(cat <<EOF
var defs = {
  "macos": "LIBCLIPBOARD_BUILD_COCOA",
  "linux": "LIBCLIPBOARD_BUILD_X11",
  "windows": "LIBCLIPBOARD_BUILD_WIN32",
}
IO.write(defs[IO.env("TARGET_OS")])
EOF
)")

if [ "$TARGET_OS" = "linux" ]
then
  #need linuxsdk -- libxcb-dev xcb
  #sdk="$BUILD_DEPS/linuxsdk"
  sdk="/usr"
  cflags="-I$sdk/include"
  ldflags="-L$sdk/lib -lxcb"
elif [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="$(pkg-config --cflags macossdk) -x objective-c"
fi

os_src="src/clipboard_$suffix.c"

touch clipboard.c
touch include/libclipboard-config.h
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I include \
  -D${def} \
  $cflags \
  clipboard.c \
  src/clipboard_common.c \
  $os_src \
  -lc

cd "$BUILD_OUT"
mkdir include lib
cp "$src/include/"*.h include
mv "$src/$(zigi lib clipboard)" lib
pkg-config --gendefault clipboard --ldflags "$ldflags"
