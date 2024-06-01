needtool rust
. $BUILD_TOOLS/rust/.cargo/env

src=$(fetch_untar "https://github.com/gfx-rs/wgpu/archive/refs/tags/v0.20.0.tar.gz" \
  "wgpu.tar.gz" \
  "19ea65304aa66b397eeeeb1526f2573bdf43a32a3e04d539e19bc2eac17893b4")
cd $src

out=$(mktemp -d)

xcargo build --target $(rusti target) --release \
  --bin naga

cd $BUILD_OUT
mkdir bin
mv $src/target/$(rusti target)/release/naga bin
