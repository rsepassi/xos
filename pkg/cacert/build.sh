fetch "https://curl.se/ca/cacert-2023-12-12.pem" \
  cacert.pem \
  "ccbdfc2fe1a0d7bbbb9cc15710271acf1bb1afe4c8f1725fe95c4c7733fcbe5a"

cd "$BUILD_OUT"
mkdir share
cp "$BUILD_DEPS/cacert.pem" share
