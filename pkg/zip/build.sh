src=$(fetch_untar "http://downloads.sourceforge.net/infozip/zip30.tar.gz" \
  "zip.tar.gz" \
  "f0e8bb1f9b7eb0b01285495a2699df3a4b766784c1765a8f1aeedf63c0806369")
cd $src

files="
crypt.c
ttyio.c
zipfile.c
zipup.c
fileio.c
util.c
crc32.c
globals.c
deflate.c
trees.c
"
unix_files="
zbz2err.c
unix/unix.c
"
win_files="
win32/win32.c
win32/win32zip.c
win32/win32i64.c
win32/nt.c
"

if [ "$TARGET_OS" = "windows" ]
then
  >&2 echo "windows build unimplemented"
  exit 1
# zig build-exe -target $TARGET -O $OPT_ZIG \ #   -DWIN32 -DFORCE_WIN32_OVER_UNIX \
#   zip.c $files $win_files \
#   -I. -luser32 -ladvapi32 -lc
else
zig build-exe -target $TARGET -O $OPT_ZIG \
  zip.c $files $unix_files -DUNIX \
  -I. -lc
fi

mkdir "$BUILD_OUT/bin"
mv $(zigi exe zip) "$BUILD_OUT/bin"
