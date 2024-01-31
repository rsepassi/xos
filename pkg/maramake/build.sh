set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

file=maramake.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $BUILD_DL/$file
src=$(mktemp -d)
untar $BUILD_DL/$file $src
cd $src

if [ "$ARCH_OS" = "macos" ]
then
  cflags="-Dst_mtim=st_mtimespec -include stat-macos.h"
  cp $BUILD_PKG/stat-macos.h .
fi
cflags="$cflags -DENABLE_FEATURE_MAKE_EXTENSIONS=1 -DENABLE_FEATURE_MAKE_POSIX_202X=1"

mkdir $BUILD_OUT/bin
zig cc --target=$ARCH -s -static -O$OPT -o $BUILD_OUT/bin/maramake *.c $cflags
