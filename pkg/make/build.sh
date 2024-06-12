src=$(fetch_untar http://mirrors.ocf.berkeley.edu/gnu/make/make-4.4.tar.gz \
  make.tar.gz \
  "581f4d4e872da74b3941c874215898a7d35802f03732bdccee1d4a7979105d18")
cd $src

cp $BUILD_PKG/build.zig .
cp $BUILD_PKG/arch/config-$TARGET_OS-$TARGET_ARCH.h src/config.h
cp $BUILD_PKG/genheaders/alloca.h lib/
cp $BUILD_PKG/genheaders/fnmatch.h lib/
cp $BUILD_PKG/genheaders/glob.h lib/
printf '%s\n%s\n' "#include <makeint.h>" "$(cat lib/concat-filename.c)" > lib/concat-filename.c
zig build -Dtarget=$TARGET -Doptimize=$OPT_ZIG
mv $PWD/zig-out/bin $BUILD_OUT
