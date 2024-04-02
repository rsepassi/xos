# xos

Hermetic cross-platform builds

* uses zig for cross-compilation
* uses busybox for sh and other utilities
* languages: c, c++, objective-c, zig, rust, go, nim, wren

wip

https://github.com/rsepassi/xos

## Usage

```
# build -h
usage: build pkgname TARGET= OPT=

echoes pkgid on successful build
build artifacts for last successful build available in build/out/

https://github.com/rsepassi/xos

example:
  build sqlite3 TARGET=aarch64-linux-musl OPT=s

Args (or env vars):
  TARGET= zig target triple, defaults to host
  OPT={Debug, Safe, Fast, Small, 0, 1, 2, 3, fast, s, z} optimization level, defaults to Small
  BUILD= directory for build artifacts, defaults to ./build
  PKG= pkg/ directory path, defaults to ./pkg
  DRY={1,0} if 1, log info and exit
  DEV={1,0} if 1, allows for dev options
```

## Example

```
# build sqlite3
a5e44ec93d3d943929bc8611eef9cb7e7a0417a5511bbf775d645b38f178577d

# tree build/out
build/out
├── include
│   ├── sqlite3.h
│   └── sqlite3ext.h
├── lib
│   └── libsqlite3.a
└── pkgconfig
    └── sqlite3.pc

4 directories, 4 files
```

## Code organization

```
pkg/: build scripts for packages
  <pkgname>/
    build.sh: build script
    manifest.txt (optional): list files in <pkgname>/ that should be considered
      source; defaults to all files
```

## build.sh scripts

Env:
* `TARGET`, `TARGET_OS`, `TARGET_ARCH`: target information
* `HOST`, `HOST_OS`, `HOST_ARCH`: host information
* `OPT`, `OPT_ZIG`: cc and zig optimization modes
* `BUILD_PKG`: the pkg source directory for this package
* `BUILD_DEPS`, `BUILD_TOOLS`: dependency directories
* `BUILD_OUT`: where build artifacts should go

Tools:
* `zig`: cross-compiler
* `busybox`: basic utilities, including sh (listed below)
* `zigi`: exe/lib naming information from zig
* `cc`, `c++`, `ar`, `ld`, `rc`, `windres`: wrappers around zig
* `pkg-config`: xos-aware pkg-config
* `fetch`, `fetch_untar`: caching downloader (+unarchiver)
* `need`, `needtool`: dynamic dependency for target/host
* `system`, `system_export`: access system tools
* `wrensh`: wren scripts

```
awk basename bzip2 cat chmod cmp cp cut diff dirname echo env expr find grep
gzip head ln ls mkdir mv nproc od patch readlink realpath rm rmdir sed sh
sha256sum sleep sort tail tar touch tr uniq unzip wc wget which xargs xz
```

## Building xos

Given xos: `build xos`

Bootstrap:
* From an Alpine Linux container: `./pkg/xos/bootstrap/podman_build.sh`
* Direct: `./pkg/xos/bootstrap/build.sh`

## Todo

* Android support (https://github.com/MasterQ32/ZigAndroidTemplate)
* wren build scripts (with samurai lib)
* linux sandboxing (bwrap)
* generic caching utilities
* binary releases
* docs
* rust
	* sccache
	* --locked
	* Windows: https://github.com/ziglang/zig/issues/15107 

## Inspirations

* Zig, by Andrew Kelley
* Cosmopolitan, by Justine Tunney
* redo, by Daniel J. Bernstein
* Nix, by Eelco Dolstra
* Alpine Linux, by Natanael Copa
* Oasis Linux, by Michael Forney
