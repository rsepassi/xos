set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

file=samurai.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src
cd $src

mkdir $BUILD_OUT/bin
zig cc --target=$ARCH -s -static -O$OPT -o $BUILD_OUT/bin/samu *.c
