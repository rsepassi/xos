url="https://github.com/yaml/libyaml/releases/download/0.2.5/yaml-0.2.5.tar.gz"
hash="c642ae9b75fee120b2d96c712538bd2cf283228d2337df2cf2988e3c02678ef4"
file="yaml.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch yaml.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DYAML_VERSION_STRING="\"0.2.5\"" \
  -DYAML_VERSION_MAJOR=0 \
  -DYAML_VERSION_MINOR=2 \
  -DYAML_VERSION_PATCH=5 \
  -Iinclude \
  yaml.c src/*.c \
  -lc

cd "$BUILD_OUT"
mkdir lib include
cp "$src/include/yaml.h" include
mv "$src/$(zigi lib yaml)" lib
pkg-config --gendefault yaml
