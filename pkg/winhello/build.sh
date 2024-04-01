need sokol

mkdir app
cp "$BUILD_DEPS/sokol/bin/demo.exe" app
cp "$BUILD_PKG/AppxManifest.xml" app
cp -r "$BUILD_PKG/images" app

system makemsix pack -d app -p app.msix

mv app.msix "$BUILD_OUT"
