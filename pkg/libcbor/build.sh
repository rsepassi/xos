need cjson

url="https://github.com/PJK/libcbor/archive/refs/tags/v0.11.0.tar.gz"
hash="89e0a83d16993ce50651a7501355453f5250e8729dfc8d4a251a78ea23bb26d7"
file="cbor.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

mkdir cbor
cat <<EOF > cbor/configuration.h
#ifndef LIBCBOR_CONFIGURATION_H
#define LIBCBOR_CONFIGURATION_H

#define CBOR_MAJOR_VERSION 0
#define CBOR_MINOR_VERSION 11
#define CBOR_PATCH_VERSION 0

#define CBOR_BUFFER_GROWTH 2
#define CBOR_MAX_STACK_SIZE 2048
#define CBOR_PRETTY_PRINTER 1

#define CBOR_RESTRICT_SPECIFIER restrict
#define CBOR_INLINE_SPECIFIER

#endif //LIBCBOR_CONFIGURATION_H
EOF

cat <<EOF > src/cbor/cbor_export.h
#ifndef CBOR_EXPORT_H
#define CBOR_EXPORT_H

#define CBOR_EXPORT
#define CBOR_NO_EXPORT
#define CBOR_DEPRECATED __attribute__ ((__deprecated__))
#define CBOR_DEPRECATED_EXPORT
#define CBOR_DEPRECATED_NO_EXPORT

#endif
EOF

internal_files="
src/cbor/internal/encoders.c
src/cbor/internal/builder_callbacks.c
src/cbor/internal/loaders.c
src/cbor/internal/memory_utils.c
src/cbor/internal/stack.c
src/cbor/internal/unicode.c
"

zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. -Isrc \
  src/cbor.c src/allocators.c src/cbor/*.c $internal_files \
  -lc

ln -s "$BUILD_DEPS/cjson/include" cjson
zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. -Isrc \
  examples/cbor2cjson.c \
  $(pkg-config --libs cjson/json) \
  $(zigi lib cbor) \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. -Isrc \
  examples/cjson2cbor.c \
  $(pkg-config --libs cjson/json) \
  $(zigi lib cbor) \
  -lc

cd "$BUILD_OUT"
mkdir -p lib include/cbor bin
cp "$src/src/cbor.h" include
cp "$src/src/cbor/"*.h include/cbor
mv "$src/$(zigi lib cbor)" lib
mv "$src/$(zigi exe cjson2cbor)" bin
mv "$src/$(zigi exe cbor2cjson)" bin
pkg-config --gendefault cbor
