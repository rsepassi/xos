need macossdk

zig build-exe -target $TARGET -O $OPT_ZIG \
  $(pkg-config --cflags macossdk) \
  -cflags \
  -fobjc-arc \
  -mmacosx-version-min=13.0 \
  -- \
  -framework Cocoa \
  "$BUILD_PKG"/hello.m

root=Hello.app/Contents
mkdir -p $root/MacOS $root/Resources
mv hello $root/MacOS
cp "$BUILD_PKG"/Info.plist $root
cp "$BUILD_PKG"/AppIcon.icns $root/Resources

cd "$BUILD_OUT"
mv "$HOME/Hello.app" .
