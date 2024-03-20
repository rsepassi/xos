url="https://github.com/wren-lang/wren/archive/refs/tags/0.4.0.tar.gz"
hash="23c0ddeb6c67a4ed9285bded49f7c91714922c2e7bb88f42428386bf1cf7b339"
src=$(fetch_untar $url wren.tar.gz $hash)
cd $src

# lib
touch wren.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  wren.c \
  src/vm/*.c \
  src/optional/wren_opt_meta.c \
  -DWREN_OPT_META=1 -DWREN_OPT_RANDOM=0 \
  -I src/include \
  -I src/optional \
  -I src/vm \
  -lc

# install
cd "$BUILD_OUT"
mkdir lib include
cp "$src"/src/include/wren.h include
cp "$src"/$(zigi lib wren) lib
pkg-config --gendefault wren
