cd $BUILD_OUT
mkdir zig pkgconfig
cp $BUILD_PKG/unwind.zig zig
cat <<EOF > pkgconfig/unwind.pc
ZRoot: \${rootdir}/zig/unwind.zig
EOF
