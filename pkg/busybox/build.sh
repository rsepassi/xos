#!/bin/sh
set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

fetch_urltxt $BUILD_PKG/url.txt $BUILD_DL/busybox.tar.bz2
src=$(mktemp -d)
untar $BUILD_DL/busybox.tar.bz2 $src

cd $src

toolchaindir=$(mktemp -d)
cp $BUILD_PKG/gcc $toolchaindir
cp $BUILD_PKG/bbcross-ar $toolchaindir
cp $BUILD_PKG/bbcross-gcc $toolchaindir
export PATH="$toolchaindir:$PATH"
export BB_BUILD_ARCH=$ARCH

cp $BUILD_PKG/config .config
cp $BUILD_PKG/platform.h include/platform.h

objs="
applets/built-in.o
archival/lib.a
archival/libarchive/lib.a
coreutils/lib.a
coreutils/libcoreutils/lib.a
debianutils/lib.a
editors/lib.a
findutils/lib.a
libbb/lib.a
libpwdgrp/lib.a
miscutils/lib.a
procps/lib.a
shell/lib.a
"
for obj in $objs
do
CROSS_COMPILE="bbcross-" make -j16 $obj
done

echo "linking..."
zig cc --target=$ARCH -s -static -Os -o busybox \
  $(echo $objs)

mkdir -p $BUILD_OUT/bin
cp busybox $BUILD_OUT/bin/busybox

