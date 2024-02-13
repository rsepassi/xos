# fetch source
file="curl.tar.gz"
fetch "https://github.com/curl/curl/releases/download/curl-8_6_0/curl-8.6.0.tar.xz" \
  $file \
  "3ccd55d91af9516539df80625f818c734dc6f2ecf9bada33c76765e99121db15"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cp "$BUILD_PKG/build.zig" .
cp "$BUILD_PKG/arch/config-$ARCH_OS.h" lib/curl_config.h

if [ "$ARCH_OS" = "macos" ]
then
  need macossdk
  zigopt="-Dsysroot=$BUILD_DEPS/macossdk"
fi

zig build -Doptimize=$OPT_ZIG -Dtarget=$ARCH $zigopt

# install
ln -s $PWD/zig-out/bin "$BUILD_OUT/bin"
ln -s $PWD/zig-out/lib "$BUILD_OUT/lib"
mkdir -p "$BUILD_OUT/include/curl"
cp "$src/include/curl"/*.h "$BUILD_OUT/include/curl"
