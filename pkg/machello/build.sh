need macossdk
sdk="$BUILD_DEPS/macossdk/sdk"
cc -target $TARGET -O$OPT \
  -fobjc-arc \
  --sysroot=$sdk \
  -DTARGET_OS_OSX=1 \
  -I $sdk/usr/include \
  -F $sdk/System/Library/Frameworks \
  -mmacosx-version-min=13.0 \
  -framework Cocoa -framework WebKit \
  -Wno-everything \
  -o hello "$BUILD_PKG"/hello.m

root=Hello.app/Contents
mkdir -p $root/MacOS $root/Resources
mv hello $root/MacOS
cp "$BUILD_PKG"/Info.plist $root
cp "$BUILD_PKG"/AppIcon.icns $root/Resources

cd "$BUILD_OUT"
mv "$HOME/Hello.app" .
