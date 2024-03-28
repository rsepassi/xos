fetch \
  "https://raw.githubusercontent.com/termbox/termbox2/a8e2644/termbox2.h" \
  termbox2.h \
  "c11c247467585874d56d7426310e5a56295cd314b82260410615c166474b6f78"

path=$(realpath $BUILD_DEPS/termbox2.h)

cc --target=$TARGET -c -x c $path -DTB_IMPL \
  -o termbox2.o -lc
ar rcs libtermbox2.a termbox2.o

mkdir -p $BUILD_OUT/lib $BUILD_OUT/include
mv libtermbox2.a $BUILD_OUT/lib
cp $BUILD_DEPS/termbox2.h $BUILD_OUT/include
