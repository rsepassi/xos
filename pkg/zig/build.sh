urlf=$BUILD_PKG/url-$TARGET_OS-$TARGET_ARCH.txt

if [ "$TARGET_OS" = "windows" ]
then
  file="zig-$TARGET_OS-$TARGET_ARCH.zip"
  fetch_urltxt $urlf $file
  tmp=$(mktemp -d)
  cd $tmp
  unzip -q "$BUILD_DEPS/$file"
  cd $(ls)
  mv ./* "$BUILD_OUT"
else
  file="zig-$TARGET_OS-$TARGET_ARCH.tar.xz"
  fetch_urltxt $urlf $file
  untar "$BUILD_DEPS/$file" "$BUILD_OUT"
fi

