# fetch
fetch_urltxt $BUILD_PKG/url.txt md4c.tar.gz

# untar
src=$(mktemp -d)
untar $BUILD_DEPS/md4c.tar.gz $src

# md4c library
cd $src/src
zig build-lib -target $ARCH -O $OPT_ZIG md4c.c md4c-html.c entity.c -lc
lib=$src/src/$(zigi lib md4c)

# md2html binary
cd $src/md2html
rm -f md2html
cc -static -s -O$OPT --target=$ARCH -o md2html \
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
