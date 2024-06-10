share="
$BUILD_PKG/android.c
$BUILD_PKG/appgpu.zig
$BUILD_PKG/appwrap.zig
$BUILD_PKG/gpu.zig
$BUILD_PKG/ios.m
$BUILD_PKG/twod.zig
"
cd $BUILD_OUT
mkdir bin share
cp $BUILD_PKG/pkg.sh bin/apppkg
cp $share share/

