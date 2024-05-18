genpkgconfig() {
  mkdir -p "$BUILD_OUT/pkgconfig"
  cat <<EOF > $BUILD_OUT/pkgconfig/macossdk.pc
Cflags: -I\${rootdir}/sdk/usr/include -F\${rootdir}/sdk/System/Library/Frameworks -DTARGET_OS_OSX=1
Libs: -L\${rootdir}/sdk/usr/lib -F\${rootdir}/sdk/System/Library/Frameworks
EOF
}

if [ "$1" != "local" ]
then
  url="https://github.com/rsepassi/xos/releases/download/macos-sdk-14.2-v5/macossdk.tar.gz"
  hash="61121dae9a1a7afd3199e43860995093ea40cd0110a4728b2a9546e1c784e99f"
  file="macossdk.tar.gz"

  fetch "$url" "$file" "$hash"
  untar "$BUILD_DEPS/$file" "$BUILD_OUT" 0
  genpkgconfig

  exit 0
fi

if [ "$HOST_OS" != "macos" ]
then
  >&2 echo "macossdk can only be built on a mac"
  exit 1
fi

sdk='/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.2.sdk'
sdk_frameworks="$sdk/System/Library/Frameworks"
sdk_includes="$sdk/usr/include"
sdk_libs="$sdk/usr/lib"

cp -RL $sdk_libs .
cp -RL $sdk_includes .

frameworks="
AGL.framework
AVFAudio.framework
AVFoundation.framework
AVKit.framework
AVRouting.framework
Accelerate.framework
Accessibility.framework
Accounts.framework
AdServices.framework
AdSupport.framework
AddressBook.framework
AppIntents.framework
AppKit.framework
AppTrackingTransparency.framework
AppleScriptKit.framework
AppleScriptObjC.framework
ApplicationServices.framework
AudioToolbox.framework
AudioUnit.framework
AudioVideoBridging.framework
AuthenticationServices.framework
AutomaticAssessmentConfiguration.framework
Automator.framework
BackgroundAssets.framework
BackgroundTasks.framework
BusinessChat.framework
CFNetwork.framework
CalendarStore.framework
CallKit.framework
CarKey.framework
Carbon.framework
Charts.framework
Cinematic.framework
ClassKit.framework
CloudKit.framework
Cocoa.framework
Collaboration.framework
ColorSync.framework
Combine.framework
Contacts.framework
ContactsUI.framework
CoreAudio.framework
CoreAudioKit.framework
CoreAudioTypes.framework
CoreBluetooth.framework
CoreData.framework
CoreDisplay.framework
CoreFoundation.framework
CoreGraphics.framework
CoreHaptics.framework
CoreImage.framework
CoreLocation.framework
CoreMIDI.framework
CoreMIDIServer.framework
CoreML.framework
CoreMedia.framework
CoreMediaIO.framework
CoreMotion.framework
CoreServices.framework
CoreSpotlight.framework
CoreTelephony.framework
CoreText.framework
CoreTransferable.framework
CoreVideo.framework
CoreWLAN.framework
CreateML.framework
CreateMLComponents.framework
CryptoKit.framework
CryptoTokenKit.framework
DVDPlayback.framework
DataDetection.framework
DeveloperToolsSupport.framework
DeviceActivity.framework
DeviceCheck.framework
DirectoryService.framework
DiscRecording.framework
DiscRecordingUI.framework
DiskArbitration.framework
DockKit.framework
DriverKit.framework
EventKit.framework
ExceptionHandling.framework
ExecutionPolicy.framework
ExtensionFoundation.framework
ExtensionKit.framework
ExternalAccessory.framework
FamilyControls.framework
FileProvider.framework
FileProviderUI.framework
FinanceKit.framework
FinanceKitUI.framework
FinderSync.framework
ForceFeedback.framework
Foundation.framework
GLKit.framework
GLUT.framework
GSS.framework
GameController.framework
GameKit.framework
GameplayKit.framework
GroupActivities.framework
HealthKit.framework
Hypervisor.framework
ICADevices.framework
IOBluetooth.framework
IOBluetoothUI.framework
IOKit.framework
IOSurface.framework
IOUSBHost.framework
IdentityLookup.framework
ImageCaptureCore.framework
ImageIO.framework
InputMethodKit.framework
InstallerPlugins.framework
InstantMessage.framework
Intents.framework
IntentsUI.framework
JavaNativeFoundation.framework
JavaRuntimeSupport.framework
JavaScriptCore.framework
Kerberos.framework
Kernel.framework
KernelManagement.framework
LDAP.framework
LatentSemanticMapping.framework
LinkPresentation.framework
LocalAuthentication.framework
LocalAuthenticationEmbeddedUI.framework
MLCompute.framework
MailKit.framework
ManagedAppDistribution.framework
ManagedSettings.framework
MapKit.framework
Matter.framework
MatterSupport.framework
MediaAccessibility.framework
MediaLibrary.framework
MediaPlayer.framework
MediaToolbox.framework
Message.framework
Metal.framework
MetalFX.framework
MetalKit.framework
MetalPerformanceShaders.framework
MetalPerformanceShadersGraph.framework
MetricKit.framework
ModelIO.framework
MultipeerConnectivity.framework
MusicKit.framework
NaturalLanguage.framework
NearbyInteraction.framework
NetFS.framework
Network.framework
NetworkExtension.framework
NotificationCenter.framework
OSAKit.framework
OSLog.framework
OpenAL.framework
OpenCL.framework
OpenDirectory.framework
OpenGL.framework
PCSC.framework
PDFKit.framework
PHASE.framework
ParavirtualizedGraphics.framework
PassKit.framework
PencilKit.framework
Photos.framework
PhotosUI.framework
PreferencePanes.framework
PushKit.framework
PushToTalk.framework
QTKit.framework
Quartz.framework
QuartzCore.framework
QuickLook.framework
QuickLookThumbnailing.framework
QuickLookUI.framework
RealityFoundation.framework
RealityKit.framework
ReplayKit.framework
SafariServices.framework
SafetyKit.framework
SceneKit.framework
ScreenCaptureKit.framework
ScreenSaver.framework
ScreenTime.framework
ScriptingBridge.framework
Security.framework
SecurityFoundation.framework
SecurityInterface.framework
SensitiveContentAnalysis.framework
SensorKit.framework
ServiceManagement.framework
SharedWithYou.framework
SharedWithYouCore.framework
ShazamKit.framework
Social.framework
SoundAnalysis.framework
Speech.framework
SpriteKit.framework
StoreKit.framework
SwiftData.framework
SwiftUI.framework
Symbols.framework
SyncServices.framework
System.framework
SystemConfiguration.framework
SystemExtensions.framework
TWAIN.framework
TabularData.framework
Tcl.framework
ThreadNetwork.framework
TipKit.framework
Tk.framework
UniformTypeIdentifiers.framework
UserNotifications.framework
UserNotificationsUI.framework
VideoDecodeAcceleration.framework
VideoSubscriberAccount.framework
VideoToolbox.framework
Virtualization.framework
Vision.framework
VisionKit.framework
WeatherKit.framework
WebKit.framework
WidgetKit.framework
_AVKit_SwiftUI.framework
_AppIntents_AppKit.framework
_AppIntents_SwiftUI.framework
_AuthenticationServices_SwiftUI.framework
_CoreData_CloudKit.framework
_DeviceActivity_SwiftUI.framework
_GroupActivities_AppKit.framework
_LocalAuthentication_SwiftUI.framework
_ManagedAppDistribution_SwiftUI.framework
_MapKit_SwiftUI.framework
_MusicKit_SwiftUI.framework
_PassKit_SwiftUI.framework
_PhotosUI_SwiftUI.framework
_QuickLook_SwiftUI.framework
_SceneKit_SwiftUI.framework
_SpriteKit_SwiftUI.framework
_StoreKit_SwiftUI.framework
_SwiftData_CoreData.framework
_SwiftData_SwiftUI.framework
iTunesLibrary.framework
vecLib.framework
vmnet.framework
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
tar czf macossdk.tar.gz sdk
genpkgconfig
