[ "$TARGET_OS" = "windows" ] && { >&2 echo "windows unsupported"; exit 1; }

url="https://github.com/kristapsdz/lowdown/archive/refs/tags/VERSION_1_1_0.tar.gz"
hash="5cc997f742fd9e3268a2bf15cb9c58bfa173b303bc13f5c61f67dedfff3bccce"
file="lowdown.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-I$sdk/usr/include"
fi

cp "$BUILD_PKG/config-$TARGET_OS.h" config.h

touch lowdown.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude -Isrc -Iklib -Iuthash \
  $cflags \
  lowdown.c \
  autolink.c buffer.c diff.c document.c entity.c gemini.c html.c \
  html_escape.c latex.c library.c libdiff.c nroff.c odt.c smartypants.c \
  term.c tree.c util.c compats.c \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  lowdown.c main.c \
  $(zigi lib lowdown) \
  -lc

cd "$BUILD_OUT"
mkdir include lib bin
cp "$src/lowdown.h" include
mv "$src/$(zigi lib lowdown)" lib
mv "$src/$(zigi exe lowdown)" bin
