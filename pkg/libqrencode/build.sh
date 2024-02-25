file="qrencode.tar.gz"
fetch "https://fukuchi.org/works/qrencode/qrencode-4.1.1.tar.gz" \
  $file \
  "da448ed4f52aba6bcb0cd48cac0dd51b8692bccc4cd127431402fca6f8171e8e"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cp "$BUILD_PKG/arch/config-$TARGET_OS.h" config.h
cp "$BUILD_PKG/build.zig" .
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv zig-out/lib "$BUILD_OUT"
mv zig-out/bin "$BUILD_OUT"
mkdir -p "$BUILD_OUT/include"
cp qrencode.h "$BUILD_OUT/include"
