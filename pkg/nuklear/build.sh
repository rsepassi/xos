url="https://raw.githubusercontent.com/Immediate-Mode-UI/Nuklear/8e5c9f7/nuklear.h"
hash="6e2341fbdc9aff0d99e9938bcdba8bb8a8b3c51207846d20d6a2668d5d718a5d"
file="nuklear.h"
fetch "$url" "$file" "$hash"

cd "$BUILD_OUT"
mkdir include
cp "$BUILD_DEPS/nuklear.h" include

