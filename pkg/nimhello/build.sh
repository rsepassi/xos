needtool nim
export PATH="$BUILD_TOOLS/nim/bin:$PATH"

# Binary
cat <<EOF > hello.nim
echo "Hello World"
EOF
nimc hello.nim

# Static library
cat <<EOF > hellofn.nim
proc hello(name:cstring) {.exportc.} =
  echo name
EOF
nimc --app:staticlib hellofn.nim

cat <<EOF > hello2.c
extern void hello(char*);

int main(int argc, char** argv) {
  hello(argv[1]);
}
EOF
cc -target $TARGET -O$OPT \
  -o $(zigi exe hello2) \
  hello2.c \
  $(zigi lib hellofn) \
  -lc

cd "$BUILD_OUT"
mkdir bin lib
cp "$HOME"/$(zigi exe hello) bin
cp "$HOME"/$(zigi exe hello2) bin
cp "$HOME"/$(zigi lib hellofn) lib
