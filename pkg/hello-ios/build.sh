# build2 hello-iossim TARGET=aarch64-ios

need iossdk
need xcodeproj

zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags iossdk) \
  --name xos \
  $BUILD_PKG/hello.m \
	-framework UIKit -framework Foundation

cp -r "$BUILD_DEPS/xcodeproj/xos-app" .
mv libxos.a xos-app/xos-app/

ln -s $PWD/xos-app $BUILD_OUT

# then cp the directory somewhere, open it in xcode, and run it on a real phone
