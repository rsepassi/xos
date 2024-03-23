need zig
need busybox
need wrensh
need wrenshbox
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

# setup output dirs
out="$(mktemp -d)"
tools="$out/tools"
scripts="$out/tools/scripts"
mkdir -p $tools $scripts

# XOS executables
mv $(zigi exe xos_build) "$out/$(zigi exe build)"
mv $(zigi exe sha256sum) "$tools"

# zig+busybox+wrensh+wrenshbox
zig="$BUILD_DEPS/zig"
bb="$BUILD_DEPS/busybox/bin/$(zigi exe busybox)"
wrensh="$BUILD_DEPS/wrensh/bin/$(zigi exe wrensh)"
wrenshbox="$BUILD_DEPS/wrenshbox/bin/$(zigi exe wrenshbox)"
if [ "$mode" = "release" ]
then
  cp -rL "$zig" "$out/zig"
  cp "$bb" "$tools"
  cp "$wrensh" "$tools"
  cp "$wrenshbox" "$tools"
else
  ln -s "$zig" "$out/zig"
  ln -s "$bb" "$tools"
  ln -s "$wrensh" "$tools"
  ln -s "$wrenshbox" "$tools"
fi

# install internal tools
script_tools="
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
xos_build.wren
xos_internal_mktemp
xos_internal_pkgid
zigi
"
for script in $script_tools
do
  s="$BUILD_PKG/src/$script"
  d="$scripts/$(zigi exe $script)"
  if [ "$mode" = "release" ]
  then
    cp "$s" "$d"
  else
    ln -s "$s" "$d"
  fi
done

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
    "$BUILD_TOOLS/libarchive/bin/bsdtar" -c --format zip -f xos.zip xos
  else
    tar czf xos.tar.gz xos
  fi
else
  mv "$out"/$(zigi exe build) "$BUILD_OUT"
  mv "$out"/tools "$BUILD_OUT"
  mv "$out"/zig "$BUILD_OUT"
  mv "$out"/.xos_id "$BUILD_OUT"
  mv "$out"/.xos_host "$BUILD_OUT"
fi
