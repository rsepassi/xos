base=$1
if [ "$base" = "alpine" ]
then
  base="alpine:3.19"
else
  >&2 echo "unsupported base $base"
  exit 1
fi

pkgs=""
oldifs=$IFS
IFS=","
for pkg in $2
do
  pkgs="$pkgs $pkg"
done
IFS=$oldifs

ldlibs=""
libs=""
oldifs=$IFS
IFS=","
for lib in $3
do
  libs="$libs $lib"
  ldlibs="$ldlibs -l$lib"
done
IFS=$oldifs

export HOME=$XOS_SYSTEM_HOME

arch=$TARGET_ARCH
if [ "$TARGET_ARCH" = "aarch64" ]
then
  arch="arm64"
fi

id=$(system podman run --arch $arch -d alpine:3.19 sleep 10000000)
system podman exec $id apk add $pkgs
system podman export $id -o export.tar.gz
system podman kill $id

mkdir export
tar xf export.tar.gz -C export
mv export/usr/include "$BUILD_OUT"

mkdir "$BUILD_OUT"/lib
for lib in $libs
do
  cp export/usr/lib/lib$lib.so "$BUILD_OUT"/lib
done

mkdir "$BUILD_OUT/pkgconfig"
cat <<EOF > "$BUILD_OUT/pkgconfig/linuxsdk.pc"
Cflags: -I\${rootdir}/include
Libs: $ldlibs
EOF
