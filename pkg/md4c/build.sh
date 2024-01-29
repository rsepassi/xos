#!/bin/sh
set -e

# options
ARCH=${ARCH:-native}
OPT=${OPT:-s}

root=$PWD

# seal PATH
tools=$root/build/tools
rm -rf $tools
mkdir -p $tools
all_tools="
mkdir
rm
cp
tar
wget
zig
sha256sum
shasum
cut
"
for tool in $all_tools
do
  srcpath=$(which $tool || echo "")
  if [ -z "$srcpath" ]
  then
    echo "tool $tool does not exist"
    exit 1
  fi
  ln -s $srcpath $tools
done
export PATH="$root/build/tools"

# fetch
mkdir -p build/dl
if [ ! -e build/dl/md4c.tar.gz ]
then
  wget https://github.com/mity/md4c/archive/refs/tags/release-0.5.1.tar.gz -O build/dl/md4c.tar.gz
fi
hash_expected="2dca17c6175a7f11182943079c2a4f9adb5071433e3d3d05ba801ff794993f34"
hash_got="$(sha256sum build/dl/md4c.tar.gz | cut -d ' ' -f1)"
if [ "$hash_expected" != "$hash_got" ]
then
  echo "bad hash"
  exit 1
fi

# untar
src=$root/build/dl/md4c
rm -rf $src
mkdir -p $src
tar xf build/dl/md4c.tar.gz -C $src --strip-components=1

# md4c library
cd $src/src
rm -f *.a *.o
zig cc --target=$ARCH -o entity.o -c entity.c
zig cc --target=$ARCH -o md4c-html.o -c md4c-html.c
zig cc --target=$ARCH -o md4c.o -c md4c.c -DMD4C_USE_UTF8
zig ar rcs libmd4c.a entity.o md4c-html.o md4c.o

# md2html binary
cd $src/md2html
rm -f md2html
zig cc -static -s -O$OPT --target=$ARCH -o md2html \
  -fstack-protector \
  -I $src/src \
  -D MD_VERSION_MAJOR=0 \
  -D MD_VERSION_MINOR=5 \
  -D MD_VERSION_RELEASE=1 \
  cmdline.c md2html.c \
  $src/src/libmd4c.a \
  -lc

# install
cd $root
out=$root/build/out
rm -rf $out
mkdir -p $out/lib $out/bin $out/include
cp $src/src/libmd4c.a $out/lib
cp $src/src/md4c.h $out/include
cp $src/src/md4c-html.h $out/include
cp $src/md2html/md2html $out/bin
