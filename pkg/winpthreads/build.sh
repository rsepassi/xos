[ "$TARGET_OS" = "windows" ] || { >&2 echo "winpthreads only available on Windows"; exit 1; }

url="https://api.github.com/repos/kassane/winpthreads-zigbuild/tarball/5dcdfe3"
hash="a1ffe4c5cc755180fdabf8963fbb2c3c49cb8776a3b21a8df1456b99523f6f26"
file="winpthreads.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG -Dcpu=baseline

mv zig-out/lib "$BUILD_OUT"
mv zig-out/include "$BUILD_OUT"
mkdir "$BUILD_OUT/pkgconfig"
cat <<EOF > "$BUILD_OUT/pkgconfig/winpthreads.pc"
Cflags: -I \${rootdir}/include
Libs: \${rootdir}/lib/winpthreads.lib
EOF
