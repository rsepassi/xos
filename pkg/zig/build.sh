#!/bin/sh
set -e

urlf=$BUILD_PKG/url-$ARCH_OS-$ARCH_ISA.txt
if [ "$ARCH_OS" = "windows" ]
file="zig-$ARCH_OS-$ARCH_ISA.zip"
decomp=unzip
then
file="zig-$ARCH_OS-$ARCH_ISA.tar.xz"
decomp=untar
fi

fetch_urltxt $urlf $BUILD_DL/$file
$decomp $BUILD_DL/$file $BUILD_OUT

