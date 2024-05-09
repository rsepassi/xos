src=$(fetch_untar "https://api.github.com/repos/rsepassi/zigcoro/tarball/0de259f" \
  zigcoro.tar.gz \
  "12236378add84de7652984517fb5a0e8badf1f02600cdb71737f1a239b87222e")
cd $src

cp "$BUILD_PKG/options.zig" $src/src
ln -s $src/src "$BUILD_OUT/zig"
cat <<EOF > "$BUILD_OUT/zig/zigcoro.pc"
ZRoot: \${rootdir}/zig/main.zig
ZLocalRequires: libcoro_options=opts
EOF
cat <<EOF > "$BUILD_OUT/zig/opts.pc"
ZRoot: \${rootdir}/zig/options.zig
EOF
