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
  app/build.gradle

sed -i \
  "s/xos_pkg_name/$pkg/g" \
  app/build.gradle

sed -i \
  "s/xos-app-name/$appname/g" \
  settings.gradle
sed -i \
  "s/xos-app-name/$appname/g" \
  app/src/main/res/values/strings.xml
