need zig
need busybox
need wrensh
needtool cstrbake

mode=$1

# Build XOS executables

# build
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/src/xos_build.zig" \
  -lc

# sha256sum
zig build-exe -target $TARGET -O $OPT_ZIG \
  "$BUILD_PKG/src/sha256sum.zig" \
  -lc

# xos_internal_build2 executable
cat "$BUILD_PKG/src/xos_internal_build2" | \
  "$BUILD_TOOLDEPS/cstrbake/bin/cstrbake" wrensh_src_user \
  > xos_internal_build2.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  xos_internal_build2.c \
  $(pkg-config --cflags --libs wrensh) \
  -lc

# wrenshbox
cat "$BUILD_PKG/src/wrenshbox" | \
  "$BUILD_TOOLDEPS/cstrbake/bin/cstrbake" wrensh_src_user \
  > wrenshbox.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  wrenshbox.c \
  $(pkg-config --cflags --libs wrensh) \
  -lc

# setup output dirs
out="$(mktemp -d)"
tools="$out/tools"
mkdir -p $tools

# XOS executables
mv $(zigi exe xos_build) "$out/build"
mv $(zigi exe xos_internal_build2) "$tools/xos_internal_build"
mv $(zigi exe wrenshbox) "$tools"
mv $(zigi exe sha256sum) "$tools"

# wrensh
ln -s "$BUILD_DEPS/wrensh/bin/$(zigi exe wrensh)" "$tools"

# zig+busybox links
zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/$(zigi exe busybox)"
if [ "$mode" = "release" ]
then
  cp -rL "$zig" "$out/zig"
  cp "$bb" "$tools"
else
  ln -s "$zig" "$out/zig"
  ln -s "$bb" "$tools"
fi
ln -s ../zig/$(zigi exe zig) "$tools"/zig

# install internal tools
scripts="
ar
c++
cc
fetch
fetch_untar
fetch_urltxt
ld
need
needtool
pkg-config
rc
untar
windres
xos_internal_link_tools
xos_internal_mktemp
xos_internal_pkgid
zigi
"
for script in $scripts
do
  s="$BUILD_PKG/src/$script"
  if [ "$mode" = "release" ]
  then
    cp "$s" "$tools"
  else
    ln -s "$s" "$tools"
  fi
done

# install busybox links
bbtools="
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
for tool in $bbtools
do
  ln -s $(zigi exe busybox) "$tools/$tool"
done

# install wrenshbox links
wrenshboxtools="
echo
dirname
basename
"
for tool in $wrenshboxtools
do
  ln -s $(zigi exe wrenshbox) "$tools/$tool"
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
  mv "$out"/build "$BUILD_OUT"
  mv "$out"/tools "$BUILD_OUT"
  mv "$out"/zig "$BUILD_OUT"
  mv "$out"/.xos_id "$BUILD_OUT"
  mv "$out"/.xos_host "$BUILD_OUT"
fi
