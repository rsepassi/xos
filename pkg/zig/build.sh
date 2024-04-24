version="0.12.0"
suffix="tar.xz"
if [ "$TARGET_OS" = "windows" ]
then
  suffix="zip"
fi

url="https://ziglang.org/builds/zig-$TARGET_OS-$TARGET_ARCH-$version.$suffix"

case "$TARGET_OS-$TARGET_ARCH" in
  "linux-aarch64")
    hash="754f1029484079b7e0ca3b913a0a2f2a6afd5a28990cb224fe8845e72f09de63"
    ;;
  "linux-x86_64")
    hash="c7ae866b8a76a568e2d5cfd31fe89cdb629bdd161fdd5018b29a4a0a17045cad"
    ;;
  "macos-aarch64")
    hash="294e224c14fd0822cfb15a35cf39aa14bd9967867999bf8bdfe3db7ddec2a27f"
    ;;
  "macos-x86_64")
    hash="4d411bf413e7667821324da248e8589278180dbc197f4f282b7dbb599a689311"
    ;;
  "windows-x86_64")
    hash="2199eb4c2000ddb1fba85ba78f1fcf9c1fb8b3e57658f6a627a8e513131893f5"
    ;;
  "windows-aarch64")
    hash="04c6b92689241ca7a8a59b5f12d2ca2820c09d5043c3c4808b7e93e41c7bf97b"
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
