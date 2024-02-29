url="https://www.bytereef.org/software/mpdecimal/releases/mpdecimal-4.0.0.tar.gz"
hash="942445c3245b22730fd41a67a7c5c231d11cb1b9936b9c0f76334fb7d0b4468c"
file="mpdec.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cp "$BUILD_PKG/config.h" libmpdec

sed -i 's/@MPD_HEADER_CONFIG@/#define MPD_CONFIG_64 1/' libmpdec/mpdecimal.h.in
mv libmpdec/mpdecimal.h.in libmpdec/mpdecimal.h

asm="-DASM"
if [ "$TARGET_ARCH" != "x86_64" ]
then
  asm="-DANSI"
fi

touch mpdec.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DCONFIG_64 $asm \
  -Ilibmpdec \
  mpdec.c libmpdec/*.c -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$src/libmpdec/mpdecimal.h" include
mv "$src/$(zigi lib mpdec)" lib
