// ███    ███  █████   ██████  ██████  ███████
// ████  ████ ██   ██ ██      ██    ██ ██
// ██ ████ ██ ███████ ██      ██    ██ ███████
// ██  ██  ██ ██   ██ ██      ██    ██      ██
// ██      ██ ██   ██  ██████  ██████  ███████
//
// >>macos

_SOKOL_PRIVATE void _sapp_macos_init_keytable(void) {
    _sapp.keycodes[0x1D] = SAPP_KEYCODE_0;
    _sapp.keycodes[0x12] = SAPP_KEYCODE_1;
    _sapp.keycodes[0x13] = SAPP_KEYCODE_2;
    _sapp.keycodes[0x14] = SAPP_KEYCODE_3;
    _sapp.keycodes[0x15] = SAPP_KEYCODE_4;
    _sapp.keycodes[0x17] = SAPP_KEYCODE_5;
    _sapp.keycodes[0x16] = SAPP_KEYCODE_6;
    _sapp.keycodes[0x1A] = SAPP_KEYCODE_7;
    _sapp.keycodes[0x1C] = SAPP_KEYCODE_8;
    _sapp.keycodes[0x19] = SAPP_KEYCODE_9;
    _sapp.keycodes[0x00] = SAPP_KEYCODE_A;
    _sapp.keycodes[0x0B] = SAPP_KEYCODE_B;
    _sapp.keycodes[0x08] = SAPP_KEYCODE_C;
    _sapp.keycodes[0x02] = SAPP_KEYCODE_D;
    _sapp.keycodes[0x0E] = SAPP_KEYCODE_E;
    _sapp.keycodes[0x03] = SAPP_KEYCODE_F;
    _sapp.keycodes[0x05] = SAPP_KEYCODE_G;
    _sapp.keycodes[0x04] = SAPP_KEYCODE_H;
    _sapp.keycodes[0x22] = SAPP_KEYCODE_I;
    _sapp.keycodes[0x26] = SAPP_KEYCODE_J;
    _sapp.keycodes[0x28] = SAPP_KEYCODE_K;
    _sapp.keycodes[0x25] = SAPP_KEYCODE_L;
    _sapp.keycodes[0x2E] = SAPP_KEYCODE_M;
    _sapp.keycodes[0x2D] = SAPP_KEYCODE_N;
    _sapp.keycodes[0x1F] = SAPP_KEYCODE_O;
    _sapp.keycodes[0x23] = SAPP_KEYCODE_P;
    _sapp.keycodes[0x0C] = SAPP_KEYCODE_Q;
    _sapp.keycodes[0x0F] = SAPP_KEYCODE_R;
    _sapp.keycodes[0x01] = SAPP_KEYCODE_S;
    _sapp.keycodes[0x11] = SAPP_KEYCODE_T;
    _sapp.keycodes[0x20] = SAPP_KEYCODE_U;
    _sapp.keycodes[0x09] = SAPP_KEYCODE_V;
    _sapp.keycodes[0x0D] = SAPP_KEYCODE_W;
    _sapp.keycodes[0x07] = SAPP_KEYCODE_X;
    _sapp.keycodes[0x10] = SAPP_KEYCODE_Y;
    _sapp.keycodes[0x06] = SAPP_KEYCODE_Z;
    _sapp.keycodes[0x27] = SAPP_KEYCODE_APOSTROPHE;
    _sapp.keycodes[0x2A] = SAPP_KEYCODE_BACKSLASH;
    _sapp.keycodes[0x2B] = SAPP_KEYCODE_COMMA;
    _sapp.keycodes[0x18] = SAPP_KEYCODE_EQUAL;
    _sapp.keycodes[0x32] = SAPP_KEYCODE_GRAVE_ACCENT;
    _sapp.keycodes[0x21] = SAPP_KEYCODE_LEFT_BRACKET;
    _sapp.keycodes[0x1B] = SAPP_KEYCODE_MINUS;
    _sapp.keycodes[0x2F] = SAPP_KEYCODE_PERIOD;
    _sapp.keycodes[0x1E] = SAPP_KEYCODE_RIGHT_BRACKET;
    _sapp.keycodes[0x29] = SAPP_KEYCODE_SEMICOLON;
    _sapp.keycodes[0x2C] = SAPP_KEYCODE_SLASH;
    _sapp.keycodes[0x0A] = SAPP_KEYCODE_WORLD_1;
    _sapp.keycodes[0x33] = SAPP_KEYCODE_BACKSPACE;
    _sapp.keycodes[0x39] = SAPP_KEYCODE_CAPS_LOCK;
    _sapp.keycodes[0x75] = SAPP_KEYCODE_DELETE;
    _sapp.keycodes[0x7D] = SAPP_KEYCODE_DOWN;
    _sapp.keycodes[0x77] = SAPP_KEYCODE_END;
    _sapp.keycodes[0x24] = SAPP_KEYCODE_ENTER;
    _sapp.keycodes[0x35] = SAPP_KEYCODE_ESCAPE;
    _sapp.keycodes[0x7A] = SAPP_KEYCODE_F1;
    _sapp.keycodes[0x78] = SAPP_KEYCODE_F2;
    _sapp.keycodes[0x63] = SAPP_KEYCODE_F3;
    _sapp.keycodes[0x76] = SAPP_KEYCODE_F4;
    _sapp.keycodes[0x60] = SAPP_KEYCODE_F5;
    _sapp.keycodes[0x61] = SAPP_KEYCODE_F6;
    _sapp.keycodes[0x62] = SAPP_KEYCODE_F7;
    _sapp.keycodes[0x64] = SAPP_KEYCODE_F8;
    _sapp.keycodes[0x65] = SAPP_KEYCODE_F9;
    _sapp.keycodes[0x6D] = SAPP_KEYCODE_F10;
    _sapp.keycodes[0x67] = SAPP_KEYCODE_F11;
    _sapp.keycodes[0x6F] = SAPP_KEYCODE_F12;
    _sapp.keycodes[0x69] = SAPP_KEYCODE_F13;
    _sapp.keycodes[0x6B] = SAPP_KEYCODE_F14;
    _sapp.keycodes[0x71] = SAPP_KEYCODE_F15;
    _sapp.keycodes[0x6A] = SAPP_KEYCODE_F16;
    _sapp.keycodes[0x40] = SAPP_KEYCODE_F17;
    _sapp.keycodes[0x4F] = SAPP_KEYCODE_F18;
    _sapp.keycodes[0x50] = SAPP_KEYCODE_F19;
    _sapp.keycodes[0x5A] = SAPP_KEYCODE_F20;
    _sapp.keycodes[0x73] = SAPP_KEYCODE_HOME;
    _sapp.keycodes[0x72] = SAPP_KEYCODE_INSERT;
    _sapp.keycodes[0x7B] = SAPP_KEYCODE_LEFT;
    _sapp.keycodes[0x3A] = SAPP_KEYCODE_LEFT_ALT;
    _sapp.keycodes[0x3B] = SAPP_KEYCODE_LEFT_CONTROL;
    _sapp.keycodes[0x38] = SAPP_KEYCODE_LEFT_SHIFT;
    _sapp.keycodes[0x37] = SAPP_KEYCODE_LEFT_SUPER;
    _sapp.keycodes[0x6E] = SAPP_KEYCODE_MENU;
    _sapp.keycodes[0x47] = SAPP_KEYCODE_NUM_LOCK;
    _sapp.keycodes[0x79] = SAPP_KEYCODE_PAGE_DOWN;
    _sapp.keycodes[0x74] = SAPP_KEYCODE_PAGE_UP;
    _sapp.keycodes[0x7C] = SAPP_KEYCODE_RIGHT;
    _sapp.keycodes[0x3D] = SAPP_KEYCODE_RIGHT_ALT;
    _sapp.keycodes[0x3E] = SAPP_KEYCODE_RIGHT_CONTROL;
    _sapp.keycodes[0x3C] = SAPP_KEYCODE_RIGHT_SHIFT;
    _sapp.keycodes[0x36] = SAPP_KEYCODE_RIGHT_SUPER;
    _sapp.keycodes[0x31] = SAPP_KEYCODE_SPACE;
    _sapp.keycodes[0x30] = SAPP_KEYCODE_TAB;
    _sapp.keycodes[0x7E] = SAPP_KEYCODE_UP;
    _sapp.keycodes[0x52] = SAPP_KEYCODE_KP_0;
    _sapp.keycodes[0x53] = SAPP_KEYCODE_KP_1;
    _sapp.keycodes[0x54] = SAPP_KEYCODE_KP_2;
    _sapp.keycodes[0x55] = SAPP_KEYCODE_KP_3;
    _sapp.keycodes[0x56] = SAPP_KEYCODE_KP_4;
    _sapp.keycodes[0x57] = SAPP_KEYCODE_KP_5;
    _sapp.keycodes[0x58] = SAPP_KEYCODE_KP_6;
    _sapp.keycodes[0x59] = SAPP_KEYCODE_KP_7;
    _sapp.keycodes[0x5B] = SAPP_KEYCODE_KP_8;
    _sapp.keycodes[0x5C] = SAPP_KEYCODE_KP_9;
    _sapp.keycodes[0x45] = SAPP_KEYCODE_KP_ADD;
    _sapp.keycodes[0x41] = SAPP_KEYCODE_KP_DECIMAL;
    _sapp.keycodes[0x4B] = SAPP_KEYCODE_KP_DIVIDE;
    _sapp.keycodes[0x4C] = SAPP_KEYCODE_KP_ENTER;
    _sapp.keycodes[0x51] = SAPP_KEYCODE_KP_EQUAL;
    _sapp.keycodes[0x43] = SAPP_KEYCODE_KP_MULTIPLY;
    _sapp.keycodes[0x4E] = SAPP_KEYCODE_KP_SUBTRACT;
}

_SOKOL_PRIVATE void _sapp_macos_discard_state(void) {
    // NOTE: it's safe to call [release] on a nil object
    if (_sapp.macos.keyup_monitor != nil) {
        [NSEvent removeMonitor:_sapp.macos.keyup_monitor];
        // NOTE: removeMonitor also releases the object
        _sapp.macos.keyup_monitor = nil;
    }
    _SAPP_OBJC_RELEASE(_sapp.macos.tracking_area);
    _SAPP_OBJC_RELEASE(_sapp.macos.app_dlg);
    _SAPP_OBJC_RELEASE(_sapp.macos.win_dlg);
    _SAPP_OBJC_RELEASE(_sapp.macos.view);
    #if defined(SOKOL_METAL)
        _SAPP_OBJC_RELEASE(_sapp.macos.mtl_device);
    #endif
    _SAPP_OBJC_RELEASE(_sapp.macos.window);
}

// undocumented methods for creating cursors (see GLFW 3.4 and imgui_impl_osx.mm)
@interface NSCursor()
+ (id)_windowResizeNorthWestSouthEastCursor;
+ (id)_windowResizeNorthEastSouthWestCursor;
+ (id)_windowResizeNorthSouthCursor;
+ (id)_windowResizeEastWestCursor;
@end

_SOKOL_PRIVATE void _sapp_macos_init_cursors(void) {
    _sapp.macos.cursors[SAPP_MOUSECURSOR_DEFAULT] = nil; // not a bug
    _sapp.macos.cursors[SAPP_MOUSECURSOR_ARROW] = [NSCursor arrowCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_IBEAM] = [NSCursor IBeamCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_CROSSHAIR] = [NSCursor crosshairCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_POINTING_HAND] = [NSCursor pointingHandCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_RESIZE_EW] = [NSCursor respondsToSelector:@selector(_windowResizeEastWestCursor)] ? [NSCursor _windowResizeEastWestCursor] : [NSCursor resizeLeftRightCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_RESIZE_NS] = [NSCursor respondsToSelector:@selector(_windowResizeNorthSouthCursor)] ? [NSCursor _windowResizeNorthSouthCursor] : [NSCursor resizeUpDownCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_RESIZE_NWSE] = [NSCursor respondsToSelector:@selector(_windowResizeNorthWestSouthEastCursor)] ? [NSCursor _windowResizeNorthWestSouthEastCursor] : [NSCursor closedHandCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_RESIZE_NESW] = [NSCursor respondsToSelector:@selector(_windowResizeNorthEastSouthWestCursor)] ? [NSCursor _windowResizeNorthEastSouthWestCursor] : [NSCursor closedHandCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_RESIZE_ALL] = [NSCursor closedHandCursor];
    _sapp.macos.cursors[SAPP_MOUSECURSOR_NOT_ALLOWED] = [NSCursor operationNotAllowedCursor];
}

_SOKOL_PRIVATE void _sapp_macos_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    _sapp_macos_init_keytable();
    [NSApplication sharedApplication];

    // set the application dock icon as early as possible, otherwise
    // the dummy icon will be visible for a short time
    sapp_set_icon(&_sapp.desc.icon);
    _sapp.macos.app_dlg = [[_sapp_macos_app_delegate alloc] init];
    NSApp.delegate = _sapp.macos.app_dlg;

    // workaround for "no key-up sent while Cmd is pressed" taken from GLFW:
    NSEvent* (^keyup_monitor)(NSEvent*) = ^NSEvent* (NSEvent* event) {
        if ([event modifierFlags] & NSEventModifierFlagCommand) {
            [[NSApp keyWindow] sendEvent:event];
        }
        return event;
    };
    _sapp.macos.keyup_monitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyUp handler:keyup_monitor];

    [NSApp run];
    // NOTE: [NSApp run] never returns, instead cleanup code
    // must be put into applicationWillTerminate
}

/* MacOS entry function */
#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_macos_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */

_SOKOL_PRIVATE uint32_t _sapp_macos_mods(NSEvent* ev) {
    const NSEventModifierFlags f = (ev == nil) ? NSEvent.modifierFlags : ev.modifierFlags;
    const NSUInteger b = NSEvent.pressedMouseButtons;
    uint32_t m = 0;
    if (f & NSEventModifierFlagShift) {
        m |= SAPP_MODIFIER_SHIFT;
    }
    if (f & NSEventModifierFlagControl) {
        m |= SAPP_MODIFIER_CTRL;
    }
    if (f & NSEventModifierFlagOption) {
        m |= SAPP_MODIFIER_ALT;
    }
    if (f & NSEventModifierFlagCommand) {
        m |= SAPP_MODIFIER_SUPER;
    }
    if (0 != (b & (1<<0))) {
        m |= SAPP_MODIFIER_LMB;
    }
    if (0 != (b & (1<<1))) {
        m |= SAPP_MODIFIER_RMB;
    }
    if (0 != (b & (1<<2))) {
        m |= SAPP_MODIFIER_MMB;
    }
    return m;
}

_SOKOL_PRIVATE void _sapp_macos_mouse_event(sapp_event_type type, sapp_mousebutton btn, uint32_t mod) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.mouse_button = btn;
        _sapp.event.modifiers = mod;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_macos_key_event(sapp_event_type type, sapp_keycode key, bool repeat, uint32_t mod) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp.event.key_code = key;
        _sapp.event.key_repeat = repeat;
        _sapp.event.modifiers = mod;
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_macos_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

/* NOTE: unlike the iOS version of this function, the macOS version
    can dynamically update the DPI scaling factor when a window is moved
    between HighDPI / LowDPI screens.
*/
_SOKOL_PRIVATE void _sapp_macos_update_dimensions(void) {
    if (_sapp.desc.high_dpi) {
        _sapp.dpi_scale = [_sapp.macos.window screen].backingScaleFactor;
    }
    else {
        _sapp.dpi_scale = 1.0f;
    }
    _sapp.macos.view.layer.contentsScale = _sapp.dpi_scale; // NOTE: needed because we set layerContentsPlacement to a non-scaling value in windowWillStartLiveResize.
    const NSRect bounds = [_sapp.macos.view bounds];
    _sapp.window_width = (int)roundf(bounds.size.width);
    _sapp.window_height = (int)roundf(bounds.size.height);
    _sapp.framebuffer_width = (int)roundf(bounds.size.width * _sapp.dpi_scale);
    _sapp.framebuffer_height = (int)roundf(bounds.size.height * _sapp.dpi_scale);
    const CGSize fb_size = _sapp.macos.view.drawableSize;
    const int cur_fb_width = (int)roundf(fb_size.width);
    const int cur_fb_height = (int)roundf(fb_size.height);
    const bool dim_changed = (_sapp.framebuffer_width != cur_fb_width) ||
                             (_sapp.framebuffer_height != cur_fb_height);
    if (_sapp.framebuffer_width == 0) {
        _sapp.framebuffer_width = 1;
    }
    if (_sapp.framebuffer_height == 0) {
        _sapp.framebuffer_height = 1;
    }
    if (_sapp.window_width == 0) {
        _sapp.window_width = 1;
    }
    if (_sapp.window_height == 0) {
        _sapp.window_height = 1;
    }
    if (dim_changed) {
        #if defined(SOKOL_METAL)
            CGSize drawable_size = { (CGFloat) _sapp.framebuffer_width, (CGFloat) _sapp.framebuffer_height };
            _sapp.macos.view.drawableSize = drawable_size;
        #else
            // nothing to do for GL?
        #endif
        if (!_sapp.first_frame) {
            _sapp_macos_app_event(SAPP_EVENTTYPE_RESIZED);
        }
    }
}

_SOKOL_PRIVATE void _sapp_macos_toggle_fullscreen(void) {
    /* NOTE: the _sapp.fullscreen flag is also notified by the
       windowDidEnterFullscreen / windowDidExitFullscreen
       event handlers
    */
    _sapp.fullscreen = !_sapp.fullscreen;
    [_sapp.macos.window toggleFullScreen:nil];
}

_SOKOL_PRIVATE void _sapp_macos_set_clipboard_string(const char* str) {
    @autoreleasepool {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard declareTypes:@[NSPasteboardTypeString] owner:nil];
        [pasteboard setString:@(str) forType:NSPasteboardTypeString];
    }
}

_SOKOL_PRIVATE const char* _sapp_macos_get_clipboard_string(void) {
    SOKOL_ASSERT(_sapp.clipboard.buffer);
    @autoreleasepool {
        _sapp.clipboard.buffer[0] = 0;
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        if (![[pasteboard types] containsObject:NSPasteboardTypeString]) {
            return _sapp.clipboard.buffer;
        }
        NSString* str = [pasteboard stringForType:NSPasteboardTypeString];
        if (!str) {
            return _sapp.clipboard.buffer;
        }
        _sapp_strcpy([str UTF8String], _sapp.clipboard.buffer, _sapp.clipboard.buf_size);
    }
    return _sapp.clipboard.buffer;
}

_SOKOL_PRIVATE void _sapp_macos_update_window_title(void) {
    [_sapp.macos.window setTitle: [NSString stringWithUTF8String:_sapp.window_title]];
}

_SOKOL_PRIVATE void _sapp_macos_mouse_update_from_nspoint(NSPoint mouse_pos, bool clear_dxdy) {
    if (!_sapp.mouse.locked) {
        float new_x = mouse_pos.x * _sapp.dpi_scale;
        float new_y = _sapp.framebuffer_height - (mouse_pos.y * _sapp.dpi_scale) - 1;
        if (clear_dxdy) {
            _sapp.mouse.dx = 0.0f;
            _sapp.mouse.dy = 0.0f;
        }
        else if (_sapp.mouse.pos_valid) {
            // don't update dx/dy in the very first update
            _sapp.mouse.dx = new_x - _sapp.mouse.x;
            _sapp.mouse.dy = new_y - _sapp.mouse.y;
        }
        _sapp.mouse.x = new_x;
        _sapp.mouse.y = new_y;
        _sapp.mouse.pos_valid = true;
    }
}

_SOKOL_PRIVATE void _sapp_macos_mouse_update_from_nsevent(NSEvent* event, bool clear_dxdy) {
    _sapp_macos_mouse_update_from_nspoint(event.locationInWindow, clear_dxdy);
}

_SOKOL_PRIVATE void _sapp_macos_show_mouse(bool visible) {
    /* NOTE: this function is only called when the mouse visibility actually changes */
    if (visible) {
        CGDisplayShowCursor(kCGDirectMainDisplay);
    }
    else {
        CGDisplayHideCursor(kCGDirectMainDisplay);
    }
}

_SOKOL_PRIVATE void _sapp_macos_lock_mouse(bool lock) {
    if (lock == _sapp.mouse.locked) {
        return;
    }
    _sapp.mouse.dx = 0.0f;
    _sapp.mouse.dy = 0.0f;
    _sapp.mouse.locked = lock;
    /*
        NOTE that this code doesn't warp the mouse cursor to the window
        center as everybody else does it. This lead to a spike in the
        *second* mouse-moved event after the warp happened. The
        mouse centering doesn't seem to be required (mouse-moved events
        are reported correctly even when the cursor is at an edge of the screen).

        NOTE also that the hide/show of the mouse cursor should properly
        stack with calls to sapp_show_mouse()
    */
    if (_sapp.mouse.locked) {
        CGAssociateMouseAndMouseCursorPosition(NO);
        [NSCursor hide];
    }
    else {
        [NSCursor unhide];
        CGAssociateMouseAndMouseCursorPosition(YES);
    }
}

_SOKOL_PRIVATE void _sapp_macos_update_cursor(sapp_mouse_cursor cursor, bool shown) {
    // show/hide cursor only if visibility status has changed (required because show/hide stacks)
    if (shown != _sapp.mouse.shown) {
        if (shown) {
            [NSCursor unhide];
        }
        else {
            [NSCursor hide];
        }
    }
    // update cursor type
    SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
    if (_sapp.macos.cursors[cursor]) {
        [_sapp.macos.cursors[cursor] set];
    }
    else {
        [[NSCursor arrowCursor] set];
    }
}

_SOKOL_PRIVATE void _sapp_macos_set_icon(const sapp_icon_desc* icon_desc, int num_images) {
    NSDockTile* dock_tile = NSApp.dockTile;
    const int wanted_width = (int) dock_tile.size.width;
    const int wanted_height = (int) dock_tile.size.height;
    const int img_index = _sapp_image_bestmatch(icon_desc->images, num_images, wanted_width, wanted_height);
    const sapp_image_desc* img_desc = &icon_desc->images[img_index];

    CGColorSpaceRef cg_color_space = CGColorSpaceCreateDeviceRGB();
    CFDataRef cf_data = CFDataCreate(kCFAllocatorDefault, (const UInt8*)img_desc->pixels.ptr, (CFIndex)img_desc->pixels.size);
    CGDataProviderRef cg_data_provider = CGDataProviderCreateWithCFData(cf_data);
    CGImageRef cg_img = CGImageCreate(
        (size_t)img_desc->width,    // width
        (size_t)img_desc->height,   // height
        8,                          // bitsPerComponent
        32,                         // bitsPerPixel
        (size_t)img_desc->width * 4,// bytesPerRow
        cg_color_space,             // space
        kCGImageAlphaLast | kCGImageByteOrderDefault,  // bitmapInfo
        cg_data_provider,           // provider
        NULL,                       // decode
        false,                      // shouldInterpolate
        kCGRenderingIntentDefault);
    CFRelease(cf_data);
    CGDataProviderRelease(cg_data_provider);
    CGColorSpaceRelease(cg_color_space);

    NSImage* ns_image = [[NSImage alloc] initWithCGImage:cg_img size:dock_tile.size];
    dock_tile.contentView = [NSImageView imageViewWithImage:ns_image];
    [dock_tile display];
    _SAPP_OBJC_RELEASE(ns_image);
    CGImageRelease(cg_img);
}

_SOKOL_PRIVATE void _sapp_macos_frame(void) {
    _sapp_frame();
    if (_sapp.quit_requested || _sapp.quit_ordered) {
        [_sapp.macos.window performClose:nil];
    }
}

@implementation _sapp_macos_app_delegate
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification {
    _SOKOL_UNUSED(aNotification);
    _sapp_macos_init_cursors();
    if ((_sapp.window_width == 0) || (_sapp.window_height == 0)) {
        // use 4/5 of screen size as default size
        NSRect screen_rect = NSScreen.mainScreen.frame;
        if (_sapp.window_width == 0) {
            _sapp.window_width = (int)roundf((screen_rect.size.width * 4.0f) / 5.0f);
        }
        if (_sapp.window_height == 0) {
            _sapp.window_height = (int)roundf((screen_rect.size.height * 4.0f) / 5.0f);
        }
    }
    const NSUInteger style =
        NSWindowStyleMaskTitled |
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable;
    NSRect window_rect = NSMakeRect(0, 0, _sapp.window_width, _sapp.window_height);
    _sapp.macos.window = [[_sapp_macos_window alloc]
        initWithContentRect:window_rect
        styleMask:style
        backing:NSBackingStoreBuffered
        defer:NO];
    _sapp.macos.window.releasedWhenClosed = NO; // this is necessary for proper cleanup in applicationWillTerminate
    _sapp.macos.window.title = [NSString stringWithUTF8String:_sapp.window_title];
    _sapp.macos.window.acceptsMouseMovedEvents = YES;
    _sapp.macos.window.restorable = YES;

    _sapp.macos.win_dlg = [[_sapp_macos_window_delegate alloc] init];
    _sapp.macos.window.delegate = _sapp.macos.win_dlg;
    NSInteger max_fps = 60;
    #if (__MAC_OS_X_VERSION_MAX_ALLOWED >= 120000)
    if (@available(macOS 12.0, *)) {
        max_fps = [NSScreen.mainScreen maximumFramesPerSecond];
    }
    #endif
    _sapp.macos.mtl_device = MTLCreateSystemDefaultDevice();
    _sapp.macos.view = [[_sapp_macos_view alloc] init];
    [_sapp.macos.view updateTrackingAreas];
    _sapp.macos.view.preferredFramesPerSecond = max_fps / _sapp.swap_interval;
    _sapp.macos.view.device = _sapp.macos.mtl_device;
    _sapp.macos.view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    _sapp.macos.view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    _sapp.macos.view.sampleCount = (NSUInteger) _sapp.sample_count;
    _sapp.macos.view.autoResizeDrawable = false;
    _sapp.macos.window.contentView = _sapp.macos.view;
    [_sapp.macos.window makeFirstResponder:_sapp.macos.view];
    _sapp.macos.view.layer.magnificationFilter = kCAFilterNearest;
    [_sapp.macos.window center];
    _sapp.valid = true;
    if (_sapp.fullscreen) {
        /* ^^^ on GL, this already toggles a rendered frame, so set the valid flag before */
        [_sapp.macos.window toggleFullScreen:self];
    }
    NSApp.activationPolicy = NSApplicationActivationPolicyRegular;
    [NSApp activateIgnoringOtherApps:YES];
    [_sapp.macos.window makeKeyAndOrderFront:nil];
    _sapp_macos_update_dimensions();
    [NSEvent setMouseCoalescingEnabled:NO];

    // workaround for window not being focused during a long init callback
    // for details see: https://github.com/floooh/sokol/pull/982
    // also see: https://gitlab.gnome.org/GNOME/gtk/-/issues/2342
    NSEvent *focusevent = [NSEvent otherEventWithType:NSEventTypeAppKitDefined
        location:NSZeroPoint
        modifierFlags:0x40
        timestamp:0
        windowNumber:0
        context:nil
        subtype:NSEventSubtypeApplicationActivated
        data1:0
        data2:0];
    [NSApp postEvent:focusevent atStart:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    _SOKOL_UNUSED(sender);
    return YES;
}

- (void)applicationWillTerminate:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_call_cleanup();
    _sapp_macos_discard_state();
    _sapp_discard_state();
}
@end

@implementation _sapp_macos_window_delegate
- (BOOL)windowShouldClose:(id)sender {
    _SOKOL_UNUSED(sender);
    /* only give user-code a chance to intervene when sapp_quit() wasn't already called */
    if (!_sapp.quit_ordered) {
        /* if window should be closed and event handling is enabled, give user code
           a chance to intervene via sapp_cancel_quit()
        */
        _sapp.quit_requested = true;
        _sapp_macos_app_event(SAPP_EVENTTYPE_QUIT_REQUESTED);
        /* user code hasn't intervened, quit the app */
        if (_sapp.quit_requested) {
            _sapp.quit_ordered = true;
        }
    }
    if (_sapp.quit_ordered) {
        return YES;
    }
    else {
        return NO;
    }
}

#if defined(SOKOL_METAL)
- (void)windowWillStartLiveResize:(NSNotification *)notification {
    // Work around the MTKView resizing glitch by "anchoring" the layer to the window corner opposite
    // to the currently manipulated corner (or edge). This prevents the content stretching back and
    // forth during resizing. This is a workaround for this issue: https://github.com/floooh/sokol/issues/700
    // Can be removed if/when migrating to CAMetalLayer: https://github.com/floooh/sokol/issues/727
    bool resizing_from_left = _sapp.mouse.x < _sapp.window_width/2;
    bool resizing_from_top = _sapp.mouse.y < _sapp.window_height/2;
    NSViewLayerContentsPlacement placement;
    if (resizing_from_left) {
        placement = resizing_from_top ? NSViewLayerContentsPlacementBottomRight : NSViewLayerContentsPlacementTopRight;
    } else {
        placement = resizing_from_top ? NSViewLayerContentsPlacementBottomLeft : NSViewLayerContentsPlacementTopLeft;
    }
    _sapp.macos.view.layerContentsPlacement = placement;
}
#endif

- (void)windowDidResize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_update_dimensions();
}

- (void)windowDidChangeScreen:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_timing_reset(&_sapp.timing);
    _sapp_macos_update_dimensions();
}

- (void)windowDidMiniaturize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_ICONIFIED);
}

- (void)windowDidDeminiaturize:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_RESTORED);
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_FOCUSED);
}

- (void)windowDidResignKey:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp_macos_app_event(SAPP_EVENTTYPE_UNFOCUSED);
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp.fullscreen = true;
}

- (void)windowDidExitFullScreen:(NSNotification*)notification {
    _SOKOL_UNUSED(notification);
    _sapp.fullscreen = false;
}
@end

@implementation _sapp_macos_window
- (instancetype)initWithContentRect:(NSRect)contentRect
                          styleMask:(NSWindowStyleMask)style
                            backing:(NSBackingStoreType)backingStoreType
                              defer:(BOOL)flag {
    if (self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag]) {
        #if __MAC_OS_X_VERSION_MAX_ALLOWED >= 101300
            [self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
        #endif
    }
    return self;
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    return NSDragOperationCopy;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender {
    return NSDragOperationCopy;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
    #if __MAC_OS_X_VERSION_MAX_ALLOWED >= 101300
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ([pboard.types containsObject:NSPasteboardTypeFileURL]) {
        _sapp_clear_drop_buffer();
        _sapp.drop.num_files = ((int)pboard.pasteboardItems.count > _sapp.drop.max_files) ? _sapp.drop.max_files : (int)pboard.pasteboardItems.count;
        bool drop_failed = false;
        for (int i = 0; i < _sapp.drop.num_files; i++) {
            NSURL *fileUrl = [NSURL fileURLWithPath:[pboard.pasteboardItems[(NSUInteger)i] stringForType:NSPasteboardTypeFileURL]];
            if (!_sapp_strcpy(fileUrl.standardizedURL.path.UTF8String, _sapp_dropped_file_path_ptr(i), _sapp.drop.max_path_length)) {
                _SAPP_ERROR(DROPPED_FILE_PATH_TOO_LONG);
                drop_failed = true;
                break;
            }
        }
        if (!drop_failed) {
            if (_sapp_events_enabled()) {
                _sapp_macos_mouse_update_from_nspoint(sender.draggingLocation, true);
                _sapp_init_event(SAPP_EVENTTYPE_FILES_DROPPED);
                _sapp.event.modifiers = _sapp_macos_mods(nil);
                _sapp_call_event(&_sapp.event);
            }
        }
        else {
            _sapp_clear_drop_buffer();
            _sapp.drop.num_files = 0;
        }
        return YES;
    }
    #endif
    return NO;
}
@end

@implementation _sapp_macos_view
_SOKOL_PRIVATE void _sapp_macos_poll_input_events(void) {
    /*

    NOTE: late event polling temporarily out-commented to check if this
    causes infrequent and almost impossible to reproduce problems with the
    window close events, see:
    https://github.com/floooh/sokol/pull/483#issuecomment-805148815


    const NSEventMask mask = NSEventMaskLeftMouseDown |
                             NSEventMaskLeftMouseUp|
                             NSEventMaskRightMouseDown |
                             NSEventMaskRightMouseUp |
                             NSEventMaskMouseMoved |
                             NSEventMaskLeftMouseDragged |
                             NSEventMaskRightMouseDragged |
                             NSEventMaskMouseEntered |
                             NSEventMaskMouseExited |
                             NSEventMaskKeyDown |
                             NSEventMaskKeyUp |
                             NSEventMaskCursorUpdate |
                             NSEventMaskScrollWheel |
                             NSEventMaskTabletPoint |
                             NSEventMaskTabletProximity |
                             NSEventMaskOtherMouseDown |
                             NSEventMaskOtherMouseUp |
                             NSEventMaskOtherMouseDragged |
                             NSEventMaskPressure |
                             NSEventMaskDirectTouch;
    @autoreleasepool {
        for (;;) {
            // NOTE: using NSDefaultRunLoopMode here causes stuttering in the GL backend,
            // see: https://github.com/floooh/sokol/issues/486
            NSEvent* event = [NSApp nextEventMatchingMask:mask untilDate:nil inMode:NSEventTrackingRunLoopMode dequeue:YES];
            if (event == nil) {
                break;
            }
            [NSApp sendEvent:event];
        }
    }
    */
}

- (void)drawRect:(NSRect)rect {
    _SOKOL_UNUSED(rect);
    _sapp_timing_measure(&_sapp.timing);
    /* Catch any last-moment input events */
    _sapp_macos_poll_input_events();
    @autoreleasepool {
        _sapp_macos_frame();
    }
}

- (BOOL)isOpaque {
    return YES;
}
- (BOOL)canBecomeKeyView {
    return YES;
}
- (BOOL)acceptsFirstResponder {
    return YES;
}
- (void)updateTrackingAreas {
    if (_sapp.macos.tracking_area != nil) {
        [self removeTrackingArea:_sapp.macos.tracking_area];
        _SAPP_OBJC_RELEASE(_sapp.macos.tracking_area);
    }
    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
                                          NSTrackingActiveInKeyWindow |
                                          NSTrackingEnabledDuringMouseDrag |
                                          NSTrackingCursorUpdate |
                                          NSTrackingInVisibleRect |
                                          NSTrackingAssumeInside;
    _sapp.macos.tracking_area = [[NSTrackingArea alloc] initWithRect:[self bounds] options:options owner:self userInfo:nil];
    [self addTrackingArea:_sapp.macos.tracking_area];
    [super updateTrackingAreas];
}

// helper function to make GL context active
static void _sapp_gl_make_current(void) {}

- (void)mouseEntered:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, true);
    /* don't send mouse enter/leave while dragging (so that it behaves the same as
       on Windows while SetCapture is active
    */
    if (0 == _sapp.macos.mouse_buttons) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_ENTER, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mods(event));
    }
}
- (void)mouseExited:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, true);
    if (0 == _sapp.macos.mouse_buttons) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_LEAVE, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mods(event));
    }
}
- (void)mouseDown:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_LEFT, _sapp_macos_mods(event));
    _sapp.macos.mouse_buttons |= (1<<SAPP_MOUSEBUTTON_LEFT);
}
- (void)mouseUp:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_LEFT, _sapp_macos_mods(event));
    _sapp.macos.mouse_buttons &= ~(1<<SAPP_MOUSEBUTTON_LEFT);
}
- (void)rightMouseDown:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_RIGHT, _sapp_macos_mods(event));
    _sapp.macos.mouse_buttons |= (1<<SAPP_MOUSEBUTTON_RIGHT);
}
- (void)rightMouseUp:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_RIGHT, _sapp_macos_mods(event));
    _sapp.macos.mouse_buttons &= ~(1<<SAPP_MOUSEBUTTON_RIGHT);
}
- (void)otherMouseDown:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (2 == event.buttonNumber) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_DOWN, SAPP_MOUSEBUTTON_MIDDLE, _sapp_macos_mods(event));
        _sapp.macos.mouse_buttons |= (1<<SAPP_MOUSEBUTTON_MIDDLE);
    }
}
- (void)otherMouseUp:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (2 == event.buttonNumber) {
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_UP, SAPP_MOUSEBUTTON_MIDDLE, _sapp_macos_mods(event));
        _sapp.macos.mouse_buttons &= (1<<SAPP_MOUSEBUTTON_MIDDLE);
    }
}
- (void)otherMouseDragged:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (2 == event.buttonNumber) {
        if (_sapp.mouse.locked) {
            _sapp.mouse.dx = [event deltaX];
            _sapp.mouse.dy = [event deltaY];
        }
        _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mods(event));
    }
}
- (void)mouseMoved:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (_sapp.mouse.locked) {
        _sapp.mouse.dx = [event deltaX];
        _sapp.mouse.dy = [event deltaY];
    }
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID , _sapp_macos_mods(event));
}
- (void)mouseDragged:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (_sapp.mouse.locked) {
        _sapp.mouse.dx = [event deltaX];
        _sapp.mouse.dy = [event deltaY];
    }
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID , _sapp_macos_mods(event));
}
- (void)rightMouseDragged:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, false);
    if (_sapp.mouse.locked) {
        _sapp.mouse.dx = [event deltaX];
        _sapp.mouse.dy = [event deltaY];
    }
    _sapp_macos_mouse_event(SAPP_EVENTTYPE_MOUSE_MOVE, SAPP_MOUSEBUTTON_INVALID, _sapp_macos_mods(event));
}
- (void)scrollWheel:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_mouse_update_from_nsevent(event, true);
    if (_sapp_events_enabled()) {
        float dx = (float) event.scrollingDeltaX;
        float dy = (float) event.scrollingDeltaY;
        if (event.hasPreciseScrollingDeltas) {
            dx *= 0.1;
            dy *= 0.1;
        }
        if ((_sapp_absf(dx) > 0.0f) || (_sapp_absf(dy) > 0.0f)) {
            _sapp_init_event(SAPP_EVENTTYPE_MOUSE_SCROLL);
            _sapp.event.modifiers = _sapp_macos_mods(event);
            _sapp.event.scroll_x = dx;
            _sapp.event.scroll_y = dy;
            _sapp_call_event(&_sapp.event);
        }
    }
}
- (void)keyDown:(NSEvent*)event {
    if (_sapp_events_enabled()) {
        _sapp_gl_make_current();
        const uint32_t mods = _sapp_macos_mods(event);
        const sapp_keycode key_code = _sapp_translate_key(event.keyCode);
        _sapp_macos_key_event(SAPP_EVENTTYPE_KEY_DOWN, key_code, event.isARepeat, mods);
        const NSString* chars = event.characters;
        const NSUInteger len = chars.length;
        if (len > 0) {
            _sapp_init_event(SAPP_EVENTTYPE_CHAR);
            _sapp.event.modifiers = mods;
            for (NSUInteger i = 0; i < len; i++) {
                const unichar codepoint = [chars characterAtIndex:i];
                if ((codepoint & 0xFF00) == 0xF700) {
                    continue;
                }
                _sapp.event.char_code = codepoint;
                _sapp.event.key_repeat = event.isARepeat;
                _sapp_call_event(&_sapp.event);
            }
        }
        /* if this is a Cmd+V (paste), also send a CLIPBOARD_PASTE event */
        if (_sapp.clipboard.enabled && (mods == SAPP_MODIFIER_SUPER) && (key_code == SAPP_KEYCODE_V)) {
            _sapp_init_event(SAPP_EVENTTYPE_CLIPBOARD_PASTED);
            _sapp_call_event(&_sapp.event);
        }
    }
}
- (void)keyUp:(NSEvent*)event {
    _sapp_gl_make_current();
    _sapp_macos_key_event(SAPP_EVENTTYPE_KEY_UP,
        _sapp_translate_key(event.keyCode),
        event.isARepeat,
        _sapp_macos_mods(event));
}
- (void)flagsChanged:(NSEvent*)event {
    const uint32_t old_f = _sapp.macos.flags_changed_store;
    const uint32_t new_f = (uint32_t)event.modifierFlags;
    _sapp.macos.flags_changed_store = new_f;
    sapp_keycode key_code = SAPP_KEYCODE_INVALID;
    bool down = false;
    if ((new_f ^ old_f) & NSEventModifierFlagShift) {
        key_code = SAPP_KEYCODE_LEFT_SHIFT;
        down = 0 != (new_f & NSEventModifierFlagShift);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagControl) {
        key_code = SAPP_KEYCODE_LEFT_CONTROL;
        down = 0 != (new_f & NSEventModifierFlagControl);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagOption) {
        key_code = SAPP_KEYCODE_LEFT_ALT;
        down = 0 != (new_f & NSEventModifierFlagOption);
    }
    if ((new_f ^ old_f) & NSEventModifierFlagCommand) {
        key_code = SAPP_KEYCODE_LEFT_SUPER;
        down = 0 != (new_f & NSEventModifierFlagCommand);
    }
    if (key_code != SAPP_KEYCODE_INVALID) {
        _sapp_macos_key_event(down ? SAPP_EVENTTYPE_KEY_DOWN : SAPP_EVENTTYPE_KEY_UP,
            key_code,
            false,
            _sapp_macos_mods(event));
    }
}
@end

