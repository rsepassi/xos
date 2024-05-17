src=$(fetch_untar "https://github.com/google/highway/archive/refs/tags/1.1.0.tar.gz" \
  hwy.tar.gz \
  "354a8b4539b588e70b98ec70844273e3f2741302c4c377bcc4e81b3d1866f7c9")
cd $src

rm -rf hwy/tests
find . -type f -name '*_test.cc' | xargs rm
rm hwy/contrib/sort/bench_parallel.cc
rm hwy/contrib/sort/bench_sort.cc

hdrs=$(find . -type f -name '*.h')
srcs=$(find . -type f -name '*.cc')

zig build-lib -target $TARGET -O $OPT_ZIG \
  --name hwy \
  -I. \
  $srcs \
  -lc++

mkdir $BUILD_OUT/include
for hdr in $hdrs
do
  cp --parents $hdr $BUILD_OUT/include
done

mkdir $BUILD_OUT/lib
mv $(zigi lib hwy) $BUILD_OUT/lib
