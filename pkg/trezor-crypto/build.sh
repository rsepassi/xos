url="https://api.github.com/repos/libbtc/libbtc/tarball/0793399"
hash="b88ea087ff7dc25ad29dd14e39e764e393d4c18ad23345d4dffa2aa510cd775c"
file="btc.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cd "$src/src/trezor-crypto"

rm aes/aestst.c

touch trezor-crypto.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. \
  trezor-crypto.c \
  *.c \
  aes/*.c \
  ed25519-donna/*.c \
  chacha20poly1305/*.c \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I. \
  tools/xpubaddrgen.c \
  $(zigi lib trezor-crypto) \
  -lc

cd "$BUILD_OUT"
mkdir lib bin include
mkdir include/aes include/chacha20poly1305 include/ed25519-donna
mv "$src/src/trezor-crypto/$(zigi lib trezor-crypto)" lib
mv "$src/src/trezor-crypto/$(zigi exe xpubaddrgen)" bin
mv "$src/src/trezor-crypto/"*.h include
mv "$src/src/trezor-crypto/aes/"*.h include/aes
mv "$src/src/trezor-crypto/chacha20poly1305/"*.h include/chacha20poly1305
mv "$src/src/trezor-crypto/ed25519-donna/"*.h include/ed25519-donna
pkg-config --gendefault trezor-crypto
