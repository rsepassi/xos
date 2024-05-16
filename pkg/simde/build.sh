src=$(fetch_untar "https://github.com/simd-everywhere/simde/releases/download/v0.8.2/simde-amalgamated-0.8.2.tar.xz" \
  simde.tar.xz \
  "59068edc3420e75c5ff85ecfd80a77196fb3a151227a666cc20abb313a5360bf")
cd $src

cd $BUILD_OUT
ln -s $src include
mkdir pkgconfig
cat <<EOF > pkgconfig/simde.pc
Cflags: -I\${rootdir}/include
EOF
