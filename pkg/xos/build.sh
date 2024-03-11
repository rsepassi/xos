need zig
need busybox

mode=$1

out="$(mktemp -d)"

# setup output dirs
tools="$out/tools"
mkdir -p $tools

# zig+busybox
zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/$(zigi exe busybox)"
if [ "$TARGET_OS" = "windows" ]
then
  exe=".exe"
fi
if [ "$mode" = "release" ]
then
  cp -rL "$zig" "$out/zig"
  cp "$bb" "$tools"
else
  ln -s "$zig" "$out/zig"
  ln -s "$bb" "$tools"
fi
ln -s ../zig/"zig$exe" "$tools"/zig

# internal tools
scripts="
fetch
fetch_untar
fetch_urltxt
cc
c++
ar
rc
windres
need
needtool
untar
xos_internal_build
xos_internal_pkgid
xos_internal_link_tools
zigi
pkg-config
"
for script in $scripts
do
  cp "$BUILD_PKG/src/$script" "$tools"
done
ln -s cc "$tools/ld"
ln -s tools/xos_internal_build "$out/build"
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
  ln -s $(zigi exe busybox) "$tools/$tool"
done

# nproc
if [ "$TARGET_OS" = "macos" ]
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
cp "$BUILD_PKG/src/dist_readme.txt" "$out/readme.txt"

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
echo "$TARGET" > "$out/.xos_host"

if [ "$mode" = "release" ]
then
  mv "$out" "$BUILD_OUT/xos"
  cd "$BUILD_OUT"
  if [ "$TARGET_OS" = "windows" ]
  then
    needtool libarchive
    "$BUILD_TOOLDEPS/libarchive/bin/bsdtar" -c --format zip -f xos.zip xos
  else
    tar czf xos.tar.gz xos
  fi
else
  cd "$out"
  mv ./* ./.xos "$BUILD_OUT"
fi
