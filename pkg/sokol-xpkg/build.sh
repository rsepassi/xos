userpkg=$1
need $userpkg
need sokol_gfx
need sokol_app
need sokol_zig

finallinkargs="$(pkg-config --libs $userpkg) $(pkg-config --libs sokol_gfx sokol_app)"
if [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ABI" = "android" ]
then
  buildarg="-dynamic"
  pcarg="--libs"
  linkarg="$finallinkargs"
fi

zig build-lib $buildarg -target $TARGET -O $OPT_ZIG \
  --name app \
  --dep sokol_zig \
  --dep app=$userpkg \
  -Mmain="$BUILD_PKG/app_wrap.zig" \
  $(pkg-config --zig sokol_zig $userpkg) \
  $linkarg \
  -lc

# todo:
# android device

if [ "$TARGET_OS" = "ios" ]
then
  if [ "$TARGET_ABI" = "simulator" ]
  then
    platform=iphonesimulator
  else
    platform=iphoneos
  fi

  need xcodeproj
  cp -r $BUILD_DEPS/xcodeproj/xos-app .

  config=Release
  HOME=$XOS_SYSTEM_HOME \
  system xcodebuild \
    -configuration $config \
    -target xos-app \
    -arch arm64 \
    -sdk ${platform}17.2 \
    -project ./xos-app/xos-app.xcodeproj \
    "OTHER_LDFLAGS=$HOME/$(zigi lib app) $finallinkargs" \
    build

  appdir=$PWD/xos-app/build/$config-$platform/xos-app.app
  ln -s $appdir "$BUILD_OUT"

  # appid=com.istudios.xos-app.hello
  #
  # Device
  # did=ryansiphone
  # xcrun devicectl device uninstall app --device $did $appid
  # xcrun devicectl device install app --device $did build/out/xos-app.app
  # xcrun devicectl device process launch --device $did $appid
  #
  # Sim
  # open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app
  # xcrun simctl uninstall booted $appid
  # xcrun simctl install booted build/out/xos-app.app
  # xcrun simctl launch booted $appid
elif [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ABI" = "android" ]
then
  # For Android, everything must have been linked into a shared object file.
  needtool androidsdk
  needtool android-template

  . $BUILD_TOOLS/androidsdk/env.sh
  $BUILD_TOOLS/android-template/bin/new_project xos.hello Hello
  cd Hello

  mv $HOME/libapp.so app/src/main/jniLibs/arm64-v8a
  ./gradlew :app:assembleRelease
  apksigner sign --ks $ANDROID_HOME/keystore/debug.keystore --ks-pass pass:android \
    app/build/outputs/apk/release/app-release-unsigned.apk
  ln -s $PWD/app/build/outputs/apk/release $BUILD_OUT/apk

  # export XDG_CACHE_HOME=$PWD/build/cache/xdg
  # . ./pkg/androidsdk/env.sh
  # emulator -avd testEmulator -wipe-data -no-boot-anim -netdelay none -no-snapshot
  # adb install build/out/apk/app-release-unsigned.apk
  # adb shell am start -n com.xos.hello/android.app.NativeActivity
else
  touch app.c
  zig build-exe -target $TARGET -O $OPT_ZIG \
    app.c \
    $(zigi lib app) \
    $finallinkargs
  cd $BUILD_OUT
  mkdir bin
  mv $HOME/$(zigi exe app) bin
fi
