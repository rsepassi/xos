url="https://github.com/microsoft/mimalloc/archive/refs/tags/v2.1.2.tar.gz"
hash="2b1bff6f717f9725c70bf8d79e4786da13de8a270059e4ba0bdd262ae7be46eb"
file="mimalloc.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

srcs="
src/alloc-aligned.c
src/alloc-posix.c
src/alloc.c
src/arena.c
src/bitmap.c
src/bitmap.h
src/heap.c
src/init.c
src/options.c
src/os.c
src/page.c
src/random.c
src/segment-map.c
src/segment.c
src/static.c
src/stats.c
"

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  sdk="$BUILD_DEPS/macossdk/sdk"
  cflags="-I$sdk/usr/include -DMI_OSX_ZONE=1 -DMI_OSX_INTERPOSE=1"
  srcs="$srcs $(ls src/prim/osx/*.c)"
elif [ "$TARGET_OS" = "linux" ]
then
  srcs="$srcs $(ls src/prim/unix/*.c)"
elif [ "$TARGET_OS" = "windows" ]
then
  srcs="$srcs $(ls src/prim/windows/*.c)"
fi

touch mimalloc.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DNDEBUG -DMI_STATIC_LIB \
  -Iinclude \
  $cflags \
  -cflags \
  -include "include/mimalloc.h" -include "include/mimalloc/types.h" \
  -fno-builtin-malloc \
  -- \
  mimalloc.c $srcs src/prim/prim.c \
  -lc

cd "$BUILD_OUT"
mkdir lib
mv "$src/$(zigi lib mimalloc)" lib
cp -r "$src/include" .
pkg-config --gendefault mimalloc
