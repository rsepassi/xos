xos

hermetic cross-platform builds

uses zig for cross-compilation
uses busybox for sh and other utilities

wip

---

usage: build pkgname TARGET= OPT=

echoes pkgid on successful build
build artifacts for last successful build available in build/out/

https://github.com/rsepassi/xos

example:
  build sqlite TARGET=aarch64-linux-musl OPT=s

Args (or env vars):
  TARGET= zig target triple, defaults to host
  OPT={Debug, Safe, Fast, Small, 0, 1, 2, 3, fast, s, z} optimization level, defaults to Small
  BUILD= directory for build artifacts, defaults to ./build
  PKG= pkg/ directory path, defaults to ./pkg
  DRY={1,0} if 1, log info and exit
  DEBUG={1,0} if 1, build script -x enabled

---

Example

# build md4c
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

pkg/: build scripts for packages
  <pkgname>/
    build.sh: build script
    manifest.txt (optional): list files in <pkgname>/ that should be considered
      source; defaults to all files

---

build.sh scripts

Env:
  DEBUG={0,1} whether debug mode is on
  TARGET, TARGET_OS, TARGET_ARCH: target information
  HOST, HOST_OS, HOST_ARCH: host information
  OPT, OPT_ZIG: cc and zig optimization modes
  BUILD_PKG: the pkg source directory for this package
  BUILD_DEPS, BUILD_TOOLDEPS: dependency directories
  BUILD_OUT: where build artifacts should go

Tools:
  zig: cross-compiler
  busybox: basic utilities, including sh
  zigi: exe/lib naming information from zig
  cc, c++, ar, rc, windres: wrappers around zig
  pkg-config: xos-aware pkg-config
  fetch, fetch_untar: caching downloader (+unarchiver)
  need: dynamic dependency for target
  needtool: dynamic dependency for host
  system, system_export: access system tools

---

to build xos given xos:

  build xos

---

bootstrapping xos

a podman alpine-based bootstrap script is provided

  ./pkg/xos/bootstrap/podman_build.sh

or run directly

  ./pkg/xos/bootstrap/build.sh

---

todo

* generic caching utilities
* linux sandboxing (bwrap)
* binary releases of xos for macos, linux, windows
* documentation update
* intro article

---

inspirations

Zig, by Andrew Kelley
Cosmopolitan, by Justine Tunney
redo, by Daniel J. Bernstein
Nix, by Eelco Dolstra
Alpine Linux, by Natanael Copa
Oasis Linux, by Michael Forney
