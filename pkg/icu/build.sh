url="https://github.com/unicode-org/icu/releases/download/release-74-2/icu4c-74_2-src.tgz"
hash="68db082212a96d6f53e35d60f47d38b962e9f9d207a74cfac78029ae8ff5e08c"
file="icu.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
src="$src/source"
cd "$src"

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-cflags -std=c++11 -DTARGET_OS_OSX -I$sdk/usr/include/c++/v1 -I$sdk/usr/include --"
else
  cflags="-cflags -std=c++11 --"
fi

cflags="
-DU_ENABLE_DYLOAD=0
-DU_DEFAULT_SHOW_DRAFT=0
-DU_SHOW_DRAFT_API
-DU_SHOW_INTERNAL_API
-DU_STATIC_IMPLEMENTATION
-DU_DISABLE_RENAMING=1
$cflags
"

cd "$src/stubdata"
touch icudata.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I../common \
  $cflags \
  icudata.cpp *.cpp \
  -lc++ -lc

cd "$src/io"
touch icuio.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_IO_IMPLEMENTATION \
  -I. -I../common -I../i18n \
  $cflags \
  icuio.cpp *.cpp \
  -lc++

cd "$src/common"
touch icuuc.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_COMMON_IMPLEMENTATION \
  -I. \
  $cflags \
  icuuc.cpp *.cpp \
  -lc++

cd "$src/i18n"
touch icui18n.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_I18N_IMPLEMENTATION \
  $cflags \
  -I. -I../common \
  -cflags -std=c++11 -- \
  icui18n.cpp *.cpp \
  -lc++

cd "$BUILD_OUT"
mkdir lib
mv \
  "$src/common/$(zigi lib icuuc)" \
  "$src/i18n/$(zigi lib icui18n)" \
  "$src/io/$(zigi lib icuio)" \
  "$src/stubdata/$(zigi lib icudata)" \
  lib
mkdir -p include/unicode
cp \
  "$src/common/unicode/"*.h \
  "$src/i18n/unicode/"*.h \
  "$src/io/unicode/"*.h \
  include/unicode
pkg-config --gendefault icuuc
