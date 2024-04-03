src=$(fetch_untar \
  "https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz" \
  brotli.tar.gz \
  "e720a6ca29428b803f4ad165371771f5398faba397edf6778837a18599ea13ff")
cd $src

srcs="
$(find c/common -type f -name '*.c')
$(find c/enc -type f -name '*.c')
$(find c/dec -type f -name '*.c')
"

touch brotli.c
zig build-lib -target $TARGET -O $OPT_ZIG \
	-I c/include \
	brotli.c \
	$srcs \
	-lc++

cd "$BUILD_OUT"
mkdir include lib
mv "$src/c/include/brotli" include
mv "$src/$(zigi lib brotli)" lib
pkg-config --gendefault brotli
