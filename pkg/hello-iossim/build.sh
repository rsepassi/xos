# build2 hello-iossim TARGET=aarch64-ios-simulator

need iossdk
need xcodeproj

zig build-lib -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags iossdk) \
  --name xos \
  $BUILD_PKG/hello.m \
	-framework UIKit -framework Foundation

cp -r "$BUILD_DEPS/xcodeproj/xos-app" .
mv libxos.a xos-app/xos-app/

config=Release
platform=iphonesimulator

system xcodebuild \
  -configuration $config \
  -target xos-app \
  -arch arm64 \
  -sdk ${platform}17.2 \
  -project ./xos-app/xos-app.xcodeproj


appdir=$PWD/xos-app/build/$config-$platform/xos-app.app
ln -s $appdir "$BUILD_OUT"

appid=com.istudios.xos-app.hello
system xcrun simctl uninstall booted $appid
system xcrun simctl install booted $BUILD_OUT/xos-app.app
system xcrun simctl launch booted $appid
