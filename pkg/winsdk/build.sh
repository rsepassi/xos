url="https://www.nuget.org/api/v2/package/Microsoft.Windows.SDK.CPP/10.0.22621.3233"
hash="e4efe1768ea61f4f999dbef61b09895320629f975f9ceed8290a9633e0c31623"
file="cpp.zip"
fetch "$url" "$file" "$hash"

url="https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.2365.46"
hash="1b0146d842344b8462520fb38ad8eff46e52031c19b955376c227bec619faefd"
file="webview.zip"
fetch "$url" "$file" "$hash"

cd "$BUILD_OUT"
mkdir sdk-cpp
cd sdk-cpp
unzip "$BUILD_DEPS/cpp.zip"

cd "$BUILD_OUT"
mkdir WebView2
cd WebView2
unzip "$BUILD_DEPS/webview.zip"
