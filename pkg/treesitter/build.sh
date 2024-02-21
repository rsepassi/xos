needtool rust

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
fi

url="https://github.com/tree-sitter/tree-sitter/archive/refs/tags/v0.20.9.tar.gz"
hash="9b2fd489a7281e3a7e5e7cbbf3a974e5a6a115889ae65676d61b79bdae96464e"
file="treesitter.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

PATH="$BUILD_TOOLDEPS/rust/.cargo/bin:$PATH"
xcargo build \
  --target $(rusti target) \
  --release \
  --quiet \
  --bin tree-sitter

mkdir "$BUILD_OUT/bin"
mv target/$(rusti target)/release/tree-sitter "$BUILD_OUT/bin"
