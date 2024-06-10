need freetype

deps="
freetype
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

needtool apppkg
USER_MODULE="$module" \
USER_LINK="$link" \
$BUILD_TOOLS/apppkg/bin/apppkg
