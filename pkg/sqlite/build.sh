fetch_urltxt $BUILD_PKG/url.txt sqlite.zip
src=$(mktemp -d)
cd $src
unzip $(realpath $BUILD_DEPS/sqlite.zip)

# lib
cd $src/sqlite-amalgamation-3450000
cc --target=$ARCH -o sqlite3.o -c sqlite3.c -lc
ar rcs libsqlite3.a sqlite3.o

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp *.h $out/include
cp libsqlite3.a $out/lib
