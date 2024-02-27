url="https://c-ares.org/download/c-ares-1.27.0.tar.gz"
hash="0a72be66959955c43e2af2fbd03418e82a2bd5464604ec9a62147e37aceb420b"
file="cares.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

if [ "$TARGET_OS" = "windows" ]
then
  ldflags="-lws2_32 -liphlpapi"
fi

touch cares.c
cp "$BUILD_PKG/arch/config-$TARGET_OS.h" src/lib/ares_config.h
cp "$BUILD_PKG/arch/build-$TARGET_OS.h" include/ares_build.h
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DCARES_BUILDING_LIBRARY -DCARES_STATICLIB \
  -Iinclude -Isrc/lib \
  cares.c \
  src/lib/ares_*.c src/lib/inet_*.c src/lib/windows_port.c \
  $ldflags -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/include/"*.h include
mv "$src/$(zigi lib cares)" lib
#mv "$src/$(zigi bin adig)" bin
#mv "$src/$(zigi bin ahost)" bin
