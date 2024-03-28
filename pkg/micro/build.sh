needtool golang
PATH="$BUILD_TOOLS/golang/bin:$PATH"

url="https://github.com/zyedidia/micro/archive/refs/tags/v2.0.13.tar.gz"
hash="a96fff974ed6bd9a1dd58a33e54ff23b78783bbb3571b86d5c37d787b1e0e4be"
file="micro.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

# For reasons unknown, linking errors for Linux with the builtin linker,
# and errors for Windows+Mac using the zig linker.
if [ "$TARGET_OS" = "linux" ]
then
  gocmd="gobuild"
else
  gocmd="go build"
fi

bin=$(zigi exe micro)
GOOS=$(goi os) GOARCH=$(goi arch) \
  $gocmd ./cmd/micro

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
