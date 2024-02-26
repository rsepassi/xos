[ "$TARGET_OS" = "linux" ] || { >&2 echo "bubblewrap only available on Linux"; exit 1; }

need libcap

url="https://github.com/containers/bubblewrap/releases/download/v0.8.0/bubblewrap-0.8.0.tar.xz"
hash="957ad1149db9033db88e988b12bcebe349a445e1efc8a9b59ad2939a113d333a"
file="bwrap.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

cp "$BUILD_PKG/config.h" .
touch bwrap.c
zig build-exe -target $TARGET -O $OPT_ZIG bwrap.c *.c \
  -I"$BUILD_DEPS/libcap/include" \
  "$BUILD_DEPS/libcap/lib/libcap.a" \
  -lc

mkdir "$BUILD_OUT/bin"
mv bwrap "$BUILD_OUT/bin"
