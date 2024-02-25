file=samurai.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src
cd $src

mkdir $BUILD_OUT/bin
cc --target=$TARGET -s -static -O$OPT -o $BUILD_OUT/bin/samu *.c -lc
