#!/bin/sh

set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

mkdir -p $BUILD_OUT/bin
zig cc --target=$ARCH -s -static -o $BUILD_OUT/bin/wrensh \
  -I $BUILD_DEPS/wren/include \
  $BUILD_PKG/wrensh.c \
  $BUILD_DEPS/wren/lib/libwren.a
