#!/usr/bin/env sh

set -ex

# setup dirs
xosroot="$PWD"
buildroot="${BOOTSTRAP_BUILD:-$xosroot/build/bootstrap}"
out="$buildroot/out"
tools="$out/tools"
cache="$buildroot/cache"
tmp="$buildroot/tmp"
rm -rf $tmp $out
mkdir -p "$tools" "$cache" "$tmp"

# system id
case $(uname) in
  Linux)
    ARCH_OS="linux"
    arch_lib="musl"
    if [ "$BOOTSTRAP_CONTAINER_BUILD" = 1 ]
    then
      apk add xz
    fi
    ;;
  Darwin)
    ARCH_OS="macos"
    arch_lib="none"
    ;;
  *)
    echo "unknown os $(uname)"
    exit 1
    ;;
esac

case $(uname -m) in
  arm64|aarch64)
    ARCH_ISA="aarch64"
    ;;
  x86_64)
    ARCH_ISA="x86_64"
    ;;
  *)
    echo "unknown isa $(uname -m)"
    exit 1
    ;;
esac
ARCH="$ARCH_ISA-$ARCH_OS-$arch_lib"

# link tools
scripts="
fetch
cc
c++
ar
rc
windres
fetch_urltxt
need
needtool
untar
xos_internal_build
xos_internal_pkgid
xos_internal_link_tools
zigi
"
for script in $scripts
do
  ln -s "$xosroot/pkg/xos/src/$script" "$tools"
done
ln -s cc "$tools/ld"
ln -s tools/xos_internal_build "$out/build"
cat <<EOF > "$tools/xos_internal_mktemp"
#!/usr/bin/env sh
exec busybox mktemp \$@
EOF
chmod +x "$tools/xos_internal_mktemp"
if [ "$ARCH_OS" = "macos" ]
then
  cat <<EOF > "$tools/nproc"
#!/usr/bin/env sh
exec system sysctl -n hw.logicalcpu
EOF
  chmod +x "$tools/nproc"
fi

# zig
mkdir -p "$out/zig"

PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/zig" \
ARCH_OS=$ARCH_OS \
ARCH_ISA=$ARCH_ISA \
BUILD_DEPS="$tmp" \
XOS_BUILD_CACHE="$cache" \
BUILD_OUT="$out/zig" \
sh -ex "$xosroot/pkg/zig/build.sh"
ln -s ../zig/zig "$tools/zig"

# make
mkdir "$tmp/make"
PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/make" \
ARCH="$ARCH" \
OPT_ZIG="ReleaseSmall" \
ARCH_OS=$ARCH_OS \
ARCH_ISA=$ARCH_ISA \
BUILD_DEPS="$tmp" \
XOS_BUILD_CACHE="$cache" \
BUILD_OUT="$tmp/make" \
sh -ex "$xosroot/pkg/make/build.sh"

# busybox
mkdir "$tmp/busybox"

PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/busybox" \
ARCH="$ARCH" \
OPT="s" \
ARCH_OS=$ARCH_OS \
BUILD_DEPS="$tmp" \
BUILD_TOOLDEPS="$tmp" \
XOS_BUILD_CACHE="$cache" \
BUILD_OUT="$tmp/busybox" \
sh -ex "$xosroot/pkg/busybox/build.sh"
cp "$tmp/busybox/bin/busybox" "$tools"

# link busybox
bbtools="
mkdir
ls
rm
mv
cp
ln
basename
dirname
realpath
tar
gzip
unzip
wget
sha256sum
cat
cut
grep
head
tail
which
env
touch
find
sed
sleep
bzip2
awk
wc
xargs
sort
uniq
diff
chmod
sh
xz
cmp
tr
od
readlink
expr
rmdir
patch
"

for tool in $bbtools
do
  ln -s busybox "$tools/$tool"
done

if [ "$ARCH_OS" != "macos" ]
then
  ln -s busybox "$tools/nproc"
fi

echo "xos bootstrap build" > "$out/readme.txt"
echo "xos bootstrap build" > "$out/.xos"
echo "$ARCH" > "$out/.xos_host"
sha256sum "$out/.xos" | cut -d' ' -f1 > "$out/.xos_id"

echo "bootstrap ok"
