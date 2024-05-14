#!/usr/bin/env sh
set -e

self=$(realpath $0)
pkgid=$1
appname=$2

org=$(echo $pkgid | cut -d. -f1)
pkg=$(echo $pkgid | cut -d. -f2)

gettemplatedir() {
  root=$(dirname $(dirname $self))
  echo "$root/share/project"
}

templatedir=$(gettemplatedir)
cp -r $templatedir $appname

cd $appname

sed -i \
  "s/xos_org/$org/g" \
  app/src/main/java/com/xos_org/xos_pkg_name/MainActivity.java
sed -i \
  "s/xos_org/$org/g" \
  app/build.gradle

sed -i \
  "s/xos_pkg_name/$pkg/g" \
  app/src/main/java/com/xos_org/xos_pkg_name/MainActivity.java
sed -i \
  "s/xos_pkg_name/$pkg/g" \
  app/build.gradle
sed -i \
  "s/xos_pkg_name/$pkg/g" \
  app/src/main/cpp/CMakeLists.txt

sed -i \
  "s/xos-app-name/$appname/g" \
  settings.gradle
sed -i \
  "s/xos-app-name/$appname/g" \
  app/src/main/res/values/strings.xml

sed -i \
  "s/Java_com_xos_1org_xos_1pkg_1name_MainActivity_stringFromJNI/Java_com_${org}_${pkg}_MainActivity_stringFromJNI/g" \
  app/src/main/cpp/native-lib.cpp

mv \
  app/src/main/java/com/xos_org/xos_pkg_name \
  app/src/main/java/com/xos_org/$pkg
mv \
  app/src/main/java/com/xos_org \
  app/src/main/java/com/$org
