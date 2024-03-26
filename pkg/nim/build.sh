if [ "$TARGET_OS" = "linux" ] && [ "$TARGET_ARCH" = "x86_64" ]
then
  src=$(fetch_untar \
    "https://nim-lang.org/download/nim-2.0.2-linux_x64.tar.xz" \
    "nim.tar.xz" \
    "047dde8ff40b18628ac1188baa9ca992d05f1f45c5121d1d07a76224f06e1551")
else
  >&2 echo "unsupported os+arch for nim"
fi

cd "$BUILD_OUT"
mkdir bin
ln -s $src/bin/nim bin
cp "$BUILD_PKG"/nimc bin
cp "$BUILD_PKG"/nimcc bin
cp "$BUILD_PKG"/nimi bin
ln -s $(which ar) bin/llvm-ar

