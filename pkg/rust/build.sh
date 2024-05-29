rustversion="1.76.0"
rustupversion="1.26.0"

if [ "$TARGET_OS" = "macos" ] && [ "$TARGET_ARCH" = "aarch64" ]
then
  triple="aarch64-apple-darwin"
  hash="ed299a8fe762dc28161a99a03cf62836977524ad557ad70e13882d2f375d3983"
elif [ "$TARGET_OS" = "macos" ] && [ "$TARGET_ARCH" = "x86_64" ]
then
  triple="x86_64-apple-darwin"
elif [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ARCH" = "aarch64" ]
then
  triple="aarch64-unknown-linux-musl"
elif [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ARCH" = "x86_64" ]
then
  triple="x86_64-unknown-linux-musl"
  hash="7aa9e2a380a9958fc1fc426a3323209b2c86181c6816640979580f62ff7d48d4"
elif [ "$TARGET_OS" = "windows" ] && [ "$TARGET_ARCH" = "x86_64" ]
then
  triple="x86_64-pc-windows-gnu"
else
  >&2 echo "unsupported arch for rust"
  exit 1
fi


file=$(zigi exe rustup-init)
url="https://static.rust-lang.org/rustup/archive/$rustupversion/$triple/$file"

fetch $url $file $hash
rustupinit="$BUILD_DEPS/$file"

targets="
x86_64-unknown-linux-musl
aarch64-unknown-linux-musl
aarch64-apple-darwin
x86_64-apple-darwin
aarch64-apple-ios-sim
aarch64-apple-ios
aarch64-linux-android
x86_64-pc-windows-gnu
"

chmod +x "$rustupinit"
mkdir -p "$XDG_CACHE_HOME/rust"
2>rustupinit.log HOME="$XDG_CACHE_HOME/rust" "$rustupinit" \
  -y -q \
  --default-host $triple \
  --default-toolchain "$rustversion" \
  --no-modify-path \
  --profile minimal

PATH="$XDG_CACHE_HOME/rust/.cargo/bin:$PATH"
for target in $targets
do
  2>>rustuptarget.log HOME="$XDG_CACHE_HOME/rust" rustup target add $target
done


ln -s "$XDG_CACHE_HOME/rust/.cargo" "$BUILD_OUT/.cargo"
ln -s "$XDG_CACHE_HOME/rust/.rustup" "$BUILD_OUT/.rustup"
cp "$BUILD_PKG/rustcc" "$BUILD_OUT/.cargo/bin"
cp "$BUILD_PKG/rusti" "$BUILD_OUT/.cargo/bin"
cp "$BUILD_PKG/cc" "$BUILD_OUT/.cargo/bin"
cp "$BUILD_PKG/cargo" "$BUILD_OUT/.cargo/bin/xcargo"
