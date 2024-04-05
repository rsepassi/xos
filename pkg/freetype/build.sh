need brotli
need zlib

src=$(fetch_untar \
  "https://gitlab.freedesktop.org/freetype/freetype/-/archive/VER-2-13-2/freetype-VER-2-13-2.tar.gz" \
  freetype.tar.gz \
  "427201f5d5151670d05c1f5b45bef5dda1f2e7dd971ef54f0feaaa7ffd2ab90c")
cd $src

srcs="
src/autofit/autofit.c
src/base/ftbase.c
src/base/ftsystem.c
src/base/ftdebug.c
src/base/ftbbox.c
src/base/ftbdf.c
src/base/ftbitmap.c
src/base/ftcid.c
src/base/ftfstype.c
src/base/ftgasp.c
src/base/ftglyph.c
src/base/ftgxval.c
src/base/ftinit.c
src/base/ftmm.c
src/base/ftotval.c
src/base/ftpatent.c
src/base/ftpfr.c
src/base/ftstroke.c
src/base/ftsynth.c
src/base/fttype1.c
src/base/ftwinfnt.c
src/bdf/bdf.c
src/bzip2/ftbzip2.c
src/cache/ftcache.c
src/cff/cff.c
src/cid/type1cid.c
src/gzip/ftgzip.c
src/lzw/ftlzw.c
src/pcf/pcf.c
src/pfr/pfr.c
src/psaux/psaux.c
src/pshinter/pshinter.c
src/psnames/psnames.c
src/raster/raster.c
src/sdf/sdf.c
src/sfnt/sfnt.c
src/smooth/smooth.c
src/svg/svg.c
src/truetype/truetype.c
src/type1/type1.c
src/type42/type42.c
src/winfonts/winfnt.c
"

touch freetype.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -Iinclude \
  -DFT2_BUILD_LIBRARY \
  -DFT_CONFIG_OPTION_USE_BROTLI \
  -DFT_CONFIG_OPTION_USE_ZLIB \
  -DHAVE_UNISTD_H \
  $(pkg-config --cflags brotli zlib/z) \
	freetype.c \
	$srcs \
	-lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG"/demo.c \
  -I include \
  $(zigi lib freetype) \
  $(pkg-config --libs brotli zlib/z) \
  -lc

cd "$BUILD_OUT"
mkdir bin lib
mv "$src/include" .
mv "$src/$(zigi lib freetype)" lib
mv "$src/$(zigi exe demo)" bin
pkg-config --gendefault freetype --deps brotli,zlib/z
