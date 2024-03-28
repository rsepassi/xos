src=$(fetch_untar \
  "https://api.github.com/repos/michaelforney/samurai/tarball/217059f" \
  samurai.tar.gz \
  "29fd1c1c4b7725fd85f627b2b5123371467bc9481c26f29b99b2c0a49c772d42")
cd $src

srcs="
samu.c
build.c
deps.c
env.c
graph.c
htab.c
log.c
parse.c
scan.c
tool.c
tree.c
util.c
"

patch -p1 < "$BUILD_PKG/samu.patch"

cp \
  "$BUILD_PKG"/build_internal.h \
  "$BUILD_PKG"/build.zig \
  .

zig build-lib -target $TARGET -O $OPT_ZIG \
  -I. build.zig -lc
zig build-exe -target $TARGET -O $OPT_ZIG \
  $srcs $(zigi lib build) -lc

mkdir "$BUILD_OUT"/bin
mv $(zigi exe samu) "$BUILD_OUT"/bin
