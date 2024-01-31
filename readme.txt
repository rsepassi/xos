cross-platform builds

uses zig for cross-compilation

wip

cmd/
  build: build a package

pkg/: build scripts for packages
  <pkgname>/
    build.sh: build script
    url.txt (optional): src download
    deps.txt (optional): pkg dependencies

tools/
  bunzip: clean zip extraction
  fetch: fetch a url and check hash
  fetch_urltxt: fetch a url and check hash, reading url and hash from file
  link_tools: symlink passed tools into a directory
  need: build a dependency and link it into $BUILD_DEPS
  pkgid: compute identifier for pkg build
  untar: clean tar extraction
