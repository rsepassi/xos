url="https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip"
hash="bde30d13ebdf84926ddd5e8b6df145be03a577a48fd075a087a5dd815bcdf740"
file="sqlite.zip"
fetch "$url" "$file" "$hash"
src=$(mktemp -d)
cd $src
unzip $(realpath $BUILD_DEPS/sqlite.zip)
src="$src/sqlite-amalgamation-3450000"
cd $src

# SQLITE_TRANSIENT is set to -1, but that's an invalid pointer, so it breaks
# Zig's C header import. Since the SQLite code only ever tests equality of
# this constant, and since we're always statically linking, replace it here
# with a value that doesn't break Zig.
sed -i '/^#define SQLITE_TRANSIENT/s/.*/#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)8)/' sqlite3.h
sed -i '/^#define SQLITE_TRANSIENT/s/.*/#define SQLITE_TRANSIENT   ((sqlite3_destructor_type)8)/' sqlite3.c

# lib
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DSQLITE_ENABLE_FTS5 \
  sqlite3.c -lc

# install
cd "$BUILD_OUT"
mkdir lib include
cp $src/*.h include
mv $src/$(zigi lib sqlite3) lib
pkg-config --gendefault sqlite3
