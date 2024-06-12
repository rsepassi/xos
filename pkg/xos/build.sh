need zig
need busybox -- cache
need wrensh
need wrenshbox
needtool cstrbake

mode=$1

# Build XOS executables

# xos
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/src/xos.zig" \
  -lc

# build
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/src/xos_build.zig" \
  -lc

# sha256sum
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/src/sha256sum.zig" \
  -lc

# setup output dirs
out="$(mktemp -d)"
tools="$out/tools"
scripts="$out/tools/scripts"
mkdir -p $tools $scripts

# XOS executables
mv $(zigi exe xos) "$out"
mv $(zigi exe xos_build) "$tools/$(zigi exe build)"
mv $(zigi exe sha256sum) "$tools"

# zig+busybox+wrensh+wrenshbox
zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/$(zigi exe busybox)"
wrensh="$BUILD_DEPS/wrensh/bin/$(zigi exe wrensh)"
wrenshbox="$BUILD_DEPS/wrenshbox/bin/$(zigi exe wrenshbox)"
ln -s "$zig" "$out/zig"
ln -s "$bb" "$tools"
ln -s "$wrensh" "$tools"
ln -s "$wrenshbox" "$tools"

# install internal tools
script_tools="
ar
c++
cc
ld
rc
pkg-config
pkg-config-zig.wren
zigi
windres
fetch
fetch_untar
untar
system
system_export
xos_build.wren
xos_main.wren
xos_internal_mktemp
xos_internal_pkgid
"
for script in $script_tools
do
  s="$BUILD_PKG/src/$script"
  d="$scripts/$(zigi exe $script)"
  ln -s "$s" "$d"
done

ln -s "$BUILD_PKG/src/need.wren" "$scripts/$(zigi exe need)"
ln -s "$BUILD_PKG/src/needtool.wren" "$scripts/$(zigi exe needtool)"

# Links
bblinks="
mkdir
ls
rm
mv
cp
ln
realpath
tar
gzip
unzip
wget
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

wrenshboxlinks="
echo
dirname
basename
vlog
"

mkdir "$tools/links"
for link in $bblinks
do
  ln -s ../busybox "$tools/links/$link"
done
for link in $wrenshboxlinks
do
  ln -s ../wrenshbox "$tools/links/$link"
done
ln -s ../../zig/zig "$tools/links/zig"
if [ "$TARGET_OS" = "macos" ]
then
  cp $BUILD_PKG/src/nproc $tools
else
  ln -s ../busybox "$tools/links/nproc"
fi

# readme
cp "$BUILD_PKG/src/dist_readme.txt" "$out/readme.txt"

# xos id
zigid="0.12.0"
bbid="1.36.1 FRP-5236-g7dff7f376"
xosid() {
  src="$PWD"
  cd "$BUILD_PKG"
  echo "ZIG=$zigid"
  echo "BUSYBOX=$bbid"
  files=$(find . -type f | sort)
  sha256sum $files
  cd "$src"
}
xosid | sha256sum | cut -d' ' -f1 > "$out/.xos_id"
echo "$TARGET" > "$out/.xos_host"

if [ "$mode" = "release" ]
then
  cp -rL "$out" "$BUILD_OUT/xos"
  cd "$BUILD_OUT"
  if [ "$TARGET_OS" = "windows" ]
  then
    needtool libarchive
    "$BUILD_TOOLS/libarchive/bin/bsdtar" -c --format zip -f xos.zip xos
  else
    tar czf xos.tar.gz xos
  fi
else
  mv "$out"/$(zigi exe xos) "$BUILD_OUT"
  mv "$out"/tools "$BUILD_OUT"
  mv "$out"/zig "$BUILD_OUT"
  mv "$out"/.xos_id "$BUILD_OUT"
  mv "$out"/.xos_host "$BUILD_OUT"
fi
