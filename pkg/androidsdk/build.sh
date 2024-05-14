# requires java
# brew install openjdk

if [ "$HOST_OS" != "macos" ]
then
  >&2 echo "androidsdk build script only supports mac currently"
  exit 1
fi

fetch "https://dl.google.com/android/repository/commandlinetools-mac-11076708_latest.zip" \
  commandlinetools.zip \
  "7bc5c72ba0275c80a8f19684fb92793b83a6b5c94d4d179fc5988930282d7e64"

mkdir -p $XDG_CACHE_HOME/android
cd $XDG_CACHE_HOME/android

unzip $BUILD_DEPS/commandlinetools.zip
mv cmdline-tools latest
mkdir cmdline-tools
mv latest cmdline-tools/

PATH="$PWD/cmdline-tools/latest/bin:$PATH"

export JAVA_HOME=$(/usr/libexec/java_home)
export ANDROID_HOME=$XDG_CACHE_HOME/android
export ANDROID_SDK_ROOT=$ANDROID_HOME
export ANDROID_AVD_HOME=$ANDROID_HOME/avd
export REPO_OS_OVERRIDE=macosx

packages='ndk-bundle platforms;android-34 platform-tools emulator system-images;android-29;google_apis;arm64-v8a'
sdkmanager $packages
busybox yes | sdkmanager --licenses
sdkmanager $packages

mkdir -p $ANDROID_HOME/keystore
cp $BUILD_PKG/debug.keystore $ANDROID_HOME/keystore

mkdir -p $ANDROID_AVD_HOME
if [ ! -f "$ANDROID_AVD_HOME/testEmulator.ini" ]
then
  echo no | avdmanager create avd -n testEmulator -k "system-images;android-29;google_apis;arm64-v8a"
fi

cd $BUILD_OUT
ln -s $XDG_CACHE_HOME/android .
cp $BUILD_PKG/env.sh .
