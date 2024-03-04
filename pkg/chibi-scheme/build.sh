url="https://api.github.com/repos/ashinn/chibi-scheme/tarball/19c7d4f"
hash="34d70b1a62bc61e49234a47137051f4caac637550421fd2b90938c69675f14a9"
file="chibi.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cat << EOF > include/chibi/install.h
#define sexp_so_extension ""
#define sexp_default_module_path ""
#define sexp_platform "$TARGET_OS"
#define sexp_version "0.10.0"
#define sexp_release_name "neon"
EOF

defs="
-DSEXP_STATIC_LIBRARY
-DSEXP_USE_DL=0
-DSEXP_USE_INTTYPES
-DSEXP_USE_BIDIRECTIONAL_PORTS
-DSEXP_USE_MODULES
-DSEXP_USE_UTF8_STRINGS
-DSEXP_USE_STRING_INDEX_TABLE
-DSEXP_USE_NTPGETTIME
-DSEXP_USE_AUTO_FORCE
-DSEXP_USE_UNIFORM_VECTOR_LITERALS
-DSEXP_USE_COMPLEX
-DSEXP_USE_RATIOS
"

srcs="
gc.c
sexp.c
bignum.c
gc_heap.c
opcodes.c
vm.c
eval.c
simplify.c
"

touch chibi-scheme.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  $defs -Iinclude chibi-scheme.c $srcs -lc

cd "$BUILD_OUT"
mkdir include lib
mv "$src/include/chibi" include
mv "$src/$(zigi lib chibi-scheme)" lib
