url="https://www.lua.org/ftp/lua-5.4.6.tar.gz"
hash="7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88"
file="lua.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  cflags="-DLUA_USE_MACOSX"
elif [ "$TARGET_OS" = "linux" ]
then
  cflags="-DLUA_USE_LINUX"
elif [ "$TARGET_OS" = "windows" ]
then
  cflags="-DLUA_USE_WINDOWS"
fi

libsrcs="
src/lapi.c
src/lcode.c
src/lctype.c
src/ldebug.c
src/ldo.c
src/ldump.c
src/lfunc.c
src/lgc.c
src/llex.c
src/lmem.c
src/lobject.c
src/lopcodes.c
src/lparser.c
src/lstate.c
src/lstring.c
src/ltable.c
src/ltm.c
src/lundump.c
src/lvm.c
src/lzio.c
src/lauxlib.c
src/lbaselib.c
src/lcorolib.c
src/ldblib.c
src/liolib.c
src/lmathlib.c
src/loadlib.c
src/loslib.c
src/lstrlib.c
src/ltablib.c
src/lutf8lib.c
src/linit.c
"

touch lua.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DLUA_COMPAT_5_3 $cflags \
  lua.c $libsrcs -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -DLUA_COMPAT_5_3 $cflags \
  -Isrc \
  src/lua.c \
  $(zigi lib lua) \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -DLUA_COMPAT_5_3 $cflags \
  -Isrc \
  src/luac.c \
  $(zigi lib lua) \
  -lc

includes="
$src/src/lua.h
$src/src/luaconf.h
$src/src/lualib.h
$src/src/lauxlib.h
"

cd "$BUILD_OUT"
mkdir -p lib include bin
mv "$src/$(zigi lib lua)" lib
mv "$src/$(zigi exe lua)" bin
mv "$src/$(zigi exe luac)" bin
cp $includes include
pkg-config --gendefault lua
