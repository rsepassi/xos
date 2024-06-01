need opus

src=$(fetch_untar https://api.github.com/repos/xiph/libopusenc/tarball/e4285b5 \
  opusenc.tar.gz \
  514e6790ec3bdc3ef94ba0cda6bc5c3465d47ce53ff25ce7c549d2c9b8adeb8a)
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
	--name opusenc \
  -Iinclude \
  -DOUTSIDE_SPEEX \
  -DRANDOM_PREFIX=XOS \
  -DPACKAGE_NAME=\"opusenc\" \
  -DPACKAGE_VERSION=\"0.1\" \
  $(pkg-config --cflags opus) \
	src/*.c

cd $BUILD_OUT
mkdir lib
mv $src/$(zigi lib opusenc) lib
mv $src/include .
pkg-config --gendefault opusenc --deps opus
