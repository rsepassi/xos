// ██  ██████  ███████
// ██ ██    ██ ██
// ██ ██    ██ ███████
// ██ ██    ██      ██
// ██  ██████  ███████
//
// >>ios

_SOKOL_PRIVATE void _sapp_ios_discard_state(void) {
    // NOTE: it's safe to call [release] on a nil object
    _SAPP_OBJC_RELEASE(_sapp.ios.textfield_dlg);
    _SAPP_OBJC_RELEASE(_sapp.ios.textfield);
    #if defined(SOKOL_METAL)
        _SAPP_OBJC_RELEASE(_sapp.ios.view_ctrl);
        _SAPP_OBJC_RELEASE(_sapp.ios.mtl_device);
    #else
        _SAPP_OBJC_RELEASE(_sapp.ios.view_ctrl);
        _SAPP_OBJC_RELEASE(_sapp.ios.eagl_ctx);
    #endif
    _SAPP_OBJC_RELEASE(_sapp.ios.view);
    _SAPP_OBJC_RELEASE(_sapp.ios.window);
}

_SOKOL_PRIVATE void _sapp_ios_run(const sapp_desc* desc) {
    _sapp_init_state(desc);
    static int argc = 1;
    static char* argv[] = { (char*)"sokol_app" };
    UIApplicationMain(argc, argv, nil, NSStringFromClass([_sapp_app_delegate class]));
}

/* iOS entry function */
#if !defined(SOKOL_NO_ENTRY)
int main(int argc, char* argv[]) {
    sapp_desc desc = sokol_main(argc, argv);
    _sapp_ios_run(&desc);
    return 0;
}
#endif /* SOKOL_NO_ENTRY */

_SOKOL_PRIVATE void _sapp_ios_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_ios_touch_event(sapp_event_type type, NSSet<UITouch *>* touches, UIEvent* event) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        NSEnumerator* enumerator = event.allTouches.objectEnumerator;
        UITouch* ios_touch;
        while ((ios_touch = [enumerator nextObject])) {
            if ((_sapp.event.num_touches + 1) < SAPP_MAX_TOUCHPOINTS) {
                CGPoint ios_pos = [ios_touch locationInView:_sapp.ios.view];
                sapp_touchpoint* cur_point = &_sapp.event.touches[_sapp.event.num_touches++];
                cur_point->identifier = (uintptr_t) ios_touch;
                cur_point->pos_x = ios_pos.x * _sapp.dpi_scale;
                cur_point->pos_y = ios_pos.y * _sapp.dpi_scale;
                cur_point->changed = [touches containsObject:ios_touch];
            }
        }
        if (_sapp.event.num_touches > 0) {
            _sapp_call_event(&_sapp.event);
        }
    }
}

_SOKOL_PRIVATE void _sapp_ios_update_dimensions(void) {
    CGRect screen_rect = UIScreen.mainScreen.bounds;
    _sapp.framebuffer_width = (int)roundf(screen_rect.size.width * _sapp.dpi_scale);
    _sapp.framebuffer_height = (int)roundf(screen_rect.size.height * _sapp.dpi_scale);
    _sapp.window_width = (int)roundf(screen_rect.size.width);
    _sapp.window_height = (int)roundf(screen_rect.size.height);
    int cur_fb_width, cur_fb_height;
    #if defined(SOKOL_METAL)
        const CGSize fb_size = _sapp.ios.view.drawableSize;
        cur_fb_width = (int)roundf(fb_size.width);
        cur_fb_height = (int)roundf(fb_size.height);
    #else
        cur_fb_width = (int)roundf(_sapp.ios.view.drawableWidth);
        cur_fb_height = (int)roundf(_sapp.ios.view.drawableHeight);
    #endif
    const bool dim_changed = (_sapp.framebuffer_width != cur_fb_width) ||
                             (_sapp.framebuffer_height != cur_fb_height);
    if (dim_changed) {
        #if defined(SOKOL_METAL)
            const CGSize drawable_size = { (CGFloat) _sapp.framebuffer_width, (CGFloat) _sapp.framebuffer_height };
            _sapp.ios.view.drawableSize = drawable_size;
        #else
            // nothing to do here, GLKView correctly respects the view's contentScaleFactor
        #endif
        if (!_sapp.first_frame) {
            _sapp_ios_app_event(SAPP_EVENTTYPE_RESIZED);
        }
    }
}

_SOKOL_PRIVATE void _sapp_ios_frame(void) {
    _sapp_ios_update_dimensions();
    _sapp_frame();
}

_SOKOL_PRIVATE void _sapp_ios_show_keyboard(bool shown) {
    /* if not happened yet, create an invisible text field */
    if (nil == _sapp.ios.textfield) {
        _sapp.ios.textfield_dlg = [[_sapp_textfield_dlg alloc] init];
        _sapp.ios.textfield = [[UITextField alloc] initWithFrame:CGRectMake(10, 10, 100, 50)];
        _sapp.ios.textfield.keyboardType = UIKeyboardTypeDefault;
        _sapp.ios.textfield.returnKeyType = UIReturnKeyDefault;
        _sapp.ios.textfield.autocapitalizationType = UITextAutocapitalizationTypeNone;
        _sapp.ios.textfield.autocorrectionType = UITextAutocorrectionTypeNo;
        _sapp.ios.textfield.spellCheckingType = UITextSpellCheckingTypeNo;
        _sapp.ios.textfield.hidden = YES;
        _sapp.ios.textfield.text = @"x";
        _sapp.ios.textfield.delegate = _sapp.ios.textfield_dlg;
        [_sapp.ios.view_ctrl.view addSubview:_sapp.ios.textfield];

        [[NSNotificationCenter defaultCenter] addObserver:_sapp.ios.textfield_dlg
            selector:@selector(keyboardWasShown:)
            name:UIKeyboardDidShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:_sapp.ios.textfield_dlg
            selector:@selector(keyboardWillBeHidden:)
            name:UIKeyboardWillHideNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:_sapp.ios.textfield_dlg
            selector:@selector(keyboardDidChangeFrame:)
            name:UIKeyboardDidChangeFrameNotification object:nil];
    }
    if (shown) {
        /* setting the text field as first responder brings up the onscreen keyboard */
        [_sapp.ios.textfield becomeFirstResponder];
    }
    else {
        [_sapp.ios.textfield resignFirstResponder];
    }
}

@implementation _sapp_app_delegate
- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    CGRect screen_rect = UIScreen.mainScreen.bounds;
    _sapp.ios.window = [[UIWindow alloc] initWithFrame:screen_rect];
    _sapp.window_width = (int)roundf(screen_rect.size.width);
    _sapp.window_height = (int)roundf(screen_rect.size.height);
    if (_sapp.desc.high_dpi) {
        _sapp.dpi_scale = (float) UIScreen.mainScreen.nativeScale;
    }
    else {
        _sapp.dpi_scale = 1.0f;
    }
    _sapp.framebuffer_width = (int)roundf(_sapp.window_width * _sapp.dpi_scale);
    _sapp.framebuffer_height = (int)roundf(_sapp.window_height * _sapp.dpi_scale);
    NSInteger max_fps = UIScreen.mainScreen.maximumFramesPerSecond;
    #if defined(SOKOL_METAL)
        _sapp.ios.mtl_device = MTLCreateSystemDefaultDevice();
        _sapp.ios.view = [[_sapp_ios_view alloc] init];
        _sapp.ios.view.preferredFramesPerSecond = max_fps / _sapp.swap_interval;
        _sapp.ios.view.device = _sapp.ios.mtl_device;
        _sapp.ios.view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
        _sapp.ios.view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        _sapp.ios.view.sampleCount = (NSUInteger)_sapp.sample_count;
        /* NOTE: iOS MTKView seems to ignore thew view's contentScaleFactor
            and automatically renders at Retina resolution. We'll disable
            autoResize and instead do the resizing in _sapp_ios_update_dimensions()
        */
        _sapp.ios.view.autoResizeDrawable = false;
        _sapp.ios.view.userInteractionEnabled = YES;
        _sapp.ios.view.multipleTouchEnabled = YES;
        _sapp.ios.view_ctrl = [[UIViewController alloc] init];
        _sapp.ios.view_ctrl.modalPresentationStyle = UIModalPresentationFullScreen;
        _sapp.ios.view_ctrl.view = _sapp.ios.view;
        _sapp.ios.window.rootViewController = _sapp.ios.view_ctrl;
    #else
        _sapp.ios.eagl_ctx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        _sapp.ios.view = [[_sapp_ios_view alloc] initWithFrame:screen_rect];
        _sapp.ios.view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
        _sapp.ios.view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        _sapp.ios.view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
        GLKViewDrawableMultisample msaa = _sapp.sample_count > 1 ? GLKViewDrawableMultisample4X : GLKViewDrawableMultisampleNone;
        _sapp.ios.view.drawableMultisample = msaa;
        _sapp.ios.view.context = _sapp.ios.eagl_ctx;
        _sapp.ios.view.enableSetNeedsDisplay = NO;
        _sapp.ios.view.userInteractionEnabled = YES;
        _sapp.ios.view.multipleTouchEnabled = YES;
        // on GLKView, contentScaleFactor appears to work just fine!
        if (_sapp.desc.high_dpi) {
            _sapp.ios.view.contentScaleFactor = _sapp.dpi_scale;
        }
        else {
            _sapp.ios.view.contentScaleFactor = 1.0;
        }
        _sapp.ios.view_ctrl = [[GLKViewController alloc] init];
        _sapp.ios.view_ctrl.view = _sapp.ios.view;
        _sapp.ios.view_ctrl.preferredFramesPerSecond = max_fps / _sapp.swap_interval;
        _sapp.ios.window.rootViewController = _sapp.ios.view_ctrl;
    #endif
    [_sapp.ios.window makeKeyAndVisible];

    _sapp.valid = true;
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    if (!_sapp.ios.suspended) {
        _sapp.ios.suspended = true;
        _sapp_ios_app_event(SAPP_EVENTTYPE_SUSPENDED);
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    if (_sapp.ios.suspended) {
        _sapp.ios.suspended = false;
        _sapp_ios_app_event(SAPP_EVENTTYPE_RESUMED);
    }
}

/* NOTE: this method will rarely ever be called, iOS application
    which are terminated by the user are usually killed via signal 9
    by the operating system.
*/
- (void)applicationWillTerminate:(UIApplication *)application {
    _SOKOL_UNUSED(application);
    _sapp_call_cleanup();
    _sapp_ios_discard_state();
    _sapp_discard_state();
}
@end

@implementation _sapp_textfield_dlg
- (void)keyboardWasShown:(NSNotification*)notif {
    _sapp.onscreen_keyboard_shown = true;
    /* query the keyboard's size, and modify the content view's size */
    if (_sapp.desc.ios_keyboard_resizes_canvas) {
        NSDictionary* info = notif.userInfo;
        CGFloat kbd_h = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size.height;
        CGRect view_frame = UIScreen.mainScreen.bounds;
        view_frame.size.height -= kbd_h;
        _sapp.ios.view.frame = view_frame;
    }
}
- (void)keyboardWillBeHidden:(NSNotification*)notif {
    _sapp.onscreen_keyboard_shown = false;
    if (_sapp.desc.ios_keyboard_resizes_canvas) {
        _sapp.ios.view.frame = UIScreen.mainScreen.bounds;
    }
}
- (void)keyboardDidChangeFrame:(NSNotification*)notif {
    /* this is for the case when the screen rotation changes while the keyboard is open */
    if (_sapp.onscreen_keyboard_shown && _sapp.desc.ios_keyboard_resizes_canvas) {
        NSDictionary* info = notif.userInfo;
        CGFloat kbd_h = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size.height;
        CGRect view_frame = UIScreen.mainScreen.bounds;
        view_frame.size.height -= kbd_h;
        _sapp.ios.view.frame = view_frame;
    }
}
- (BOOL)textField:(UITextField*)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string {
    if (_sapp_events_enabled()) {
        const NSUInteger len = string.length;
        if (len > 0) {
            for (NSUInteger i = 0; i < len; i++) {
                unichar c = [string characterAtIndex:i];
                if (c >= 32) {
                    /* ignore surrogates for now */
                    if ((c < 0xD800) || (c > 0xDFFF)) {
                        _sapp_init_event(SAPP_EVENTTYPE_CHAR);
                        _sapp.event.char_code = c;
                        _sapp_call_event(&_sapp.event);
                    }
                }
                if (c <= 32) {
                    sapp_keycode k = SAPP_KEYCODE_INVALID;
                    switch (c) {
                        case 10: k = SAPP_KEYCODE_ENTER; break;
                        case 32: k = SAPP_KEYCODE_SPACE; break;
                        default: break;
                    }
                    if (k != SAPP_KEYCODE_INVALID) {
                        _sapp_init_event(SAPP_EVENTTYPE_KEY_DOWN);
                        _sapp.event.key_code = k;
                        _sapp_call_event(&_sapp.event);
                        _sapp_init_event(SAPP_EVENTTYPE_KEY_UP);
                        _sapp.event.key_code = k;
                        _sapp_call_event(&_sapp.event);
                    }
                }
            }
        }
        else {
            /* this was a backspace */
            _sapp_init_event(SAPP_EVENTTYPE_KEY_DOWN);
            _sapp.event.key_code = SAPP_KEYCODE_BACKSPACE;
            _sapp_call_event(&_sapp.event);
            _sapp_init_event(SAPP_EVENTTYPE_KEY_UP);
            _sapp.event.key_code = SAPP_KEYCODE_BACKSPACE;
            _sapp_call_event(&_sapp.event);
        }
    }
    return NO;
}
@end

@implementation _sapp_ios_view
- (void)drawRect:(CGRect)rect {
    _SOKOL_UNUSED(rect);
    _sapp_timing_measure(&_sapp.timing);
    @autoreleasepool {
        _sapp_ios_frame();
    }
}
- (BOOL)isOpaque {
    return YES;
}
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_BEGAN, touches, event);
}
- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_MOVED, touches, event);
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_ENDED, touches, event);
}
- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent*)event {
    _sapp_ios_touch_event(SAPP_EVENTTYPE_TOUCHES_CANCELLED, touches, event);
}
@end
