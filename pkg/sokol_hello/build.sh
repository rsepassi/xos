need sokol_debugtext
need sokol_zig

cd $BUILD_OUT
mkdir zig pkgconfig
cp $BUILD_PKG/app.zig zig
cat <<EOF > pkgconfig/sokol_hello.pc
ZRoot: \${rootdir}/zig/app.zig
ZRequires: sokol_zig
Requires: sokol_debugtext
EOF
