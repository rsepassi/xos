genpkgconfig() {
  mkdir -p "$BUILD_OUT/pkgconfig"
  cat <<EOF > $BUILD_OUT/pkgconfig/iossdk.pc
Cflags: -I\${rootdir}/sdk/usr/include -L\${rootdir}/sdk/usr/lib -F\${rootdir}/sdk/System/Library/Frameworks -DTARGET_OS_IOS=1
EOF
}

genlibctxt() {
	mkdir -p "$BUILD_OUT/share"
	cp "$BUILD_PKG"/libc.txt "$BUILD_OUT/share"
}

# TODO: download
# if [ "$1" != "local" ]
# then
#   url="https://github.com/rsepassi/xos/releases/download/ios-sdk-17.2-v0/iossdk.tar.gz"
#   hash="xx"
#   file="iossdk.tar.gz"
# 
#   fetch "$url" "$file" "$hash"
#   untar "$BUILD_DEPS/$file" "$BUILD_OUT" 0
#   genpkgconfig
# 	genlibctxt
# 
#   exit 0
# fi

if [ "$HOST_OS" != "macos" ]
then
  >&2 echo "iossdk can only be built on a mac"
  exit 1
fi

if [ "$TARGET_ABI" = "simulator" ]
then
  sdk='/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator17.2.sdk'
else
  sdk='/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS17.2.sdk'
fi
sdk_frameworks="$sdk/System/Library/Frameworks"
sdk_includes="$sdk/usr/include"
sdk_libs="$sdk/usr/lib"

cp -RL $sdk_libs .
cp -RL $sdk_includes .

frameworks="
AVFAudio.framework
AVFoundation.framework
AVKit.framework
AVRouting.framework
Accelerate.framework
Accessibility.framework
Accounts.framework
ActivityKit.framework
AddressBook.framework
AddressBookUI.framework
AppClip.framework
AppIntents.framework
AssetsLibrary.framework
AudioToolbox.framework
AudioUnit.framework
AuthenticationServices.framework
BackgroundAssets.framework
BackgroundTasks.framework
CFNetwork.framework
Charts.framework
ClassKit.framework
ColorSync.framework
Combine.framework
Contacts.framework
ContactsUI.framework
CoreAudio.framework
CoreAudioKit.framework
CoreAudioTypes.framework
CoreBluetooth.framework
CoreData.framework
CoreFoundation.framework
CoreGraphics.framework
CoreHaptics.framework
CoreImage.framework
CoreLocation.framework
CoreLocationUI.framework
CoreMIDI.framework
CoreMedia.framework
CoreMotion.framework
CoreNFC.framework
CoreServices.framework
CoreTelephony.framework
CoreTransferable.framework
CoreVideo.framework
CryptoKit.framework
CryptoTokenKit.framework
DataDetection.framework
DeviceActivity.framework
DeviceCheck.framework
DeviceDiscoveryExtension.framework
EventKit.framework
EventKitUI.framework
ExtensionFoundation.framework
ExtensionKit.framework
ExternalAccessory.framework
FileProvider.framework
FileProviderUI.framework
Foundation.framework
GLKit.framework
GSS.framework
GameController.framework
GroupActivities.framework
IOKit.framework
IOSurface.framework
IdentityLookup.framework
IdentityLookupUI.framework
ImageCaptureCore.framework
ImageIO.framework
Intents.framework
IntentsUI.framework
LinkPresentation.framework
LocalAuthentication.framework
LocalAuthenticationEmbeddedUI.framework
ManagedSettings.framework
ManagedSettingsUI.framework
MapKit.framework
Matter.framework
MatterSupport.framework
MediaAccessibility.framework
MediaPlayer.framework
MediaToolbox.framework
MessageUI.framework
Messages.framework
Metal.framework
MetalKit.framework
MetalPerformanceShaders.framework
MetalPerformanceShadersGraph.framework
MobileCoreServices.framework
ModelIO.framework
MultipeerConnectivity.framework
NaturalLanguage.framework
Network.framework
NetworkExtension.framework
NotificationCenter.framework
OSLog.framework
OpenAL.framework
OpenGLES.framework
ProximityReader.framework
PushKit.framework
QuartzCore.framework
QuickLook.framework
QuickLookThumbnailing.framework
Security.framework
Speech.framework
StoreKit.framework
Symbols.framework
SystemConfiguration.framework
TabularData.framework
UIKit.framework
UniformTypeIdentifiers.framework
UserNotifications.framework
UserNotificationsUI.framework
WeatherKit.framework
WidgetKit.framework
WorkoutKit.framework
"

mkdir Frameworks
for framework in $frameworks
do
  cp -RL "$sdk_frameworks/$framework" Frameworks
done

out="$BUILD_OUT/sdk"
mkdir -p "$out/System/Library" "$out/usr"
mv ./Frameworks "$out/System/Library"
mv ./lib "$out/usr"
mv ./include "$out/usr"

cd "$BUILD_OUT"
tar czf iossdk.tar.gz sdk
genpkgconfig
genlibctxt
