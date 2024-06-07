needtool rust
PATH="$BUILD_TOOLS/rust/.cargo/bin:$PATH"

src=$(fetch_untar "https://api.github.com/repos/gfx-rs/wgpu-native/tarball/33133da" \
  wgpu.tar.gz \
  "bb55824cda330f594ca23864cfc5a0b0b9b483174baa8807edb8caabfba02708")
cd $src

fetch "https://raw.githubusercontent.com/webgpu-native/webgpu-headers/aef5e428a1fdab2ea770581ae7c95d8779984e0a/webgpu.h" \
  webgpu.h \
  "defb965756966d04186f80fb193994cfa70b375247da7a34d20608662216a50f"
cp $BUILD_DEPS/webgpu.h ffi/webgpu-headers

if [ "$TARGET_OS" = "macos" ]
then
  need macossdk
  ldflags="$(pkg-config --libs macossdk) -framework Metal -framework QuartzCore"
elif [ "$TARGET_OS" = "linux" ]
then
  if [ "$TARGET_ABI" = "android" ]
  then
    ldflags=""
  else
    ldflags="-lunwind"
  fi
elif [ "$TARGET_OS" = "windows" ]
then
  ldflags="--subsystem windows -lunwind -ldxgi -ld3d11 -lkernel32 -luser32 -L$XOS_SYSTEM_HOME/winsdk -ld3dcompiler"
elif [ "$TARGET_OS" = "ios" ]
then
  need iossdk
  export SDKROOT="$BUILD_DEPS/iossdk/sdk"
fi

sed -i 's/"cdylib",//g' Cargo.toml

cat <<EOF >> Cargo.toml
[profile.release]
panic = "abort"
EOF

xcargo build \
  --target $(rusti target) \
  --release \
  --lib

cd $BUILD_OUT
mkdir lib include
cp $src/ffi/wgpu.h include
cp $src/ffi/webgpu-headers/webgpu.h include
mv $src/target/$(rusti target)/release/libwgpu_native.a lib/$(zigi lib wgpu)
pkg-config --gendefault wgpu --ldflags "$ldflags"
