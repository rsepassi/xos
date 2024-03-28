src=$(fetch_untar \
  "https://api.github.com/repos/troydhanson/uthash/tarball/eeba196" \
  uthash.tar.gz \
  "42aab1f52675b3b362b14cd839b4c8de3eb129909a710b08b16c97f60bdf57b2")
cd $src

cd "$BUILD_OUT"
mkdir include pkgconfig
cp $src/src/* include
cat <<EOF > $BUILD_OUT/pkgconfig/uthash.pc
Cflags: -I\${rootdir}/include
EOF
