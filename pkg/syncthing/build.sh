needtool golang
PATH="$BUILD_TOOLDEPS/golang/bin:$PATH"

version="v1.27.3"

url="https://github.com/syncthing/syncthing/archive/refs/tags/$version.tar.gz"
hash="fa2edae90c7999a6f667bba26a6c63c7165647f77c02c83860237c6d08ee4bbd"
file="syncthing.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

bin=$(zigi exe syncthing)

export EXTRA_LDFLAGS="-linkmode=external"
gorun build.go \
  -goarch $(goi arch) -goos $(goi os) \
  -version "$version" -no-upgrade \
  build 

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
