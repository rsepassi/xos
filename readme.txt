cross-platform builds

uses zig for cross-compilation

very much not complete

cmd/
  build: build a package for a single platform
  build_all: build a package for all platforms

pkg/
  build scripts for packages

tools/
  fetch: fetch a url and check hash
  fetch_urltxt: fetch a url and check hash, reading url and hash from file
  seal: symlink passed tools into a directory
  untar: clean tar extraction
