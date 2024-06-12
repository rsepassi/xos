src=$(fetch_untar "https://api.github.com/repos/rsepassi/zig-lmdb/tarball/8bd7331" \
  "lmdb-zig" \
  "d9b9b51dd391fa0415daa75d0d579aff7d600cb6a0353676ecd757e5f90f6e19")

cp -r "$src/src" "$BUILD_OUT/zig"
