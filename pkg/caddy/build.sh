needtool golang
PATH="$BUILD_TOOLS/golang/bin:$PATH"

url="https://github.com/caddyserver/caddy/archive/refs/tags/v2.7.6.tar.gz"
hash="e1c524fc4f4bd2b0d39df51679d9d065bb811e381b7e4e51466ba39a0083e3ed"
file="caddy.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src/cmd/caddy

bin=$(zigi exe caddy)
GOOS=$(goi os) GOARCH=$(goi arch) \
  gobuild -o $bin

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
