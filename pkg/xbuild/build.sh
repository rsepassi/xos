#!/bin/sh
set -e

# options
pkg=$2
if [ -z $pkg ]
then
  echo "xbuild requires passing a pkg"
  exit 1
fi

archs="
aarch64-linux-musl
x86_64-linux-musl
x86_64-macos
aarch64-macos
x86_64-windows-gnu
aarch64-windows-gnu
"

for arch in $archs
do
  pkgid=$(ARCH=$arch $BUILD_ROOT/cmd/build $pkg)
  ln -s $BUILD_CACHE/$pkgid $BUILD_OUT/$arch
done
