fetch_urltxt $BUILD_PKG/url.txt make.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/make.tar.gz $src
cd $src
cp $BUILD_PKG/build.zig .
cp $BUILD_PKG/arch/config-$TARGET_OS-$TARGET_ARCH.h src/config.h
cp $BUILD_PKG/genheaders/alloca.h lib/
cp $BUILD_PKG/genheaders/fnmatch.h lib/
cp $BUILD_PKG/genheaders/glob.h lib/
printf '%s\n%s\n' "#include <makeint.h>" "$(cat lib/concat-filename.c)" > lib/concat-filename.c
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG
ln -s $PWD/zig-out/bin $BUILD_OUT
