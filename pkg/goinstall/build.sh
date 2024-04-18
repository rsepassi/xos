needtool golang

PATH="$BUILD_TOOLS/golang/bin:$PATH"

if [ "$TARGET_OS" = "$HOST_OS" ] && [ "$TARGET_ARCH" = "$HOST_ARCH" ]
then
  suffix=""
else
  suffix="$(goi os)_$(goi arch)"
fi

GOOS=$(goi os) GOARCH=$(goi arch) \
go install -ldflags="-linkmode=external -extldflags -s -w" "$@"

ln -s "$HOME/go/bin/$suffix" "$BUILD_OUT/bin"
