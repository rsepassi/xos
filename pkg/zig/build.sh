urlf=$BUILD_PKG/url-$ARCH_OS-$ARCH_ISA.txt

if [ "$ARCH_OS" = "windows" ]
then
  file="zig-$ARCH_OS-$ARCH_ISA.zip"
  fetch_urltxt $urlf $file
  tmp=$(mktemp -d)
  cd $tmp
  unzip -q "$BUILD_DEPS/$file"
  cd $(ls)
  mv ./* "$BUILD_OUT"
else
  file="zig-$ARCH_OS-$ARCH_ISA.tar.xz"
  fetch_urltxt $urlf $file
  untar "$BUILD_DEPS/$file" "$BUILD_OUT"
fi

