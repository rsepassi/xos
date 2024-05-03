fetch "https://www.eff.org/files/2016/07/18/eff_large_wordlist.txt" \
  "large.txt" \
  "addd35536511597a02fa0a9ff1e5284677b8883b83e986e43f15a3db996b903e"
fetch "https://www.eff.org/files/2016/09/08/eff_short_wordlist_1.txt" \
  "short1.txt" \
  "8f5ca830b8bffb6fe39c9736c024a00a6a6411adb3f83a9be8bfeeb6e067ae69"
fetch "https://www.eff.org/files/2016/09/08/eff_short_wordlist_2_0.txt" \
  "short2.txt" \
  "22b45c52e0bd0bbf03aa522240b111eb4c7c0c1d86c4e518e1be2a7eb2a625e4"

cd "$BUILD_OUT"
mkdir share
ln -s "$BUILD_DEPS/large.txt" share
ln -s "$BUILD_DEPS/short1.txt" share
ln -s "$BUILD_DEPS/short2.txt" share
