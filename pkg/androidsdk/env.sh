export JAVA_HOME=$(/usr/libexec/java_home)
export ANDROID_HOME=$XDG_CACHE_HOME/android
export ANDROID_SDK_ROOT=$ANDROID_HOME
export ANDROID_AVD_HOME=$ANDROID_HOME/avd
export REPO_OS_OVERRIDE=macosx
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/emulator:$ANDROID_HOME/platform-tools:$ANDROID_HOME/ndk-bundle:$ANDROID_HOME/build-tools/33.0.1:$JAVA_HOME/bin:$PATH"
export GRADLE_USER_HOME=$ANDROID_HOME/gradle
