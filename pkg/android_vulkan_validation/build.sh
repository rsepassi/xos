src=$(fetch_untar "https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.3.283.0/android-binaries-1.3.283.0.tar.gz" \
  vulkanbin.tar.gz \
  "1ace107c0d99dec77ac0e24f481077ba2271ad319d1b48708c776fe457415bae")
cd $src

if [ "$TARGET_ARCH" = "aarch64" ] && [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ABI" = "android" ]
then
  echo
else
  >&2 echo "only supported on arm64 android"
  exit 1
fi

mkdir $BUILD_OUT/lib
mv $src/arm64-v8a/libVkLayer_khronos_validation.so $BUILD_OUT/lib
