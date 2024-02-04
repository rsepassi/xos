# fetch
fetch_urltxt $BUILD_PKG/url.txt md4c.tar.gz

# untar
src=$(mktemp -d)
untar $BUILD_DEPS/md4c.tar.gz $src

# md4c library
cd $src/src
cc --target=$ARCH -o entity.o -c entity.c -lc
cc --target=$ARCH -o md4c-html.o -c md4c-html.c -lc
cc --target=$ARCH -o md4c.o -c md4c.c -DMD4C_USE_UTF8 -lc
ar rcs libmd4c.a entity.o md4c-html.o md4c.o

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
  $src/src/libmd4c.a \
  -lc

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/bin $out/include
cp $src/src/libmd4c.a $out/lib
cp $src/src/md4c.h $out/include
cp $src/src/md4c-html.h $out/include
cp $src/md2html/md2html $out/bin
