xos

hermetic cross-platform builds

uses zig for cross-compilation
uses busybox for sh and other utilities

wip

---

usage: ARCH= OPT= build pkgname

echoes pkgid on successful build
build artifacts for last successful build available in build/out/

https://github.com/rsepassi/xos

example:
  ARCH=aarch64-linux-musl OPT=s build sqlite

Env vars:
  ARCH= zig target triple, defaults to host
  OPT={Debug, Safe, Fast, Small, 0, 1, 2, 3, fast, s, z} optimization level
  BUILD= directory for build artifacts, defaults to ./build
  PKG= pkg/ directory path, defaults to ./pkg
  DRY={1,0} if 1, log info and exit

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

pkg/: build scripts for packages
  <pkgname>/
    build.sh: build script
    manifest.txt (optional): list files in <pkgname>/ that should be considered
      source; defaults to all files
    deps.txt (optional): pkg dependencies, built for target
    tooldeps.txt (optional): pkg tool dependencies, built for host

pkg/xos/
  build: build a package
  cc: zig cc wrapper
  ar: zig ar wrapper
  fetch: fetch a url and check hash
  fetch_urltxt: fetch a url and check hash, reading url and hash from file
  link_tools: link all tools needed by xos builds
  need: build a dependency and link it into $BUILD_DEPS
  pkgid: compute identifier for pkg build
  untar: clean tar extraction

---

to build xos given xos:
build xos

---

bootstrapping xos

a podman alpine-based bootstrap script is provided

./pkg/xos/bootstrap/podman_build.sh

or run directly (on mac or linux)

./pkg/xos/bootstrap/build.sh

---

todo

* documentation
* dynamic dependency tracking (need)
* binary releases of xos for macos, linux, windows
* prefix scripts with xos_
* traps
* parallelism
* detect dependency cycles

---

inspirations

Zig, by Andrew Kelley
Cosmopolitan, by Justine Tunney
redo, by Daniel J. Bernstein
Nix, by Eelco Dolstra
Alpine Linux, by Natanael Copa
