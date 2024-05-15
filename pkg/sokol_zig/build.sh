src=$(fetch_untar "https://api.github.com/repos/floooh/sokol-zig/tarball/864422a" \
  sokol-zig.tar.gz \
  "a110c5e5f3a9f9a4aabbff31dc75ab70e31ff50e0b0f8f12e7f94dc5ae567d77")
cd $src

cd $BUILD_OUT
mkdir zig
cp $src/src/sokol/*.zig zig/
cat <<EOF > zig/sokol_zig.pc
ZRoot: \${rootdir}/zig/sokol.zig
EOF
