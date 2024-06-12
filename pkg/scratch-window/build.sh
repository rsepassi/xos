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
--dep dummydata
-Muserlib=$BUILD_PKG/app.zig
--dep app=main
-Mdummydata=$BUILD_PKG/data.zig
"


link="
$(pkg-config --libs $deps)
"

needtool apppkg
USER_MODULE="$module" \
USER_LINK="$link" \
$BUILD_TOOLS/apppkg/bin/apppkg
