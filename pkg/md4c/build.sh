# fetch
src=$(fetch_untar https://github.com/mity/md4c/archive/refs/tags/release-0.5.1.tar.gz \
  md4c.tar.gz \
  "2dca17c6175a7f11182943079c2a4f9adb5071433e3d3d05ba801ff794993f34")
cd $src/src

# md4c library
zig build-lib -target $TARGET -O $OPT_ZIG md4c.c md4c-html.c entity.c -lc
lib=$src/src/$(zigi lib md4c)

# md2html binary
cd $src/md2html
rm -f md2html
cc -static -s -O$OPT --target=$TARGET -o md2html \
  -fstack-protector \
  -I $src/src \
  -D MD_VERSION_MAJOR=0 \
  -D MD_VERSION_MINOR=5 \
  -D MD_VERSION_RELEASE=1 \
  cmdline.c md2html.c \
  $lib \
  -lc

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/bin $out/include
cp $lib $out/lib
cp $src/src/md4c.h $out/include
cp $src/src/md4c-html.h $out/include
cp $src/md2html/md2html $out/bin
