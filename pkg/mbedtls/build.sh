file=mbedtls.tar.gz
fetch_urltxt $BUILD_PKG/url.txt $file
src=$(mktemp -d)
untar $BUILD_DEPS/$file $src
cd $src

cp $BUILD_PKG/build.zig .
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG

mv $PWD/zig-out/lib $BUILD_OUT
find include -type f -name '*.h' -exec cp --parents {} $BUILD_OUT \;
pkg-config --gendefault mbedtls
pkg-config --gendefault mbedcrypto
pkg-config --gendefault mbedx509
