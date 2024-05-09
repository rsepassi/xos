need libuv
need libuv-zig
need zigcoro
needtool golang
PATH="$BUILD_TOOLS/golang/bin:$PATH"

zig build-exe -target $TARGET -O $OPT_ZIG \
  --name client \
  --dep uv \
  --dep coro \
  -M=$BUILD_PKG/client.zig \
  $(pkg-config --zig coro=zigcoro libuv-zig/uv)

zig build-exe -target $TARGET -O $OPT_ZIG \
  --name server \
  --dep uv \
  --dep coro \
  -M=$BUILD_PKG/server.zig \
  -O Debug \
  $(pkg-config --zig coro=zigcoro libuv-zig/uv)

zig build-exe -target $TARGET -O $OPT_ZIG \
  --name client_udp \
  --dep uv \
  --dep coro \
  -M=$BUILD_PKG/client_udp.zig \
  -O Debug \
  $(pkg-config --zig coro=zigcoro libuv-zig/uv)

GOOS=$(goi os) GOARCH=$(goi arch) \
  go build $BUILD_PKG/echo.go
GOOS=$(goi os) GOARCH=$(goi arch) \
  go build $BUILD_PKG/echo_udp.go

cd $BUILD_OUT
mkdir bin
mv $HOME/$(zigi exe client) bin
mv $HOME/$(zigi exe client_udp) bin
mv $HOME/$(zigi exe server) bin
mv $HOME/$(zigi exe echo) bin
mv $HOME/$(zigi exe echo_udp) bin
