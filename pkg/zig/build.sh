version="0.12.0-dev.3457+341857e5c"
suffix="tar.xz"
if [ "$TARGET_OS" = "windows" ]
then
  suffix="zip"
fi

url="https://ziglang.org/builds/zig-$TARGET_OS-$TARGET_ARCH-$version.$suffix"

case "$TARGET_OS-$TARGET_ARCH" in
  "linux-aarch64")
    hash="81e30263b7eda5bf9ec88291d11193a37c0d7094ac44d2847041230e222a762f"
    ;;
  "linux-x86_64")
    hash="f8f13317a869ce1ae82868c20e032d204747d8fecd9da41b486324bf3ab52fab"
    ;;
  "macos-aarch64")
    hash="10216c1b377c00ef17cfbf2163ea7f32c824d459081adda055d4c5d76d0c54ee"
    ;;
  "macos-x86_64")
    hash="68d9c4fbd5bf4a536f4bc004c3bd4d03f799c926758bbede09af86a4db287451"
    ;;
  "windows-x86_64")
    hash="a9eee5616858961068822a8f5d9c5ba5797598394bde3df7c7d87805d4889514"
    ;;
  "windows-aarch64")
    hash="4405cd2155ad1ff7685251300d197733c5cc173417bd6d9dad0be7951dfe8e14"
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
