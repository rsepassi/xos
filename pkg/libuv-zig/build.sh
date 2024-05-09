need libuv
need zigcoro

mkdir "$BUILD_OUT/zig"
cp "$BUILD_PKG/"*.zig "$BUILD_OUT/zig"

cat <<EOF > "$BUILD_OUT/zig/uv.pc"
ZRoot: \${rootdir}/zig/uv.zig
ZRequires: zigcoro c=libuv/uv
EOF
