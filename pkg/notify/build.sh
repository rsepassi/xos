needtool golang
PATH="$BUILD_TOOLS/golang/bin:$PATH"

url="https://api.github.com/repos/rjeczalik/cmd/tarball/440232c"
hash="0d8cf1ce7be1c431861a73627a1c12bd4d5127ea6ddf7a5a1a23050120bc5c65"
file="cmd.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

go get github.com/rjeczalik/notify@8c8ecfe

cd notify
bin=$(zigi exe notify)
GOOS=$(goi os) GOARCH=$(goi arch) \
  gobuild -o $bin

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
