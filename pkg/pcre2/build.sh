need zlib

url="https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.42/pcre2-10.42.tar.gz"
hash="c33b418e3b936ee3153de2c61cc638e7e4fe3156022a5c77d0711bcbb9d64f1f"
file="pcre2.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

files="
pcre2_auto_possess.c
pcre2_compile.c
pcre2_config.c
pcre2_context.c
pcre2_convert.c
pcre2_dfa_match.c
pcre2_error.c
pcre2_extuni.c
pcre2_find_bracket.c
pcre2_jit_compile.c
pcre2_maketables.c
pcre2_match.c
pcre2_match_data.c
pcre2_newline.c
pcre2_ord2utf.c
pcre2_pattern_info.c
pcre2_script_run.c
pcre2_serialize.c
pcre2_string_utils.c
pcre2_study.c
pcre2_substitute.c
pcre2_substring.c
pcre2_tables.c
pcre2_ucd.c
pcre2_valid_utf.c
pcre2_xclass.c
pcre2posix.c
pcre2_chartables.c
"

cd src
mv pcre2_chartables.c.dist pcre2_chartables.c
cp pcre2.h.generic pcre2.h
cp "$BUILD_PKG/arch/config-$ARCH_OS.h" config.h
touch pcre2.c
zig build-lib -target $ARCH -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DPCRE2_CODE_UNIT_WIDTH=8 -D_THREAD_SAFE -DPCRE2_STATIC -isystem . \
  pcre2.c $files \
  -lc
lib=$PWD/$(zigi lib pcre2)

zig build-exe -target $ARCH -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DPCRE2_CODE_UNIT_WIDTH=8 -D_THREAD_SAFE -DPCRE2_STATIC -isystem . \
  -I "$BUILD_DEPS/zlib/include" \
  pcre2grep.c $lib "$BUILD_DEPS/zlib/lib/$(zigi lib z)" \
  -lc

mkdir "$BUILD_OUT/lib" "$BUILD_OUT/include" "$BUILD_OUT/bin"
mv $lib "$BUILD_OUT/lib"
mv pcre2.h pcre2posix.h "$BUILD_OUT/include"
mv $(zigi exe pcre2grep) "$BUILD_OUT/bin"
