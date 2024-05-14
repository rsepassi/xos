needtool androidsdk
needtool android-template

. $BUILD_TOOLS/androidsdk/env.sh

# create new project
$BUILD_TOOLS/android-template/bin/new_project xos.hello Hello
cd Hello

# build
./gradlew assembleRelease

# sign
apksigner sign --ks $ANDROID_HOME/keystore/debug.keystore --ks-pass pass:android \
  app/build/outputs/apk/release/app-release-unsigned.apk

ln -s $PWD/app/build/outputs/apk/release $BUILD_OUT/apk

# to launch the emulator
# in a different terminal
# XDG_CACHE_HOME=$PWD/build/cache/xdg
# . ./pkg/androidsdk/env.sh
# emulator -avd testEmulator
# pass -wipe-data to start fresh

# install
# adb install app/build/outputs/apk/release/app-release-unsigned.apk

# run
# adb shell am start -n com.xos_org.xos_pkg_name/.MainActivity
