# options
pkg=$2
if [ -z $pkg ]
then
  echo "xbuild requires passing a pkg"
  exit 1
fi
shift  # xbuild
shift  # pkg

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
  pkgid=$(ARCH=$arch OPT=$OPT need $pkg $arch -- "$@")
  ln -s $(realpath $BUILD_DEPS/$arch) $BUILD_OUT/$arch
done
