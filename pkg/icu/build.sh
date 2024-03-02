[ "$TARGET_OS" = "linux" ] || { >&2 echo "unimpl, only linux builds"; exit 1; }

url="https://github.com/unicode-org/icu/releases/download/release-74-2/icu4c-74_2-src.tgz"
hash="68db082212a96d6f53e35d60f47d38b962e9f9d207a74cfac78029ae8ff5e08c"
file="icu.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
src="$src/source"
cd "$src"

cflags="
-D_REENTRANT
-DU_ENABLE_DYLOAD=0
-DU_HAVE_ELF_H
-DU_DEFAULT_SHOW_DRAFT=0
-DU_HAVE_STRTOD_L=1
-DU_HAVE_XLOCALE_H=0
-DU_SHOW_DRAFT_API
-DU_SHOW_INTERNAL_API
-DU_ATTRIBUTE_DEPRECATED=
-DU_STATIC_IMPLEMENTATION
"

cd "$src/common"
touch icuuc.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_COMMON_IMPLEMENTATION \
  "-DDEFAULT_ICU_PLUGINS=\"\" " \
  $cflags \
  -I. \
  -cflags -std=c++11 -- \
  icuuc.cpp *.cpp \
  -lc++ -lc

cd "$src/i18n"
touch icui18n.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_I18N_IMPLEMENTATION \
  $cflags \
  -I. -I../common \
  -cflags -std=c++11 -- \
  icui18n.cpp *.cpp \
  -lc++ -lc

cd "$src/io"
touch icuio.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DU_IO_IMPLEMENTATION \
  $cflags \
  -I. -I../common -I../i18n \
  -cflags -std=c++11 -- \
  icuio.cpp *.cpp \
  -lc++ -lc

cd "$src/stubdata"
touch icudata.cpp
zig build-lib -target $TARGET -O $OPT_ZIG \
  $cflags \
  -I../common \
  -cflags -std=c++11 -- \
  icudata.cpp *.cpp \
  -lc++ -lc

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
