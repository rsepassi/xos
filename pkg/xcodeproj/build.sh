appname="${1:-xos-app}"

cd "$BUILD_OUT"
mkdir $appname

cp -r $BUILD_PKG/xos-app $BUILD_OUT/$appname/$appname
cp -r $BUILD_PKG/xos-app.xcodeproj $BUILD_OUT/$appname/$appname.xcodeproj
sed -i "s/xos-app/$appname/g" $BUILD_OUT/$appname/$appname.xcodeproj/project.pbxproj
