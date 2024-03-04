need secp256k1
need trezor-crypto

url="https://api.github.com/repos/libbtc/libbtc/tarball/0793399"
hash="b88ea087ff7dc25ad29dd14e39e764e393d4c18ad23345d4dffa2aa510cd775c"
file="btc.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

rm src/net.c
rm src/netspv.c
rm src/headersdb_file.c
rm src/protocol.c

touch btc.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DRANDOM_DEVICE="\"/dev/urandom\"" \
  -Iinclude -Isrc \
  btc.c src/*.c \
  -lc

touch src/libbtc-config.h
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DPACKAGE_NAME="\"libbtc\"" \
  -DPACKAGE_VERSION="\"0793399\"" \
  -Iinclude -Isrc \
  src/tools/bitcointool.c \
  $(zigi lib btc) \
  $(pkg-config --libs secp256k1) \
  $(pkg-config --libs trezor-crypto) \
  -lc

cd "$BUILD_OUT"
mkdir lib bin include
mv "$src/$(zigi lib btc)" lib
mv "$src/include/btc" include
mv "$src/$(zigi exe bitcointool)" bin
