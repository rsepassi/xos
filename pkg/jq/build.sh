url="https://github.com/jqlang/jq/releases/download/jq-1.7.1/jq-1.7.1.tar.gz"
hash="478c9ca129fd2e3443fe27314b455e211e0d8c60bc8ff7df703873deeee580c2"
file="jq.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

if [ "$TARGET_OS" = "windows" ]
then
  need winpthread
  cflags="-I$BUILD_DEPS/winpthread/include"
  ldflags="$BUILD_DEPS/winpthread/lib/winpthreads.lib -lshlwapi"
  sed -i 's/umain/main/' src/main.c
fi

cp "$BUILD_PKG/builtin.inc" src
echo '#define JQ_CONFIG ""' > src/config_opts.inc
echo '#define JQ_VERSION "jq-1.7.1"' > src/version.h

libfiles="
src/builtin.c
src/bytecode.c
src/compile.c
src/execute.c
src/jq_test.c
src/jv.c
src/jv_alloc.c
src/jv_aux.c
src/jv_dtoa.c
src/jv_file.c
src/jv_parse.c
src/jv_print.c
src/jv_unicode.c
src/lexer.c
src/linker.c
src/locfile.c
src/parser.c
src/util.c
src/decNumber/decContext.c
src/decNumber/decNumber.c
src/jv_dtoa_tsd.c
"

touch jq.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  jq.c \
  $libfiles \
  -DIEEE_8087 \
  -I. $cflags \
  -lc

zig build-exe -target $TARGET -O $OPT_ZIG \
  jq.c \
  src/main.c \
  $(zigi lib jq) \
  -DIEEE_8087 \
  -I. -Ilib \
  $ldflags -lc

mkdir "$BUILD_OUT/bin" "$BUILD_OUT/lib" "$BUILD_OUT/include"
mv $(zigi exe jq) "$BUILD_OUT/bin"
mv $(zigi lib jq) "$BUILD_OUT/lib"
cp src/jv.h src/jq.h "$BUILD_OUT/include"
