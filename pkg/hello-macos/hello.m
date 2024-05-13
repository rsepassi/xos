#import <Cocoa/Cocoa.h>

@interface MyCustomView : NSView
@end

@implementation MyCustomView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];

    // Drawing code here.
    NSString *text = @"Hello, World!";

    NSFont *font = [NSFont systemFontOfSize:24];
    NSDictionary *attributes = @{
        NSFontAttributeName: font,
        NSForegroundColorAttributeName: [NSColor whiteColor]
    };

    NSAttributedString *attrString = [[NSAttributedString alloc] initWithString:text attributes:attributes];
    NSSize textSize = attrString.size;

    NSRect textRect = NSMakeRect((NSWidth(self.bounds) - textSize.width)/2,
                                 (NSHeight(self.bounds) - textSize.height)/2,
                                 textSize.width,
                                 textSize.height);

    [attrString drawInRect:textRect];
}

@end

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
		[self.window setBackgroundColor:[NSColor blueColor]];

		MyCustomView *customView = [[MyCustomView alloc] initWithFrame:[self.window contentRectForFrameRect:self.window.frame]];
		[self.window setContentView:customView];

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
