[ "$TARGET_OS" = "windows" ] && { >&2 echo "windows support incomplete"; exit 1; }

need expat

url="https://dbus.freedesktop.org/releases/dbus/dbus-1.14.10.tar.xz"
hash="ba1f21d2bd9d339da2d4aa8780c09df32fea87998b73da24f49ab9df1e36a50f"
file="dbus.tar.gz"
src=$(fetch_untar "$url" "$file" "$hash")
cd $src

cp "$BUILD_PKG/config.h" .
cp "$BUILD_PKG/dbus-arch-deps.h" dbus

unix_files="
dbus/dbus-file-unix.c
dbus/dbus-sysdeps-unix.c
dbus/dbus-sysdeps-util-unix.c
dbus/dbus-server-unix.c
dbus/dbus-userdb.c
dbus/dbus-userdb-util.c
dbus/dbus-uuidgen.c
dbus/dbus-pipe-unix.c
dbus/dbus-spawn-unix.c
dbus/dbus-transport-unix.c
dbus/dbus-sysdeps-pthread.c
"

linux_files="
dbus/dbus-pollable-set-epoll.c
"

windows_files="
dbus/dbus-pipe-win.c
dbus/dbus-file-win.c
dbus/dbus-server-win.c
dbus/dbus-spawn-win.c
dbus/dbus-sysdeps-win.c
dbus/dbus-sysdeps-util-win.c
dbus/dbus-sysdeps-wince-glue.c
dbus/dbus-sysdeps-thread-win.c
dbus/dbus-transport-win.c
"

if [ "$TARGET_OS" = "macos" ]
then
  cflags="
  -DDBUS_BUS_ENABLE_KQUEUE 
  -DDBUS_ENABLE_LAUNCHD 
  -DDBUS_UNIX 
  -DHAVE_CRT_EXTERNS_H 
  -DHAVE_EXECINFO_H 
  -DHAVE_BACKTRACE 
  -DHAVE_POLL
  -DHAVE_UNIX_FD_PASSING 
  "
  echo "$windows_files" | xargs rm
  echo "$linux_files" | xargs rm
  daemon_os_sources="bus/dir-watch-kqueue.c"
elif [ "$TARGET_OS" = "linux" ]
then
  cflags="
  -DDBUS_HAVE_LINUX_EPOLL 
  -DDBUS_UNIX 
  -DHAVE_POLL
  -DHAVE_UNIX_FD_PASSING 
  "
  echo "$windows_files" | xargs rm
  daemon_os_sources="bus/dir-watch-inotify.c"
elif [ "$TARGET_OS" = "windows" ]
then
  cflags="
  -DDBUS_WIN 
  "
  echo "$linux_files" | xargs rm
  echo "$unix_files" | xargs rm
  rm dbus/dbus-sysdeps-wince-glue.c
  daemon_os_sources="bus/dir-watch-default.c"
fi

touch dbus.c
zig build-lib -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DDBUS_STATIC_BUILD -DDBUS_COMPILATION \
  -DDBUS_MACHINE_UUID_FILE="\"\"" \
  -DDBUS_SYSTEM_CONFIG_FILE="\"\"" \
  -DDBUS_SESSION_CONFIG_FILE="\"\"" \
  -DDBUS_RUNSTATEDIR="\"\"" \
  -Ddbus_1_EXPORTS \
  -I. \
  $cflags \
  dbus.c dbus/*.c \
  -lc

daemon_sources="
bus/main.c
bus/activation.c
bus/apparmor.c
bus/audit.c
bus/bus.c
bus/config-loader-expat.c
bus/config-parser.c
bus/config-parser-common.c
bus/connection.c
bus/containers.c
bus/desktop-file.c
bus/dispatch.c
bus/driver.c
bus/expirelist.c
bus/policy.c
bus/selinux.c
bus/services.c
bus/signals.c
bus/stats.c
bus/test.c
bus/utils.c
"
touch dbus-daemon.c
zig build-exe -target $TARGET -O $OPT_ZIG \
  -DHAVE_CONFIG_H -DDBUS_COMPILATION \
  -DDBUS_MACHINE_UUID_FILE="\"\"" \
  -DDBUS_SYSTEM_CONFIG_FILE="\"\"" \
  -DDBUS_SESSION_CONFIG_FILE="\"\"" \
  -DDBUS_RUNSTATEDIR="\"\"" \
  -I. -Ibus \
  $cflags \
  dbus-daemon.c $daemon_sources $daemon_os_sources \
  $(pkg-config --cflags --libs expat) \
  $(zigi lib dbus) \
  -lc

cd "$BUILD_OUT"
mkdir lib include bin
cp "$src/dbus/dbus.h" include
mv "$src/$(zigi lib dbus)" lib
mv "$src/$(zigi exe dbus-daemon)" bin
