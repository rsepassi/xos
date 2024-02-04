#!/usr/bin/env sh
set -e

ARCH_HOST=${ARCH_HOST:-"x86_64-linux-musl"}
BOOTSTRAP_OUT=${BOOTSTRAP_OUT:-"../bootstrap_out"}
if [ ! -d "$BOOTSTRAP_OUT" ]
then
  echo "\$BOOTSTRAP_OUT must be set and exist. set to $BOOTSTRAP_OUT"
  exit 1
fi

XOS_PKG="$(realpath "$(dirname "$(dirname "$0")")")"
xos_root="$(mktemp -d)"
tools="$xos_root/tools"
mkdir -p "$tools"
touch "$xos_root/.xos"

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
cp "$XOS_PKG/$script" "$tools"
done

mktemp="$(which mktemp)"
cat <<EOF > "$tools/internal_mktemp"
#!/usr/bin/env sh
set -e
$mktemp \$@
EOF
  chmod +x "$tools/internal_mktemp"

XOS_BOOTSTRAP=1 \
ARCH_HOST=$ARCH_HOST \
  "$tools/build" zig
ln -s "$(realpath ./build/out/zig)" "$tools"

XOS_BOOTSTRAP=1 \
ARCH_HOST=$ARCH_HOST \
  "$tools/build" xos

# build xos with bootstrap xos
xos1=$(./build/out/tools/build xos)

# build xos with xos
xos2=$(./build/out/tools/build xos)

# make sure there's no change
if [ "$xos1" != "$xos2" ]
then
  echo "failed bootstrap"
  exit 1
fi

echo "$xos1"

# package it up
xos_dir=$(realpath build/out)
# copy zig
zig_dir=$(realpath "$xos_dir"/zig)
rm "$xos_dir"/zig
cp -r $zig_dir "$xos_dir"/zig
# copy busybox
bb_bin=$(realpath "$xos_dir"/tools/busybox)
rm "$xos_dir"/tools/busybox
cp $bb_bin "$xos_dir"/tools

tmp=$(mktemp -d)
mv $xos_dir $tmp/xos
cd $tmp
tar cJf xos.tar.xz xos
mv xos.tar.xz $BOOTSTRAP_OUT
