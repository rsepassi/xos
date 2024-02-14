#!/usr/bin/env sh

# To get config.h

configure() {
  os=$1
  isa=$2
  arch=$3
  ./configure --host=$isa-$os --build=aarch64-darwin --enable-static \
    --disable-shared --disable-dependency-tracking --disable-rpath \
    --without-png CC="cc --target=$arch"
  cp config.h arch/config-$os.h
}

mkdir -p arch
configure darwin aarch64 aarch64-macos
configure windows aarch64 aarch64-windows-gnu
configure linux aarch64 aarch64-linux-musl
