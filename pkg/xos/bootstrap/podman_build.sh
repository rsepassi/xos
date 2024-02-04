#!/usr/bin/env sh

podman build \
  -f ./pkg/xos/bootstrap/build.containerfile \
  -t xos-bootstrap \
  .

rm -rf build; mkdir -p build

podman run \
  -v $PWD/build:/root/xos/build \
  xos-bootstrap
