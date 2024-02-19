set -e

mode=$2

out="$(mktemp -d)"

# setup output dirs
tools="$out/tools"
mkdir -p $tools

# zig+make+busybox
zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/busybox"
make="$BUILD_DEPS/make/bin/make"
if [ "$ARCH_OS" = "windows" ]
then
  exe=".exe"
fi
if [ "$mode" = "release" ]
then
  cp -rL "$zig" "$out/zig"
  cp "$bb" "$tools"
  cp "$make" "$tools"
else
  ln -s "$zig" "$out/zig"
  ln -s "$bb" "$tools"
  ln -s "$make" "$tools"
fi
ln -s ../zig/"zig$exe" "$tools"/zig

# internal tools
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
  cp "$BUILD_PKG/src/$script" "$tools"
done
ln -s cc "$tools/ld"
ln -s tools/build "$out"
cat <<EOF > "$tools/xos_internal_mktemp"
#!/usr/bin/env sh
set -e
busybox mktemp \$@
EOF
chmod +x "$tools/xos_internal_mktemp"

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

# nproc
if [ "$ARCH_OS" = "macos" ]
then
  cat <<EOF > "$tools/nproc"
#!/usr/bin/env sh
exec system sysctl -n hw.logicalcpu
EOF
  chmod +x "$tools/nproc"
else
  ln -s busybox "$tools/nproc"
fi

# readme
cp "$BUILD_PKG/src/dist_readme.txt" "$out"

# xos id
zigid="0.12.0-dev.2341+92211135f"
bbid="1.36.1 FRP-5236-g7dff7f376"
xosid() {
  echo "ZIG=$zigid"
  echo "BUSYBOX=$bbid"
  files=$(find . -type f | sort)
  sha256sum $files
}
cd "$BUILD_PKG"
xosid > "$out/.xos"
sha256sum "$out/.xos" | cut -d' ' -f1 > "$out/.xos_id"
echo "$ARCH" > "$out/.xos_host"

if [ "$mode" = "release" ]
then
  mv "$out" "$BUILD_OUT/xos"
  cd "$BUILD_OUT"
  tar czf xos.tar.gz xos
  rm -rf ./xos
else
  cd "$out"
  mv ./* ./.xos "$BUILD_OUT"
fi
