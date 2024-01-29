#/bin/sh
set -e

pkg=md4c
archs="
aarch64-linux-musl
x86_64-linux-musl
x86_64-macos
aarch64-macos
x86_64-windows-gnu
aarch64-windows-gnu
"

rm -rf build/arch
mkdir -p build/arch
for arch in $archs
do
  echo $arch
  ARCH=$arch ./pkg/$pkg/build.sh
  cp -r build/out build/arch/$arch
done
