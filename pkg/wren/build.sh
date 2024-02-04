fetch_urltxt $BUILD_PKG/url.txt wren.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/wren.tar.gz $src

# lib
cd $src/src/vm
cflags="-I $src/src/include -DWREN_OPT_META=0 -DWREN_OPT_RANDOM=0"
rm -f *.a *.o
cc --target=$ARCH $cflags -o wren_compiler.o -c wren_compiler.c -lc
cc --target=$ARCH $cflags -o wren_core.o -c wren_core.c -lc
cc --target=$ARCH $cflags -o wren_debug.o -c wren_debug.c -lc
cc --target=$ARCH $cflags -o wren_primitive.o -c wren_primitive.c -lc
cc --target=$ARCH $cflags -o wren_utils.o -c wren_utils.c -lc
cc --target=$ARCH $cflags -o wren_value.o -c wren_value.c -lc
cc --target=$ARCH $cflags -o wren_vm.o -c wren_vm.c -lc
ar rcs libwren.a \
    wren_compiler.o \
    wren_core.o \
    wren_debug.o \
    wren_primitive.o \
    wren_utils.o \
    wren_value.o \
    wren_vm.o

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp $src/src/include/wren.h $out/include
cp $src/src/vm/libwren.a $out/lib
