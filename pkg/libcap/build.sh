[ "$TARGET_OS" = "linux" ] || { >&2 echo "libcap only available on Linux"; exit 1; }

url="https://git.kernel.org/pub/scm/libs/libcap/libcap.git/snapshot/libcap-2.69.tar.gz"
hash="9cc2fa3ee744d881629cdac1a1b54c302e7684fda3e3622056218c7681642ffb"
file="libcap.tar.gz"

fetch "$url" "$file" "$hash"
src=$(mktemp -d)
untar "$BUILD_DEPS/$file" "$src"
cd $src/libcap

grep -E '^#define\s+CAP_([^\s]+)\s+[0-9]+\s*$' include/uapi/linux/capability.h | \
  sed -e 's/^#define\s\+/{"/' -e 's/\s*$/},/' -e 's/\s\+/",/' -e 'y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghipqrstuvwxyz/' \
  > cap_names.list.h

zig cc -o _makenames _makenames.c
./_makenames > cap_names.h

rm cap_test.c
touch cap.c
zig build-lib -target $TARGET -O $OPT_ZIG -Iinclude cap.c cap_*.c -lc

mkdir "$BUILD_OUT/lib"
mv libcap.a "$BUILD_OUT/lib"
cp -r include "$BUILD_OUT"
mv libcap.h "$BUILD_OUT/include"
