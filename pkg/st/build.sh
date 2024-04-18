src=$(fetch_untar "https://dl.suckless.org/st/st-0.9.2.tar.gz" \
  st.tar.gz \
  "6b215d4f472b21d6232f30f221117a777e24bcfee68955ddefb7426467f9494b")
cd $src

if [ "$TARGET_OS" != "linux" ]
then
  >&2 echo "st only runs on linux"
  exit 1
fi

need linuxsdk -- alpine libxft-dev,freetype-dev,fontconfig Xft,X11,fontconfig

pwd

cp "$BUILD_PKG/config.h" .
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DVERSION=\"0.0.1\" \
  -I "$BUILD_DEPS/linuxsdk/include" \
  -I "$BUILD_DEPS/linuxsdk/include/freetype2" \
  st.c x.c \
  -L "$BUILD_DEPS/linuxsdk/lib" \
  -lX11 -lXft -lfontconfig \
  -lc

cd "$BUILD_OUT"
mkdir bin
mv "$src/$(zigi exe st)" bin
