fetch \
  "https://sqlite.org/src/raw/19e368bc8e97ff4071115119a7911ca3b0c56eba7926d8ada8b4a86fcc69a176?at=lemon.c" \
  lemon.c \
  "8a7d39a6d55cef0e6769f869a09d1269e3aca366c44291e1445bb877e5e94b00"
fetch \
  "https://sqlite.org/src/raw/57478ea48420da05faa873c6d1616321caa5464644588c97fbe8e0ea04450748?at=lempar.c" \
  lempar.c \
  "4cfe425b513895dd11af7d3cde963920a2daeb7f5731f706e6973bfc79f207ed"

mkdir "$BUILD_OUT/bin"
cc -static -s --target=$TARGET -O$OPT \
  "$BUILD_DEPS/lemon.c" -o "$BUILD_OUT/bin/lemon" -lc
mkdir -p "$BUILD_OUT/share"
cp "$BUILD_DEPS/lempar.c" "$BUILD_OUT/share"
