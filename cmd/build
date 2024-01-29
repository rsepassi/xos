#!/bin/sh
pkg=$1
if [ -z "$pkg" ]
then
  echo "must provide package"
  exit 1
fi
/bin/sh $PWD/pkg/$pkg/build.sh || echo error
