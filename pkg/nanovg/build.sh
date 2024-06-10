src=$(fetch_untar "https://api.github.com/repos/memononen/nanovg/tarball/f93799c" \
  nanovg.tar.gz \
  "9972acabedd7e2f40f897de5af1c548479f74c7cd4cc059c2747f77d7a3eb279")
cd $src/src

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name nanovg \
  -DNVG_NO_STB \
  nanovg.c \
  -lc

cd $BUILD_OUT
mkdir lib include
mv $src/src/$(zigi lib nanovg) lib
mv $src/src/nanovg.h include
pkg-config --gendefault nanovg


