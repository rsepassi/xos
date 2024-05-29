need glfw
need wgpu
need wgpu_glfw_glue

# Note: has to be dynamically linked to libc for dlopen to work

zig build-exe -dynamic -target $TARGET -O $OPT_ZIG \
  --name app \
  $(pkg-config --cflags --libs glfw wgpu wgpu_glfw_glue) \
  $BUILD_PKG/main.zig \
  -lc

cd $BUILD_OUT
mkdir bin
mv $HOME/$(zigi exe app) bin
