src=$(fetch_untar "https://blend2d.com/download/blend2d-0.11.1.tar.gz" \
  blend2d.tar.gz \
  "f46d61b6aa477fea1a353a41f5906d4e861817ae059ed22fc6ecdd50ff859dd2")
cd $src

rm -rf test
find . -type f -name '*_test.cpp' | xargs rm
srcs="$(find . -type f -name '*.cpp')"

if [ "$TARGET_ARCH" = "x86_64" ]
then
  cflags="-DBL_BUILD_OPT_SSE2"
elif [ "$TARGET_ARCH" = "aarch64" ]
then
  cflags="-DBL_BUILD_OPT_ASIMD"
fi

# TODO:
# * vector extensions
# * jit

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name blend2d \
  -DBL_BUILD_NO_STDCXX \
  -DBL_STATIC \
  -DBL_BUILD_NO_JIT \
  -Isrc \
  $cflags \
  $srcs \
  -lc++ \
  -lc

cd $BUILD_OUT
mkdir lib include
cp $src/src/*.h include
mv $src/$(zigi lib blend2d) lib
pkg-config --gendefault blend2d
