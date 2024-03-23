needtool golang
PATH="$BUILD_TOOLS/golang/bin:$PATH"

url="https://github.com/tendermint/tendermint/archive/refs/tags/v0.34.24.tar.gz"
hash="4b288b16e95a7a90fa8ce09eea596a2c3f7d75a2c5f78f235a9680028a63033b"
file="tendermint.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

bin=$(zigi exe tendermint)
GOOS=$(goi os) GOARCH=$(goi arch) \
  gobuild -o $bin ./cmd/tendermint/

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
