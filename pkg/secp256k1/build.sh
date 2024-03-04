url="https://api.github.com/repos/libbtc/libbtc/tarball/0793399"
hash="b88ea087ff7dc25ad29dd14e39e764e393d4c18ad23345d4dffa2aa510cd775c"
file="btc.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cd "$src/src/secp256k1"

rm src/valgrind_ctime_test.c src/tests.c src/bench_*.c src/gen_context.c src/tests_exhaustive.c

touch secp256k1.c
defs="
-DUSE_NUM_GMP
-DUSE_FIELD_10X26
-DUSE_FIELD_INV_BUILTIN
-DUSE_SCALAR_8X32
-DUSE_SCALAR_INV_BUILTIN
-DENABLE_MODULE_RECOVERY
-DECMULT_WINDOW_SIZE=15
-DECMULT_GEN_PREC_BITS=4
"
zig build-lib -target $TARGET -O $OPT_ZIG \
  $defs -I. \
  secp256k1.c src/*.c -lc
ls

cd "$BUILD_OUT"
mkdir lib
mv "$src/src/secp256k1/include" .
mv "$src/src/secp256k1/$(zigi lib secp256k1)" lib
pkg-config --gendefault secp256k1
