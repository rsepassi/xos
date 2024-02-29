url="https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.17.tar.gz"
hash="c91d1eeb7175c50d49f6ba2a25e69b46bd05cffb798382c19bfb202e467ec51c"
file="cjson.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

touch json.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  json.c cJSON.c cJSON_Utils.c \
  -lc

cd "$BUILD_OUT"
mkdir -p lib include
mv "$src/$(zigi lib json)" lib
cp "$src/cJSON.h" "$src/cJSON_Utils.h" include
pkg-config --gendefault json
