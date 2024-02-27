version="0.12.0-dev.2341+92211135f"
suffix="tar.xz"
if [ "$TARGET_OS" = "windows" ]
then
  suffix="zip"
fi

url="https://ziglang.org/builds/zig-$TARGET_OS-$TARGET_ARCH-$version.$suffix"

case "$TARGET_OS-$TARGET_ARCH" in
  "linux-aarch64")
    hash="0d0aa58c499bd35dd72edfbbe87b50981feba56cb30b3681deb9dbd4608090f9"
    ;;
  "linux-x86_64")
    hash="8145e518decedc2e1dfe9f55d547e4103b0dd68a2f24527c4da9f8b31b3deaba"
    ;;
  "macos-aarch64")
    hash="77a948acb0048c57b66ef791039b09ea2f55663543bb7cef4b04c0b22a3696e6"
    ;;
  "macos-x86_64")
    hash="1ece29c81cc388ebc213c8e20f564127f5d8077d2d1a97404a141a6a49fcf8ad"
    ;;
  "windows-x86_64")
    hash="1f48cd851871e154895ef67b52aeaf2db8835725007bd3816b492216fde26af2"
    ;;
  "windows-aarch64")
    hash="fc0c527003c3d853899f3c93ed7e16474ac16cc9fa9962e285614cb928f0a1ee"
    ;;
  *)
    >&2 echo "unsupported zig target $TARGET_OS $TARGET_ARCH"
esac

file="zig-$TARGET_OS-$TARGET_ARCH.$suffix"
fetch "$url" "$file" "$hash"

if [ "$TARGET_OS" = "windows" ]
then
  tmp=$(mktemp -d)
  cd $tmp
  unzip -q "$BUILD_DEPS/$file"
  cd $(ls)
  mv ./* "$BUILD_OUT"
else
  untar "$BUILD_DEPS/$file" "$BUILD_OUT"
fi
