set -e

fetch_urltxt $BUILD_PKG/url.txt termbox2.h

path=$(realpath $BUILD_DEPS/termbox2.h)

cc --target=$ARCH -c -x c $path -DTB_IMPL \
  -o termbox2.o
ar rcs libtermbox2.a termbox2.o

mkdir -p $BUILD_OUT/lib $BUILD_OUT/include
mv libtermbox2.a $BUILD_OUT/lib
ln -s $BUILD_DEPS/termbox2.h $BUILD_OUT/include
