need sokol_app
need sokol_debugtext
need sokol_zig

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name app \
  $(pkg-config --cflags sokol_debugtext) \
  --dep sokol_zig \
  -Mmain="$BUILD_PKG/app.zig" \
  $(pkg-config --zig sokol_zig) \
  -lc

# todo:
# android sim
# android device

if [ "$TARGET_OS" = "ios" ]
then
  if [ "$TARGET_ABI" = "simulator" ]
  then
    platform=iphonesimulator
  else
    platform=iphoneos
  fi

  genlinkflags() {
    echo "$HOME/$(zigi lib app)"
    pkg-config --cflags --libs sokol_debugtext sokol_app
  }
  linkflags=$(genlinkflags)

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
    "OTHER_LDFLAGS=$linkflags" \
    build

  appdir=$PWD/xos-app/build/$config-$platform/xos-app.app
  ln -s $appdir "$BUILD_OUT"

  # appid=com.istudios.xos-app.hello
  #
  # Device
  # xcrun devicectl device uninstall app --device ryansiphone $appid
  # xcrun devicectl device install app --device ryansiphone build/out/xos-app.app
  # xcrun devicectl device process launch --device ryansiphone $appid
  #
  # Sim
  # xcrun simctl uninstall booted $appid
  # xcrun simctl install booted build/out/xos-app.app
  # xcrun simctl launch booted $appid
else
  touch app.c
  zig build-exe -target $TARGET -O $OPT_ZIG \
    app.c \
    $(zigi lib app) \
    $(pkg-config --cflags --libs sokol_debugtext sokol_app)
  cd $BUILD_OUT
  mkdir bin
  mv $HOME/$(zigi exe app) bin
fi
