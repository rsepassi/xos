suffix="tar.gz"
case $ARCH_OS in
  macos)
    os="darwin"
    ;;
  windows)
    os="windows"
    suffix="zip"
    ;;
  *)
    os=$ARCH_OS
    ;;
esac

case $ARCH_ISA in
  aarch64)
    isa="arm64"
    ;;
  x86_64)
    isa="amd64"
    ;;
  *)
    >&2 echo "unrecognized isa $ARCH_ISA"
    exit 1
    ;;
esac

version="1.22.0"

hash_darwin_amd64="ebca81df938d2d1047cc992be6c6c759543cf309d401b86af38a6aed3d4090f4"
hash_darwin_arm64="bf8e388b09134164717cd52d3285a4ab3b68691b80515212da0e9f56f518fb1e"
hash_linux_amd64="f6c8a87aa03b92c4b0bf3d558e28ea03006eb29db78917daec5cfb6ec1046265"
hash_linux_arm64="6a63fef0e050146f275bf02a0896badfe77c11b6f05499bb647e7bd613a45a10"
hash_windows_amd64="78b3158fe3aa358e0b6c9f26ecd338f9a11441e88bc434ae2e9f0ca2b0cc4dd3"
hash_windows_arm64="31a61e41d06a3bb2189a303f5f3e777ca4b454eff439f0a67bc2b166330021f4"

url="https://go.dev/dl/go$version.$os-$isa.$suffix"
eval "hash=\$hash_${os}_${isa}"

file="golang.$suffix"
fetch "$url" $file "$hash"

# output directory structure
# /bin/{go,goi}
# /golang/*

mkdir "$BUILD_OUT/golang"
if [ "$ARCH_OS" = "windows" ]
then
  tmp=$(mktemp -d)
  cd $tmp
  unzip -q "$BUILD_DEPS/$file"
  mv ./* "$BUILD_OUT/golang"
else
  untar "$BUILD_DEPS/$file" "$BUILD_OUT/golang"
fi

mkdir "$BUILD_OUT/bin"
cp \
  "$BUILD_PKG/go" \
  "$BUILD_PKG/gobuild" \
  "$BUILD_PKG/goi" \
  "$BUILD_PKG/gocc" \
  "$BUILD_PKG/goc++" \
  "$BUILD_OUT/bin"
