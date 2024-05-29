
if [ "$TARGET_OS" = "ios" ]
then
  need iossdk
  need wgpu

  finallinkargs="-framework Foundation -framework UIKit -framework Metal -framework MetalKit $(pkg-config --libs wgpu)"

	zig build-lib -target $TARGET -O $OPT_ZIG \
    --name app \
    $(pkg-config --cflags iossdk wgpu) \
    $(zigi libc iossdk) \
    $BUILD_PKG/ios.m \
    $BUILD_PKG/main-ios.zig

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
open /Applications/Xcode.app/Contents/Developer/Applications/Simulator.app
nprocesses=\$(ps ax | grep Simulator.app | wc -l)
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
did=\${1:?must provide deviceid}
appid=com.istudios.xos-app.hello
xcrun devicectl device uninstall app --device \$did \$appid
xcrun devicectl device install app --device \$did $BUILD_OUT/xos-app.app
xcrun devicectl device process launch --device \$did \$appid
EOF
  fi

  chmod +x $BUILD_OUT/run.sh
else
  need wgpu
  need glfw
  need wgpu_glfw_glue

  # Note: has to be dynamically linked to libc for dlopen to work
  zig build-exe -dynamic -target $TARGET -O $OPT_ZIG \
    --name app \
    $(pkg-config --cflags --libs glfw wgpu wgpu_glfw_glue) \
    $BUILD_PKG/main.zig \
    -lc

  cd $BUILD_OUT
  mkdir bin
  mv $HOME/$(zigi exe app) bin
fi
