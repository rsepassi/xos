url="https://github.com/eradman/entr/archive/refs/tags/5.5.tar.gz"
hash="128c0ce2efea5ae6bd3fd33c3cd31e161eb0c02609d8717ad37e95b41656e526"
file="entr.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

if [ "$TARGET_OS" = "macos" ]
then
  cargs="-D_MACOS_PORT"
elif [ "$TARGET_OS" = "linux" ]
then
  cargs="-D_LINUX_PORT missing/strlcpy.c missing/kqueue_inotify.c -Imissing"
fi

bin=$(zigi exe entr)
cc -target $TARGET -O$OPT -o $bin \
  entr.c \
  -DRELEASE="\"5.5\"" \
  $cargs \
  -lc

mkdir "$BUILD_OUT/bin"
mv $bin "$BUILD_OUT/bin"
