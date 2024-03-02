need mbedtls
need brotli
need nghttp2
need zlib
need zstd
need c-ares
need libssh2
need cacert

# fetch source
file="curl.tar.gz"
fetch "https://github.com/curl/curl/releases/download/curl-8_6_0/curl-8.6.0.tar.xz" \
  $file \
  "3ccd55d91af9516539df80625f818c734dc6f2ecf9bada33c76765e99121db15"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  zigopt="-Dsysroot=$BUILD_DEPS/macossdk/sdk"
fi

# build
cp "$BUILD_PKG/build.zig" .
cp "$BUILD_PKG/arch/config-$TARGET_OS.h" lib/curl_config.h
zig build -Doptimize=$OPT_ZIG -Dtarget=$TARGET $zigopt \
  -Dmbedtls="$BUILD_DEPS/mbedtls" \
  -Dbrotli="$BUILD_DEPS/brotli" \
  -Dnghttp2="$BUILD_DEPS/nghttp2" \
  -Dzlib="$BUILD_DEPS/zlib" \
  -Dzstd="$BUILD_DEPS/zstd" \
  -Dcares="$BUILD_DEPS/c-ares" \
  -Dssh2="$BUILD_DEPS/libssh2"

# install
mv $PWD/zig-out/bin "$BUILD_OUT"
mv $PWD/zig-out/lib "$BUILD_OUT"
mkdir -p "$BUILD_OUT/include/curl"
cp "$src/include/curl"/*.h "$BUILD_OUT/include/curl"
ln -s "$BUILD_DEPS/cacert/share" "$BUILD_OUT"
