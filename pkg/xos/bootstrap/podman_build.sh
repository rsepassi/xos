#!/usr/bin/env sh

podman build \
  -f ./pkg/xos/bootstrap/build.containerfile \
  -t xos-bootstrap \
  .

rm -rf bootstrap_out; mkdir -p bootstrap_out
podman run \
  -e "BOOTSTRAP_OUT=/root/bootstrap_out" \
  -v $PWD/bootstrap_out:/root/bootstrap_out \
  xos-bootstrap
