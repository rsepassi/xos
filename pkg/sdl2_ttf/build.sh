fetch_urltxt $BUILD_PKG/url.txt sdl_ttf.tar.gz
src=$(mktemp -d)
untar $BUILD_DEPS/sdl_ttf.tar.gz $src
cd $src

SDL_lib=$BUILD_DEPS/sdl2/lib/libSDL2.a
FT_lib=$BUILD_DEPS/freetype/lib/libfreetype.a
HB_lib=$BUILD_DEPS/harfbuzz/lib/libharfbuzz.a
Brotli_lib=$BUILD_DEPS/brotli/lib/libbrotli.a
if [ "$ARCH_OS" = "macos" ]
then
  need macossdk -- dl
  macsdk="$BUILD_DEPS/macossdk/sdk"

  linkargs="\
  -F $macsdk/System/Library/Frameworks \
  -L $macsdk/usr/lib \
  $macsdk/usr/lib/libcharset.1.tbd \
  -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal \
  -framework CoreData -framework AppKit -framework CoreImage -framework Foundation \
  -framework ApplicationServices -framework CoreFoundation \
  -framework ImageIO -framework CoreText -framework CoreServices -framework CoreGraphics \
  -framework ColorSync -framework CFNetwork \
  -liconv -lobjc"
elif [ "$ARCH_OS" = "windows" ]
then
  SDL_lib=$BUILD_DEPS/sdl2/lib/SDL2.lib
  FT_lib=$BUILD_DEPS/freetype/lib/freetype.lib
  HB_lib=$BUILD_DEPS/harfbuzz/lib/harfbuzz.lib
  Brotli_lib=$BUILD_DEPS/brotli/lib/brotli.lib
  linkargs="-lsetupapi -lwinmm -lgdi32 -limm32 -lversion -loleaut32 -lole32 -lc"
fi

cc --target=$ARCH -c SDL_ttf.c -o SDL_ttf.o -DTTF_USE_HARFBUZZ \
  -I $BUILD_DEPS/sdl2/include/SDL2 \
  -I $BUILD_DEPS/freetype/include \
  -I $BUILD_DEPS/harfbuzz/include/harfbuzz \
  -D_THREAD_SAFE \
  -lc
ar rcs libSDL_ttf.a SDL_ttf.o

cc --target=$ARCH -c showfont.c -o showfont.o \
  -I $BUILD_DEPS/sdl2/include/SDL2 \
  -I $BUILD_DEPS/freetype/include \
  -I $BUILD_DEPS/harfbuzz/include/harfbuzz \
  -D_THREAD_SAFE \
  -lc

# Test link
# zig c++ --target=$ARCH -o showfont showfont.o \
#   libSDL_ttf.a \
#   -I . \
#   $SDL_lib \
#   -I $BUILD_DEPS/sdl2/include/SDL2 \
#   $FT_lib \
#   -I $BUILD_DEPS/freetype/include \
#   $HB_lib \
#   -I $BUILD_DEPS/harfbuzz/include/harfbuzz \
#   $Brotli_lib \
#   $linkargs

mkdir -p $BUILD_OUT/lib $BUILD_OUT/include
cp SDL_ttf.h $BUILD_OUT/include
cp libSDL_ttf.a $BUILD_OUT/lib
