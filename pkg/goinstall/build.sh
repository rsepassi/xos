needtool golang

PATH="$BUILD_TOOLS/golang/bin:$PATH"

GOOS=$(goi os) GOARCH=$(goi arch) \
go install -ldflags="-linkmode=external -extldflags -s -w" "$@"

ln -s "$HOME/go/bin/$(goi os)_$(goi arch)" "$BUILD_OUT/bin"
