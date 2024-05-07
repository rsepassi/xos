src=$(fetch_untar "https://github.com/Mbed-TLS/mbedtls/archive/refs/tags/v3.5.2.tar.gz" \
mbedtls.tar.gz \
"35890edf1a2c7a7e29eac3118d43302c3e1173e0df0ebaf5db56126dabe5bb05")
cd $src

cp $BUILD_PKG/build.zig .
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv $PWD/zig-out/lib $BUILD_OUT
mv $PWD/zig-out/bin $BUILD_OUT
find include -type f -name '*.h' -exec cp --parents {} $BUILD_OUT \;
pkg-config --gendefault mbedtls
pkg-config --gendefault mbedcrypto
pkg-config --gendefault mbedx509
