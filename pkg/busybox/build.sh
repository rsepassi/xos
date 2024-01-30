#!/bin/sh
set -e

# options
ARCH=${ARCH:-native}
OPT=${OPT:-s}

fetch_urltxt $BUILD_PKG/url.txt $BUILD_DL/busybox.tar.bz2
src=$(mktemp -d)
untar $BUILD_DL/busybox.tar.bz2 $src

cd $src

xprefix="bbcross"

toolchaindir=$(mktemp -d)
cp $BUILD_PKG/gcc $toolchaindir
cp $BUILD_PKG/bbcross-ar $toolchaindir
cp $BUILD_PKG/bbcross-gcc $toolchaindir
export PATH="$toolchaindir:$PATH"
export BB_BUILD_ARCH=$ARCH

cp $BUILD_PKG/config .config
CROSS_COMPILE="$xprefix-" \
  make -j16 busybox_unstripped
zig objcopy -S busybox_unstripped busybox

mkdir -p $BUILD_OUT/bin
cp busybox $BUILD_OUT/bin/busybox
