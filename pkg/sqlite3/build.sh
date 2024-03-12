url="https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip"
hash="bde30d13ebdf84926ddd5e8b6df145be03a577a48fd075a087a5dd815bcdf740"
file="sqlite.zip"
fetch "$url" "$file" "$hash"
src=$(mktemp -d)
cd $src
unzip $(realpath $BUILD_DEPS/sqlite.zip)
src="$src/sqlite-amalgamation-3450000"
cd $src

# lib
cc --target=$TARGET -o sqlite3.o -c sqlite3.c -lc
ar rcs $(zigi lib sqlite3) sqlite3.o

# install
cd "$BUILD_OUT"
mkdir lib include
cp $src/*.h include
cp $src/$(zigi lib sqlite3) lib
pkg-config --gendefault sqlite3
