set -e

ARCH=${ARCH:-native}
OPT=${OPT:-s}

fetch_urltxt $BUILD_PKG/url.txt $BUILD_DL/sqlite.zip
src=$(mktemp -d)
cd $src
unzip $BUILD_DL/sqlite.zip

# lib
cd $src/sqlite-amalgamation-3450000
zig cc --target=$ARCH -o sqlite3.o -c sqlite3.c
zig ar rcs libsqlite3.a sqlite3.o

# install
out=$BUILD_OUT
mkdir -p $out/lib $out/include
cp *.h $out/include
cp libsqlite3.a $out/lib
