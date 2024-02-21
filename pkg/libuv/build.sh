url="https://dist.libuv.org/dist/v1.48.0/libuv-v1.48.0.tar.gz"
hash="7f1db8ac368d89d1baf163bac1ea5fe5120697a73910c8ae6b2fffb3551d59fb"
file="libuv.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src

unix_files="
src/unix/async.c
src/unix/core.c
src/unix/dl.c
src/unix/fs.c
src/unix/getaddrinfo.c
src/unix/getnameinfo.c
src/unix/internal.h
src/unix/loop-watcher.c
src/unix/loop.c
src/unix/pipe.c
src/unix/poll.c
src/unix/process.c
src/unix/random-devurandom.c
src/unix/signal.c
src/unix/stream.c
src/unix/tcp.c
src/unix/thread.c
src/unix/tty.c
src/unix/udp.c
"

case "$ARCH_OS" in
  linux)
    files="
      $unix_files
      src/unix/linux.c
      src/unix/procfs-exepath.c
      src/unix/proctitle.c
      src/unix/random-getrandom.c
      src/unix/random-sysctl-linux.c
    "
    flags="-I./src/unix -D_GNU_SOURCE"
    headers="
    include/uv/linux.h
    include/uv/errno.h
    include/uv/unix.h
    "
    ;;
  macos)
    files="
      $unix_files
      src/unix/bsd-ifaddrs.c
      src/unix/darwin-proctitle.c
      src/unix/darwin-stub.h
      src/unix/darwin.c
      src/unix/fsevents.c
      src/unix/kqueue.c
      src/unix/proctitle.c
      src/unix/random-getentropy.c
    "
    flags="-I./src/unix -D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1"
    headers="
    include/uv/darwin.h
    "
    ;;

  windows)
    files="
      src/win/async.c
      src/win/core.c
      src/win/detect-wakeup.c
      src/win/dl.c
      src/win/error.c
      src/win/fs-event.c
      src/win/fs.c
      src/win/getaddrinfo.c
      src/win/getnameinfo.c
      src/win/handle.c
      src/win/loop-watcher.c
      src/win/pipe.c
      src/win/poll.c
      src/win/process-stdio.c
      src/win/process.c
      src/win/signal.c
      src/win/stream.c
      src/win/tcp.c
      src/win/thread.c
      src/win/tty.c
      src/win/udp.c
      src/win/util.c
      src/win/winapi.c
      src/win/winsock.c
    "
    flags="-I./src/win -DWIN32_LEAN_AND_MEAN"
    headers="
    include/uv/win.h
    include/uv/tree.h
    "
    ;;
esac

touch uv.c
zig build-lib -target $ARCH -O $OPT_ZIG \
  -DPACKAGE_NAME="libuv" \
  -DPACKAGE_TARNAME="libuv" \
  -DPACKAGE_VERSION="1.48.0" \
  -DPACKAGE_STRING="libuv 1.48.0" \
  -DPACKAGE_BUGREPORT="https://github.com/libuv/libuv/issues" \
  -DPACKAGE_URL="" \
  -DPACKAGE="libuv" \
  -DVERSION="1.48.0" \
  -DSUPPORT_ATTRIBUTE_VISIBILITY_DEFAULT=1 \
  -DSUPPORT_FLAG_VISIBILITY=1 \
  -DHAVE_STDIO_H=1 \
  -DHAVE_STDLIB_H=1 \
  -DHAVE_STRING_H=1 \
  -DHAVE_INTTYPES_H=1 \
  -DHAVE_STDINT_H=1 \
  -DHAVE_STRINGS_H=1 \
  -DHAVE_SYS_STAT_H=1 \
  -DHAVE_SYS_TYPES_H=1 \
  -DHAVE_UNISTD_H=1 \
  -DSTDC_HEADERS=1 \
  -DHAVE_DLFCN_H=1 \
  -DHAVE_PTHREAD_PRIO_INHERIT=1 \
  $flags \
  -I./include -I./src \
  -lc \
  -cflags -std=gnu89 -- \
  uv.c src/*.c $files

mkdir "$BUILD_OUT/lib" "$BUILD_OUT/include"
mv $(zigi lib uv) "$BUILD_OUT/lib"
cp \
  include/uv.h \
  include/uv/version.h \
  include/uv/threadpool.h \
  include/uv/errno.h \
  $headers \
  "$BUILD_OUT/include"
