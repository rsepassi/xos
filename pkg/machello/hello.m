#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Create the window
    self.window = [
      [NSWindow alloc]
      initWithContentRect:NSMakeRect(0, 0, 640, 480)
                styleMask:NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask
                  backing:NSBackingStoreBuffered
                    defer:NO
    ];
    [self.window setTitle:@"Hello World!"];
    [self.window makeKeyAndOrderFront:nil];

    // Create the web view
    WKWebView *webView = [[WKWebView alloc] initWithFrame:[
      [self.window contentView] bounds]
    ];
    [self.window setContentView:webView];

    // Load HTML string into the web view
    NSString *htmlString = @"<html><body><h1>Hello, world!</h1></body></html>";
    [webView loadHTMLString:htmlString baseURL:nil];

    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

int main(int argc, const char* argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *appDelegate = [[AppDelegate alloc] init];
        app.delegate = appDelegate;
        [app run];
    }
    return 0;
}
