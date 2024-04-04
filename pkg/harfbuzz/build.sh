need freetype

src=$(fetch_untar \
	"https://github.com/harfbuzz/harfbuzz/releases/download/8.0.1/harfbuzz-8.0.1.tar.xz" \
	harfbuzz.tar.xz \
	"c1ce780acd385569f25b9a29603d1d5bc71e6940e55bfdd4f7266fad50e42620")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_FREETYPE \
  $(pkg-config --cflags freetype) \
  src/harfbuzz.cc \
  -lc++

mkdir -p include/harfbuzz
cp src/*.h include/harfbuzz

zig build-exe -target $TARGET -O $OPT_ZIG \
  -I include \
  "$BUILD_PKG/demo.c" \
  $(zigi lib harfbuzz) \
	$(pkg-config --cflags --libs freetype) \
  -lc++ -lunwind -lc

cd "$BUILD_OUT"
mkdir bin lib
mv "$src/include" .
mv "$src/$(zigi exe demo)" bin
mv "$src/$(zigi lib harfbuzz)" lib
pkg-config --gendefault harfbuzz --ldflags "-lc++"
