url="https://github.com/skvadrik/re2c/archive/refs/tags/3.1.tar.gz"
hash="087c44de0400fb15caafde09fd72edc7381e688a35ef505ee65e0e3d2fac688b"
file="re2c.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

find src -type f -name 'test.cc' | xargs rm

exesrcs() {
  find src -type f -name '*.cc'
  find bootstrap/src -type f -name '*.cc'
}

cp "$BUILD_PKG/config.h" .
touch re2c.cc
zig build-exe -target $ARCH -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DRE2C_STDLIB_DIR='""' \
  -I. -Ibootstrap \
  -cflags -std=c++11 -- \
  re2c.cc \
  $(exesrcs) \
  -lc++ -lc

libsrcs() {
  find lib -type f -name '*.cc'
  find bootstrap/lib -type f -name '*.cc'
}

zig build-lib -target $ARCH -O $OPT_ZIG \
  -DHAVE_CONFIG_H \
  -I. -Ibootstrap \
  -cflags -std=c++11 -- \
  re2c.cc \
  $(libsrcs) \
  -lc++ -lc

mkdir "$BUILD_OUT/bin" "$BUILD_OUT/lib" "$BUILD_OUT/include" "$BUILD_OUT/share"
mv $(zigi exe re2c) "$BUILD_OUT/bin"
mv $(zigi lib re2c) "$BUILD_OUT/lib"
cp lib/regex.h "$BUILD_OUT/include"
cp include/* "$BUILD_OUT/share"
