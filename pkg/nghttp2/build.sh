file="nghttp2.tar.xz"
fetch "https://github.com/nghttp2/nghttp2/releases/download/v1.59.0/nghttp2-1.59.0.tar.xz" \
  $file \
  "fdc9bd71f5cf8d3fdfb63066b89364c10eb2fdeab55f3c6755cd7917b2ec4ffb"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cp "$BUILD_PKG/build.zig" .
cp "$BUILD_PKG/arch/config-$TARGET_OS.h" config.h
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mkdir -p "$BUILD_OUT/include/nghttp2"
ln -s "$PWD/zig-out/lib" "$BUILD_OUT"
cp lib/includes/nghttp2/*.h "$BUILD_OUT/include/nghttp2"
