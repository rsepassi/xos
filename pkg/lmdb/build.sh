#!/bin/sh

set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

fetch_urltxt $BUILD_PKG/url.txt $BUILD_DL/lmdb.tar.gz
src=$(mktemp -d)
untar $BUILD_DL/lmdb.tar.gz $src 3

# lib
cd $src
zig cc --target=$ARCH -o mdb.o -c mdb.c
zig cc --target=$ARCH -o midl.o -c midl.c
zig ar rcs liblmdb.a \
    mdb.o \
    midl.o

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp $src/lmdb.h $out/include
cp $src/liblmdb.a $out/lib
