# >&2 echo "testtest package is for scratch development, edit pkg/testtest/build.sh"
# exit 1

need sqlite3
need sqlite3-zig
need zstd
need zstd-zig

zig build-exe -target $TARGET -O $OPT_ZIG \
  --dep zstd \
  --dep sqlite \
  $(pkg-config --libs sqlite3 zstd) \
  -Mtest=$BUILD_PKG/test.zig \
  $(pkg-config --cflags zstd) \
  -Mzstd=$BUILD_DEPS/zstd-zig/zig/zstd.zig \
  $(pkg-config --cflags sqlite3) \
  -Msqlite=$BUILD_DEPS/sqlite3-zig/zig/sqlite.zig

mkdir $BUILD_OUT/bin
mv test $BUILD_OUT/bin
