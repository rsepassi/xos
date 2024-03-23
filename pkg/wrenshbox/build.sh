need wrensh
needtool cstrbake

# wrenshbox
cat "$BUILD_PKG/wrenshbox.wren" | \
  "$BUILD_TOOLS/cstrbake/bin/cstrbake" wrensh_src_user \
  > wrenshbox.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  wrenshbox.c \
  $(pkg-config --cflags --libs wrensh) \
  -lc

cd "$BUILD_OUT"
mkdir bin
mv "$HOME/$(zigi exe wrenshbox)" bin
