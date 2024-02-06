#!/usr/bin/env sh

set -ex

# setup dirs
xosroot="$PWD"
xospkg="$xosroot/pkg/xos"
buildroot="$(realpath "${BOOTSTRAP_BUILD:-$PWD/build/bootstrap}")"
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
      apk add make xz
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

# link tools
scripts="
fetch
cc
ar
fetch_urltxt
need
pkgid
untar
link_tools
"
for script in $scripts
do
  cp "$xospkg/src/$script" "$tools"
done
cp "$xospkg/src/build" "$out"
ln -s ../build "$tools/build"
sh -e "$tools/link_tools" "$tools"
cat <<EOF > "$tools/internal_mktemp"
#!/usr/bin/env sh
exec busybox mktemp \$@
EOF
chmod +x "$tools/internal_mktemp"

# zig
mkdir -p "$out/zig"

PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/zig" \
ARCH_OS=$ARCH_OS \
ARCH_ISA=$ARCH_ISA \
BUILD_DEPS="$tmp" \
BUILD_CACHE="$cache" \
BUILD_OUT="$out/zig" \
sh -ex "$xosroot/pkg/zig/build.sh"
ln -s ../zig/zig "$tools/zig"

# make
mkdir "$tmp/make"
PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/make" \
ARCH="$ARCH_ISA-$ARCH_OS-$arch_lib" \
OPT_ZIG="ReleaseSmall" \
ARCH_OS=$ARCH_OS \
ARCH_ISA=$ARCH_ISA \
BUILD_DEPS="$tmp" \
BUILD_CACHE="$cache" \
BUILD_OUT="$tmp/make" \
sh -ex "$xosroot/pkg/make/build.sh"
cp "$tmp/make/bin/make" "$tools"

# busybox
mkdir "$tmp/busybox"

PATH="$tools:$PATH" \
BUILD_PKG="$xosroot/pkg/busybox" \
ARCH="$ARCH_ISA-$ARCH_OS-$arch_lib" \
OPT="s" \
ARCH_OS=$ARCH_OS \
BUILD_DEPS="$tmp" \
BUILD_CACHE="$cache" \
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
"

for tool in $bbtools
do
  ln -s busybox "$tools/$tool"
done

# nproc
if [ "$ARCH_OS" = "macos" ]
then
  cat <<EOF > "$tools/nproc"
#!/usr/bin/env sh
exec sysctl -n hw.logicalcpu
EOF
  chmod +x "$tools/nproc"
else
  ln -s busybox "$tools/nproc"
fi


echo "xos bootstrap build" > "$out/readme.txt"
echo "xos bootstrap build" > "$out/.xos"

echo "bootstrap ok"
