needtool rust

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
fi

url="https://github.com/BurntSushi/ripgrep/archive/refs/tags/14.1.0.tar.gz"
hash="33c6169596a6bbfdc81415910008f26e0809422fda2d849562637996553b2ab6"
file="ripgrep.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

# jemalloc xcompile is broken
patch -p1 < "$BUILD_PKG/rm_jemalloc.patch"

PATH="$BUILD_TOOLDEPS/rust/.cargo/bin:$PATH"
xcargo build \
  --target $(rusti target) \
  --release \
  --quiet \
  --bin rg

mkdir "$BUILD_OUT/bin"
mv target/$(rusti target)/release/rg "$BUILD_OUT/bin"
