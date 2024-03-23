# Simple Static Server
# 
#   # cd somedir
#   # ssserve
#   2024/02/18 18:58:02 Serving . on HTTP port: 8100
needtool golang

PATH="$BUILD_TOOLS/golang/bin:$PATH"

bin=$(zigi exe ssserve)

GOOS=$(goi os) GOARCH=$(goi arch) \
  gobuild -o $bin "$BUILD_PKG/serve.go"

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
