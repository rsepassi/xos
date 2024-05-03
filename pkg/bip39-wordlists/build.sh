src=$(fetch_untar "https://api.github.com/repos/bitcoin/bips/tarball/24a15a6" \
  "bips.tar.gz" \
  "e7d44949620d15ac039f24b951885c803f21077cc79b70d66770d333696c46ab")

cd $BUILD_OUT
mkdir share
ln -s $src/bip-0039/*.txt share
