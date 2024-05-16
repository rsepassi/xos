src=$(fetch_untar "https://api.github.com/repos/antirez/smaz2/tarball/71ecc43" \
  smaz.tar.gz \
  "e35771e5176cf5291e9763b824e06d4153f22b524fd564f0d94511c6d6a7f24d")
cd $src

zig build-lib -target $TARGET -O $OPT_ZIG \
  smaz2.c

cd $BUILD_OUT
mkdir include lib
cp $src/smaz2.h include
mv $src/$(zigi lib smaz2) lib
pkg-config --gendefault smaz2
