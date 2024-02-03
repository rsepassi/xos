# TODO:
# * add_system_commands has to happen live
set -e

zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/busybox"

tools="$BUILD_OUT/tools"

mkdir -p "$tools"
# cp -rL $zig $BUILD_OUT/zig
ln -s "$zig" "$BUILD_OUT/zig"
ln -s ../zig/zig "$tools"
# cp $bb $tools
ln -s "$bb" "$tools"

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
"

for tool in $bbtools
do
  ln -s busybox "$BUILD_OUT/tools/$tool"
done

  cat <<EOF > "$tools/internal_mktemp"
#!/usr/bin/env sh
set -e
busybox mktemp "\$@"
EOF
  chmod +x "$tools/internal_mktemp"

scripts="
build
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
  cp "$BUILD_PKG/$script" "$tools"
done

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
xosid > "$BUILD_OUT/.xos"
