set -e

urlf=$BUILD_PKG/url-$ARCH_OS-$ARCH_ISA.txt
if [ "$ARCH_OS" = "windows" ]
then
file="zig-$ARCH_OS-$ARCH_ISA.zip"
decomp=unzip
else
file="zig-$ARCH_OS-$ARCH_ISA.tar.xz"
decomp=untar
fi

fetch_urltxt $urlf $file
$decomp $BUILD_DEPS/$file $BUILD_OUT
