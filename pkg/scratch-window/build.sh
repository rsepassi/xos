need freetype
need nanovg

deps="
freetype
nanovg
"

module="
$(pkg-config --cflags $deps)
--dep app=main
--dep gpu
--dep appgpu
-Muserlib=$BUILD_PKG/app.zig
"


link="
$(pkg-config --libs $deps)
"

mkdir resources
cp $BUILD_PKG/CourierPrime-Regular.ttf resources

needtool apppkg
USER_MODULE="$module" \
USER_LINK="$link" \
USER_RESOURCES="$PWD/resources" \
$BUILD_TOOLS/apppkg/bin/apppkg
