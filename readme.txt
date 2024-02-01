cross-platform builds

uses zig for cross-compilation

wip

---

Usage

ARCH= OPT= build pkgname

ARCH is a zig triple, e.g. aarch64-linux-musl
OPT is a C optimization option, one of:
  0, 1, 2, 3, fast, s, z

---

Example

# ./cmd/build md4c
f48ea956bd4032922d1be258afa96d033f7c90578ab2d0116efd1392d8d037f3
# tree build/out/
build/out/
├── bin
│   └── md2html
├── include
│   ├── md4c-html.h
│   └── md4c.h
└── lib
    └── libmd4c.a

4 directories, 4 files

---

cmd/
  build: build a package

pkg/: build scripts for packages
  <pkgname>/
    build.sh: build script
    manifest.txt (optional): list files in <pkgname>/ that should be considered
      source; defaults to all files
    deps.txt (optional): pkg dependencies

tools/
  fetch: fetch a url and check hash
  fetch_urltxt: fetch a url and check hash, reading url and hash from file
  link_tools: link all tools needed by xos builds
  need: build a dependency and link it into $BUILD_DEPS
  pkgid: compute identifier for pkg build
  untar: clean tar extraction

---

todo

* dynamic dependency tracking
* binary releases of xos for macos, linux, windows
* additional command line tools (e.g. dry run, etc)
* add make
* add cc, ar, ... aliases with path seals
