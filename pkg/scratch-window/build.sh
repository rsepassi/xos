need freetype

userlibmodule="
$(pkg-config --cflags freetype)
--dep app=main
--dep gpu
--dep appgpu
-Muserlib=$BUILD_PKG/demoapp/app.zig
"

userliblink="
$(pkg-config --libs freetype)
"

if [ "$TARGET_OS" = "ios" ]
then
  need iossdk
  need wgpu

  finallinkargs="-framework Foundation -framework UIKit -framework Metal -framework MetalKit $(pkg-config --libs wgpu)"

  zig build-lib -target $TARGET -O $OPT_ZIG \
    --name app \
    $(pkg-config --cflags iossdk) \
    $(zigi libc iossdk) \
    $BUILD_PKG/ios.m \
    --dep userlib=userlib \
    -Mmain=$BUILD_PKG/appwrap.zig \
    $userliblink \
    $userlibmodule \
    $(pkg-config --cflags wgpu) \
    -Mgpu=$BUILD_PKG/gpu.zig \
    --dep app=main \
    --dep gpu \
    -Mappgpu=$BUILD_PKG/appgpu.zig \
    -lc

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

  if [ "$TARGET_ABI" = "simulator" ]
  then
  cat <<EOF > $BUILD_OUT/run.sh
#!/usr/bin/env sh
set -ex
appid=com.istudios.xos-app.hello
nprocesses=\$(ps ax | grep Simulator.app | wc -l)
open -a Console
open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app
if [ \$nprocesses -lt 2 ]
then
  sleep 5
fi
xcrun simctl uninstall booted \$appid
xcrun simctl install booted $BUILD_OUT/xos-app.app
xcrun simctl launch booted \$appid
EOF
  else
  cat <<EOF > $BUILD_OUT/run.sh
#!/usr/bin/env sh
set -ex
did="\${1:?must provide deviceid}"
appid=com.istudios.xos-app.hello
xcrun devicectl device uninstall app --device "\$did" \$appid
xcrun devicectl device install app --device "\$did" $BUILD_OUT/xos-app.app
xcrun devicectl device process launch --device "\$did" \$appid
EOF
  fi

  chmod +x $BUILD_OUT/run.sh
elif [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ABI" = "android" ]
then
  need wgpu
  needtool androidsdk
  . $BUILD_TOOLS/androidsdk/env.sh

  cflags="
    $(BUILD_DEPS=$BUILD_TOOLS pkg-config --cflags --libs androidsdk)
    $(BUILD_DEPS=$BUILD_TOOLS zigi libc androidsdk)
    -landroid -lnativewindow -llog -lvulkan
  "

  zig build-lib -dynamic -target $TARGET -O $OPT_ZIG \
    --name app \
    $cflags \
    $BUILD_PKG/android.c \
    $BUILD_TOOLS/androidsdk/sdk/ndk-bundle/sources/android/native_app_glue/android_native_app_glue.c \
    --dep userlib=userlib \
    -Mmain=$BUILD_PKG/appwrap.zig \
    $userliblink \
    $userlibmodule \
    $(pkg-config --cflags --libs wgpu) \
    -Mgpu=$BUILD_PKG/gpu.zig \
    --dep app=main \
    --dep gpu \
    -Mappgpu=$BUILD_PKG/appgpu.zig \
    -ldl -lc

  # For Android, everything must have been linked into a shared object file.
  needtool android-template
  $BUILD_TOOLS/android-template/bin/new_project xos.hello Hello
  cd Hello

  mv $HOME/libapp.so app/src/main/jniLibs/arm64-v8a

  if [ $OPT_ZIG = "Debug" ]
  then
    need android_vulkan_validation
    cp $BUILD_DEPS/android_vulkan_validation/lib/*.so app/src/main/jniLibs/arm64-v8a
  fi

  2>&1 ./gradlew :app:assembleRelease
  apksigner sign --ks $ANDROID_HOME/keystore/debug.keystore --ks-pass pass:android \
    app/build/outputs/apk/release/app-release-unsigned.apk
  ln -s $PWD/app/build/outputs/apk/release $BUILD_OUT/apk

  cat <<EOF > $BUILD_OUT/run.sh
#!/usr/bin/env sh
set -ex

launch=\${1:-emu}

export XDG_CACHE_HOME=$XOS_BUILD_ROOT/cache/xdg
. $XOS_PKG_ROOT/androidsdk/env.sh

appid=com.xos.hello

if [ "\$launch" = "emu" ]
then
  device="-e"
  nprocesses=\$(ps ax | grep testEmulator | wc -l)
  if [ \$nprocesses -lt 2 ]
  then
    emulator -avd testEmulator -wipe-data -no-boot-anim -netdelay none -no-snapshot 2>&1 >/dev/null &
    sleep 10
  fi
else
  # this keeps the screen on, not sure how to include it
  echo 'to keep device on, run this in a separate shell'
  echo 'while true; do adb -d shell input keyevent mouse ; sleep 1 ; done'
  device="-d"
fi
adb \$device uninstall \$appid || echo
adb \$device install $BUILD_OUT/apk/app-release-unsigned.apk
adb \$device shell am start -n \$appid/android.app.NativeActivity
adb \$device logcat | grep 'NativeActivity:'
EOF
  chmod +x $BUILD_OUT/run.sh
else
  need wgpu
  need glfw
  need wgpu_glfw_glue

  zig build-exe -dynamic -target $TARGET -O $OPT_ZIG \
    --name app \
    $(pkg-config --cflags --libs glfw) \
    --dep userlib=userlib \
    -Mmain=$BUILD_PKG/appwrap.zig \
    $userliblink \
    $userlibmodule \
    $(pkg-config --cflags --libs wgpu) \
    -Mgpu=$BUILD_PKG/gpu.zig \
    $(pkg-config --cflags --libs wgpu_glfw_glue) \
    --dep app=main \
    --dep gpu \
    -Mappgpu=$BUILD_PKG/appgpu.zig \
    -lc

  cd $BUILD_OUT
  mkdir bin
  mv $HOME/$(zigi exe app) bin
fi
