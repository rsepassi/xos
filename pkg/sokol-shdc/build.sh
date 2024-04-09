version="5b8d524"

macos_aarch64="https://github.com/floooh/sokol-tools-bin/raw/$version/bin/osx_arm64/sokol-shdc"
macos_aarch64_hash="ae4064824ea079d10cdc5e0aef8e3a11308ef4acc0b64add651194620f5f7037"

linux_x86_64="https://github.com/floooh/sokol-tools-bin/raw/$version/bin/linux/sokol-shdc"
linux_x86_64_hash="fffc93a057ae27fbdf98822a87a7419cdcda3163a3842b65da2a14b886cc15a5"


url=$(eval "echo \$${TARGET_OS}_${TARGET_ARCH}")
hash=$(eval "echo \$${TARGET_OS}_${TARGET_ARCH}_hash")

if [ -z "$url" ]
then
  >&2 echo "arch-os combo unsupported"
  exit 1
fi

fetch $url sokol-shdc "$hash"
chmod +x "$BUILD_DEPS/sokol-shdc"

cd "$BUILD_OUT"
mkdir bin
ln -s "$BUILD_DEPS/sokol-shdc" bin

# https://github.com/floooh/sokol-tools/blob/master/docs/sokol-shdc.md
# Vulkan-style GLSL version 450

# example: sokol-shdc --input shader1.glsl --output shader1.h --slang metal_macos
# linux
# glsl300es: GLES3 / WebGL2
# macos
# metal_macos: Metal on macOS
# ios
# metal_ios: Metal on iOS device
# ios sim
# metal_sim: Metal on iOS simulator
# windows
# hlsl4: D3D11
# hlsl5: D3D11
# web
# wgsl: WebGPU
