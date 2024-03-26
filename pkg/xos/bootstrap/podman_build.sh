#!/usr/bin/env sh

build="build/bootstrap"
mkdir -p $build
podman run \
  -e "BOOTSTRAP_BUILD=/root/xos/$build" \
  -e "BOOTSTRAP_CONTAINER_BUILD=1" \
  -v "$PWD/$build":/root/xos/$build \
  -v "$PWD/pkg":/root/xos/pkg:ro \
  -w /root/xos \
  alpine:3.19 \
  /root/xos/pkg/xos/bootstrap/build.sh
./build/bootstrap/out/build xos
