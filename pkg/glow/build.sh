needtool golang
PATH="$BUILD_TOOLDEPS/golang/bin:$PATH"

url="https://github.com/charmbracelet/glow/archive/refs/tags/v1.5.1.tar.gz"
hash="b4ecf269b7c6447e19591b1d23f398ef2b38a6a75be68458390b42d3efc44b92"
file="glow.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

bin=$(zigi exe glow)
GOOS=$(goi os) GOARCH=$(goi arch) \
  gobuild -o $bin

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
