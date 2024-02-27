needtool make
make=$BUILD_TOOLDEPS/make/bin/make

if [ "$TARGET_OS" = "windows" ]
then
  tarfile=busybox-w32.tar.gz
  url="https://github.com/rmyorston/busybox-w32/archive/refs/tags/FRP-5236-g7dff7f376.tar.gz"
  hash="8509aafaa404edc9f09e0d416322df0c1eae71acbfdd4574dce75d017afc0ddb"
  configfile=config-w32
  cflags="-Dclockid_t=int -DCLOCK_REALTIME=0 -Dffs=__builtin_ffs"
  ldflags="-lws2_32"
else
  tarfile=busybox.tar.bz2
  url="https://busybox.net/downloads/busybox-1.36.1.tar.bz2"
  hash="b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314"
  configfile=config
  cflags=""
  ldflags=""
fi


src=$(mktemp -d)
fetch "$url" "$tarfile" "$hash"
untar $BUILD_DEPS/$tarfile $src
cd $src

toolchaindir=$(mktemp -d)
cp $BUILD_PKG/gcc $toolchaindir
cp $BUILD_PKG/bbcross-ar $toolchaindir
cp $BUILD_PKG/bbcross-gcc $toolchaindir
export PATH="$toolchaindir:$PATH"
export BB_BUILD_TARGET=$TARGET

cp $BUILD_PKG/$configfile .config
if [ "$TARGET_OS" = "macos" ]
then
  # this is just for bootstrapping
  sed=sed
  if which gsed > /dev/null
  then
    sed=gsed
  fi
  $sed -i 's/CONFIG_NPROC=y/# CONFIG_NPROC is not set/g' .config
fi

if [ "$TARGET_OS" != "windows" ]
then
  cp $BUILD_PKG/platform.h include/platform.h
fi

if [ "$TARGET_OS" = "windows" ]
then
objs="
applets/built-in.o
win32/resources/built-in.o
archival/lib.a
archival/libarchive/lib.a
console-tools/lib.a
coreutils/lib.a
coreutils/libcoreutils/lib.a
debianutils/lib.a
klibc-utils/lib.a
e2fsprogs/lib.a
editors/lib.a
findutils/lib.a
init/lib.a
libbb/lib.a
libpwdgrp/lib.a
loginutils/lib.a
mailutils/lib.a
miscutils/lib.a
modutils/lib.a
networking/lib.a
networking/libiproute/lib.a
networking/udhcp/lib.a
printutils/lib.a
procps/lib.a
runit/lib.a
selinux/lib.a
shell/lib.a
sysklogd/lib.a
util-linux/lib.a
util-linux/volume_id/lib.a
win32/lib.a
archival/built-in.o
archival/libarchive/built-in.o
console-tools/built-in.o
coreutils/built-in.o
coreutils/libcoreutils/built-in.o
debianutils/built-in.o
klibc-utils/built-in.o
e2fsprogs/built-in.o
editors/built-in.o
findutils/built-in.o
init/built-in.o
libbb/built-in.o
libpwdgrp/built-in.o
loginutils/built-in.o
mailutils/built-in.o
miscutils/built-in.o
modutils/built-in.o
networking/built-in.o
networking/libiproute/built-in.o
networking/udhcp/built-in.o
printutils/built-in.o
procps/built-in.o
runit/built-in.o
selinux/built-in.o
shell/built-in.o
sysklogd/built-in.o
util-linux/built-in.o
util-linux/volume_id/built-in.o
win32/built-in.o
"
else
objs="
applets/built-in.o
archival/lib.a
archival/libarchive/lib.a
console-tools/lib.a
coreutils/lib.a
coreutils/libcoreutils/lib.a
debianutils/lib.a
editors/lib.a
findutils/lib.a
libbb/lib.a
libpwdgrp/lib.a
miscutils/lib.a
networking/lib.a
procps/lib.a
shell/lib.a
util-linux/lib.a
"
fi

for obj in $objs
do
CROSS_COMPILE="bbcross-" \
CFLAGS="$cflags" \
  $make -j$(nproc) $obj
done

echo "linking..."
mkdir -p $BUILD_OUT/bin
cc --target=$TARGET -s -O$OPT -o $BUILD_OUT/bin/busybox \
  $(echo $objs) $ldflags -lc
