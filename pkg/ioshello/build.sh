need iossdk


export ZIG_LIBC=$(zigi libc iossdk | cut -d' ' -f2)
cc -target $TARGET -O$OPT \
	--sysroot="$BUILD_DEPS/iossdk/sdk" \
  $(pkg-config --cflags iossdk) \
	-Wno-everything \
	-fobjc-abi-version=2 -fobjc-arc -fobjc-weak -fobjc-legacy-dispatch \
  $BUILD_PKG/hello.m \
	-o hello \
	-framework UIKit -framework Foundation

cd "$BUILD_OUT"
mkdir Hello.app
cp "$HOME/$(zigi exe hello)" Hello.app
cp "$BUILD_PKG/Info.plist" Hello.app
