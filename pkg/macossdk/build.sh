set -e

if [ "$ARCH_HOST_OS" != "macos" ]
then
  >&2 echo "macossdk can only be built on a mac"
  exit 1
fi

# from https://github.com/hexops/xcode-frameworks

sdk='/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.2.sdk'
frameworks="$sdk/System/Library/Frameworks"
includes="$sdk/usr/include"
libs="$sdk/usr/lib"

rm -rf Frameworks/
rm -rf include/
rm -rf lib/

mkdir -p ./Frameworks
cp -RL $includes .
mkdir -p ./lib

# General includes, removing uncommon or useless ones
rm -rf ./include/apache2

# General libraries
mkdir -p lib/
cp $libs/libobjc.tbd ./lib/
cp $libs/libobjc.A.tbd ./lib/

# General frameworks
cp -RL $frameworks/CoreFoundation.framework ./Frameworks/CoreFoundation.framework
cp -RL $frameworks/Foundation.framework ./Frameworks/Foundation.framework
cp -RL $frameworks/IOKit.framework ./Frameworks/IOKit.framework
cp -RL $frameworks/Security.framework ./Frameworks/Security.framework
cp -RL $frameworks/CoreServices.framework ./Frameworks/CoreServices.framework
cp -RL $frameworks/DiskArbitration.framework ./Frameworks/DiskArbitration.framework
cp -RL $frameworks/CFNetwork.framework ./Frameworks/CFNetwork.framework
cp -RL $frameworks/ApplicationServices.framework ./Frameworks/ApplicationServices.framework
cp -RL $frameworks/ImageIO.framework ./Frameworks/ImageIO.framework
cp -RL $frameworks/Symbols.framework ./Frameworks/Symbols.framework

# Audio frameworks
cp -RL $frameworks/AudioToolbox.framework ./Frameworks/AudioToolbox.framework
cp -RL $frameworks/CoreAudio.framework ./Frameworks/CoreAudio.framework
cp -RL $frameworks/CoreAudioTypes.framework ./Frameworks/CoreAudioTypes.framework
cp -RL $frameworks/AudioUnit.framework ./Frameworks/AudioUnit.framework
cp -RL $frameworks/AVFAudio.framework ./Frameworks/AVFAudio.framework

# Graphics frameworks
cp -RL $frameworks/Metal.framework ./Frameworks/Metal.framework
cp -RL $frameworks/OpenGL.framework ./Frameworks/OpenGL.framework
cp -RL $frameworks/CoreGraphics.framework ./Frameworks/CoreGraphics.framework
cp -RL $frameworks/IOSurface.framework ./Frameworks/IOSurface.framework
cp -RL $frameworks/QuartzCore.framework ./Frameworks/QuartzCore.framework
cp -RL $frameworks/CoreImage.framework ./Frameworks/CoreImage.framework
cp -RL $frameworks/CoreVideo.framework ./Frameworks/CoreVideo.framework
cp -RL $frameworks/CoreText.framework ./Frameworks/CoreText.framework
cp -RL $frameworks/ColorSync.framework ./Frameworks/ColorSync.framework

# Input/Windowing frameworks & deps
cp -RL $frameworks/Carbon.framework ./Frameworks/Carbon.framework
cp -RL $frameworks/Cocoa.framework ./Frameworks/Cocoa.framework
cp -RL $frameworks/AppKit.framework ./Frameworks/AppKit.framework
cp -RL $frameworks/CoreData.framework ./Frameworks/CoreData.framework
cp -RL $frameworks/CloudKit.framework ./Frameworks/CloudKit.framework
cp -RL $frameworks/CoreLocation.framework ./Frameworks/CoreLocation.framework
cp -RL $frameworks/Kernel.framework ./Frameworks/Kernel.framework
cp -RL $frameworks/GameController.framework ./Frameworks/GameController.framework

cp -RL $frameworks/AVFoundation.framework ./Frameworks/AVFoundation.framework
cp -RL $frameworks/ForceFeedback.framework ./Frameworks/ForceFeedback.framework
cp -RL $frameworks/CoreMIDI.framework ./Frameworks/CoreMIDI.framework

# Remove unnecessary files
find . | grep '\.swiftmodule' | xargs rm -rf
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/ndrvsupport
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/pwr_mgt
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/scsi
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/firewire
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/storage
rm -rf Frameworks/IOKit.framework/Versions/A/Headers/usb

# Trim large frameworks

# 4.9M -> 1M
cat ./Frameworks/Foundation.framework/Versions/C/Foundation.tbd | grep -v 'libswiftFoundation' > tmp
mv tmp ./Frameworks/Foundation.framework/Versions/C/Foundation.tbd

# 13M -> 368K
find ./Frameworks/Kernel.framework -type f | grep -v IOKit/hidsystem | xargs rm -rf

# 29M -> 28M
find . | grep '\.apinotes' | xargs rm -rf
find . | grep '\.r' | xargs rm -rf
find . | grep '\.modulemap' | xargs rm -rf

# 668K
rm ./Frameworks/OpenGL.framework/Versions/A/Libraries/libLLVMContainer.tbd

# 672K
rm ./Frameworks/OpenGL.framework/Versions/A/Libraries/3425AMD/libLLVMContainer.tbd

# 444K
rm ./Frameworks/CloudKit.framework/Versions/A/CloudKit.tbd

# Now that /Versions/Current symlinks are realized, we no longer need the duplicate
find Frameworks | grep '/Versions/A/' | xargs rm -rf
find Frameworks | grep '/Versions/C/' | xargs rm -rf

mkdir -p $BUILD_OUT/System/Library $BUILD_OUT/usr
mv ./Frameworks $BUILD_OUT/System/Library
mv ./lib $BUILD_OUT/usr
mv ./include $BUILD_OUT/usr
