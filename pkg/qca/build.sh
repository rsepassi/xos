need libarchive
need busybox
need curl
need ssserve
need wrensh

cd "$BUILD_OUT"
mkdir bin
cd bin
cp "$BUILD_DEPS"/libarchive/bin/$(zigi exe bsdtar) .
cp "$BUILD_DEPS"/busybox/bin/$(zigi exe busybox) .
cp "$BUILD_DEPS"/curl/bin/$(zigi exe curl) .
cp "$BUILD_DEPS"/ssserve/bin/$(zigi exe ssserve) .
cp "$BUILD_DEPS"/wrensh/bin/$(zigi exe wrensh) .
