//  █████  ███    ██ ██████  ██████   ██████  ██ ██████
// ██   ██ ████   ██ ██   ██ ██   ██ ██    ██ ██ ██   ██
// ███████ ██ ██  ██ ██   ██ ██████  ██    ██ ██ ██   ██
// ██   ██ ██  ██ ██ ██   ██ ██   ██ ██    ██ ██ ██   ██
// ██   ██ ██   ████ ██████  ██   ██  ██████  ██ ██████
//
// >>android

/* android loop thread */
_SOKOL_PRIVATE bool _sapp_android_init_egl(void) {
    SOKOL_ASSERT(_sapp.android.display == EGL_NO_DISPLAY);
    SOKOL_ASSERT(_sapp.android.context == EGL_NO_CONTEXT);

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return false;
    }
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE) {
        return false;
    }
    EGLint alpha_size = _sapp.desc.alpha ? 8 : 0;
    const EGLint cfg_attributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, alpha_size,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE,
    };
    EGLConfig available_cfgs[32];
    EGLint cfg_count;
    eglChooseConfig(display, cfg_attributes, available_cfgs, 32, &cfg_count);
    SOKOL_ASSERT(cfg_count > 0);
    SOKOL_ASSERT(cfg_count <= 32);

    /* find config with 8-bit rgb buffer if available, ndk sample does not trust egl spec */
    EGLConfig config;
    bool exact_cfg_found = false;
    for (int i = 0; i < cfg_count; ++i) {
        EGLConfig c = available_cfgs[i];
        EGLint r, g, b, a, d;
        if (eglGetConfigAttrib(display, c, EGL_RED_SIZE, &r) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_GREEN_SIZE, &g) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_BLUE_SIZE, &b) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_ALPHA_SIZE, &a) == EGL_TRUE &&
            eglGetConfigAttrib(display, c, EGL_DEPTH_SIZE, &d) == EGL_TRUE &&
            r == 8 && g == 8 && b == 8 && (alpha_size == 0 || a == alpha_size) && d == 16) {
            exact_cfg_found = true;
            config = c;
            break;
        }
    }
    if (!exact_cfg_found) {
        config = available_cfgs[0];
    }

    EGLint ctx_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE,
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attributes);
    if (context == EGL_NO_CONTEXT) {
        return false;
    }

    _sapp.android.config = config;
    _sapp.android.display = display;
    _sapp.android.context = context;
    return true;
}

_SOKOL_PRIVATE void _sapp_android_cleanup_egl(void) {
    if (_sapp.android.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(_sapp.android.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (_sapp.android.surface != EGL_NO_SURFACE) {
            eglDestroySurface(_sapp.android.display, _sapp.android.surface);
            _sapp.android.surface = EGL_NO_SURFACE;
        }
        if (_sapp.android.context != EGL_NO_CONTEXT) {
            eglDestroyContext(_sapp.android.display, _sapp.android.context);
            _sapp.android.context = EGL_NO_CONTEXT;
        }
        eglTerminate(_sapp.android.display);
        _sapp.android.display = EGL_NO_DISPLAY;
    }
}

_SOKOL_PRIVATE bool _sapp_android_init_egl_surface(ANativeWindow* window) {
    SOKOL_ASSERT(_sapp.android.display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(_sapp.android.context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(_sapp.android.surface == EGL_NO_SURFACE);
    SOKOL_ASSERT(window);

    /* TODO: set window flags */
    /* ANativeActivity_setWindowFlags(activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0); */

    /* create egl surface and make it current */
    EGLSurface surface = eglCreateWindowSurface(_sapp.android.display, _sapp.android.config, window, NULL);
    if (surface == EGL_NO_SURFACE) {
        return false;
    }
    if (eglMakeCurrent(_sapp.android.display, surface, surface, _sapp.android.context) == EGL_FALSE) {
        return false;
    }
    _sapp.android.surface = surface;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&_sapp.gl.framebuffer);
    return true;
}

_SOKOL_PRIVATE void _sapp_android_cleanup_egl_surface(void) {
    if (_sapp.android.display == EGL_NO_DISPLAY) {
        return;
    }
    eglMakeCurrent(_sapp.android.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (_sapp.android.surface != EGL_NO_SURFACE) {
        eglDestroySurface(_sapp.android.display, _sapp.android.surface);
        _sapp.android.surface = EGL_NO_SURFACE;
    }
}

_SOKOL_PRIVATE void _sapp_android_app_event(sapp_event_type type) {
    if (_sapp_events_enabled()) {
        _sapp_init_event(type);
        _sapp_call_event(&_sapp.event);
    }
}

_SOKOL_PRIVATE void _sapp_android_update_dimensions(ANativeWindow* window, bool force_update) {
    SOKOL_ASSERT(_sapp.android.display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(_sapp.android.context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(_sapp.android.surface != EGL_NO_SURFACE);
    SOKOL_ASSERT(window);

    const int32_t win_w = ANativeWindow_getWidth(window);
    const int32_t win_h = ANativeWindow_getHeight(window);
    SOKOL_ASSERT(win_w >= 0 && win_h >= 0);
    const bool win_changed = (win_w != _sapp.window_width) || (win_h != _sapp.window_height);
    _sapp.window_width = win_w;
    _sapp.window_height = win_h;
    if (win_changed || force_update) {
        if (!_sapp.desc.high_dpi) {
            const int32_t buf_w = win_w / 2;
            const int32_t buf_h = win_h / 2;
            EGLint format;
            EGLBoolean egl_result = eglGetConfigAttrib(_sapp.android.display, _sapp.android.config, EGL_NATIVE_VISUAL_ID, &format);
            SOKOL_ASSERT(egl_result == EGL_TRUE); _SOKOL_UNUSED(egl_result);
            /* NOTE: calling ANativeWindow_setBuffersGeometry() with the same dimensions
                as the ANativeWindow size results in weird display artefacts, that's
                why it's only called when the buffer geometry is different from
                the window size
            */
            int32_t result = ANativeWindow_setBuffersGeometry(window, buf_w, buf_h, format);
            SOKOL_ASSERT(result == 0); _SOKOL_UNUSED(result);
        }
    }

    /* query surface size */
    EGLint fb_w, fb_h;
    EGLBoolean egl_result_w = eglQuerySurface(_sapp.android.display, _sapp.android.surface, EGL_WIDTH, &fb_w);
    EGLBoolean egl_result_h = eglQuerySurface(_sapp.android.display, _sapp.android.surface, EGL_HEIGHT, &fb_h);
    SOKOL_ASSERT(egl_result_w == EGL_TRUE); _SOKOL_UNUSED(egl_result_w);
    SOKOL_ASSERT(egl_result_h == EGL_TRUE); _SOKOL_UNUSED(egl_result_h);
    const bool fb_changed = (fb_w != _sapp.framebuffer_width) || (fb_h != _sapp.framebuffer_height);
    _sapp.framebuffer_width = fb_w;
    _sapp.framebuffer_height = fb_h;
    _sapp.dpi_scale = (float)_sapp.framebuffer_width / (float)_sapp.window_width;
    if (win_changed || fb_changed || force_update) {
        if (!_sapp.first_frame) {
            _sapp_android_app_event(SAPP_EVENTTYPE_RESIZED);
        }
    }
}

_SOKOL_PRIVATE void _sapp_android_cleanup(void) {
    if (_sapp.android.surface != EGL_NO_SURFACE) {
        /* egl context is bound, cleanup gracefully */
        if (_sapp.init_called && !_sapp.cleanup_called) {
            _sapp_call_cleanup();
        }
    }
    /* always try to cleanup by destroying egl context */
    _sapp_android_cleanup_egl();
}

_SOKOL_PRIVATE void _sapp_android_shutdown(void) {
    /* try to cleanup while we still have a surface and can call cleanup_cb() */
    _sapp_android_cleanup();
    /* request exit */
    ANativeActivity_finish(_sapp.android.activity);
}

_SOKOL_PRIVATE void _sapp_android_frame(void) {
    SOKOL_ASSERT(_sapp.android.display != EGL_NO_DISPLAY);
    SOKOL_ASSERT(_sapp.android.context != EGL_NO_CONTEXT);
    SOKOL_ASSERT(_sapp.android.surface != EGL_NO_SURFACE);
    _sapp_timing_measure(&_sapp.timing);
    _sapp_android_update_dimensions(_sapp.android.current.window, false);
    _sapp_frame();
    eglSwapBuffers(_sapp.android.display, _sapp.android.surface);
}

_SOKOL_PRIVATE bool _sapp_android_touch_event(const AInputEvent* e) {
    if (AInputEvent_getType(e) != AINPUT_EVENT_TYPE_MOTION) {
        return false;
    }
    if (!_sapp_events_enabled()) {
        return false;
    }
    int32_t action_idx = AMotionEvent_getAction(e);
    int32_t action = action_idx & AMOTION_EVENT_ACTION_MASK;
    sapp_event_type type = SAPP_EVENTTYPE_INVALID;
    switch (action) {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            type = SAPP_EVENTTYPE_TOUCHES_BEGAN;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            type = SAPP_EVENTTYPE_TOUCHES_MOVED;
            break;
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            type = SAPP_EVENTTYPE_TOUCHES_ENDED;
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            type = SAPP_EVENTTYPE_TOUCHES_CANCELLED;
            break;
        default:
            break;
    }
    if (type == SAPP_EVENTTYPE_INVALID) {
        return false;
    }
    int32_t idx = action_idx >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    _sapp_init_event(type);
    _sapp.event.num_touches = (int)AMotionEvent_getPointerCount(e);
    if (_sapp.event.num_touches > SAPP_MAX_TOUCHPOINTS) {
        _sapp.event.num_touches = SAPP_MAX_TOUCHPOINTS;
    }
    for (int32_t i = 0; i < _sapp.event.num_touches; i++) {
        sapp_touchpoint* dst = &_sapp.event.touches[i];
        dst->identifier = (uintptr_t)AMotionEvent_getPointerId(e, (size_t)i);
        dst->pos_x = (AMotionEvent_getX(e, (size_t)i) / _sapp.window_width) * _sapp.framebuffer_width;
        dst->pos_y = (AMotionEvent_getY(e, (size_t)i) / _sapp.window_height) * _sapp.framebuffer_height;
        dst->android_tooltype = (sapp_android_tooltype) AMotionEvent_getToolType(e, (size_t)i);
        if (action == AMOTION_EVENT_ACTION_POINTER_DOWN ||
            action == AMOTION_EVENT_ACTION_POINTER_UP) {
            dst->changed = (i == idx);
        } else {
            dst->changed = true;
        }
    }
    _sapp_call_event(&_sapp.event);
    return true;
}

_SOKOL_PRIVATE bool _sapp_android_key_event(const AInputEvent* e) {
    if (AInputEvent_getType(e) != AINPUT_EVENT_TYPE_KEY) {
        return false;
    }
    if (AKeyEvent_getKeyCode(e) == AKEYCODE_BACK) {
        /* FIXME: this should be hooked into a "really quit?" mechanism
           so the app can ask the user for confirmation, this is currently
           generally missing in sokol_app.h
        */
        _sapp_android_shutdown();
        return true;
    }
    return false;
}

_SOKOL_PRIVATE int _sapp_android_input_cb(int fd, int events, void* data) {
    _SOKOL_UNUSED(fd);
    _SOKOL_UNUSED(data);
    if ((events & ALOOPER_EVENT_INPUT) == 0) {
        _SAPP_ERROR(ANDROID_UNSUPPORTED_INPUT_EVENT_INPUT_CB);
        return 1;
    }
    SOKOL_ASSERT(_sapp.android.current.input);
    AInputEvent* event = NULL;
    while (AInputQueue_getEvent(_sapp.android.current.input, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(_sapp.android.current.input, event) != 0) {
            continue;
        }
        int32_t handled = 0;
        if (_sapp_android_touch_event(event) || _sapp_android_key_event(event)) {
            handled = 1;
        }
        AInputQueue_finishEvent(_sapp.android.current.input, event, handled);
    }
    return 1;
}

_SOKOL_PRIVATE int _sapp_android_main_cb(int fd, int events, void* data) {
    _SOKOL_UNUSED(data);
    if ((events & ALOOPER_EVENT_INPUT) == 0) {
        _SAPP_ERROR(ANDROID_UNSUPPORTED_INPUT_EVENT_MAIN_CB);
        return 1;
    }

    _sapp_android_msg_t msg;
    if (read(fd, &msg, sizeof(msg)) != sizeof(msg)) {
        _SAPP_ERROR(ANDROID_READ_MSG_FAILED);
        return 1;
    }

    pthread_mutex_lock(&_sapp.android.pt.mutex);
    switch (msg) {
        case _SOKOL_ANDROID_MSG_CREATE:
            {
                _SAPP_INFO(ANDROID_MSG_CREATE);
                SOKOL_ASSERT(!_sapp.valid);
                bool result = _sapp_android_init_egl();
                SOKOL_ASSERT(result); _SOKOL_UNUSED(result);
                _sapp.valid = true;
                _sapp.android.has_created = true;
            }
            break;
        case _SOKOL_ANDROID_MSG_RESUME:
            _SAPP_INFO(ANDROID_MSG_RESUME);
            _sapp.android.has_resumed = true;
            _sapp_android_app_event(SAPP_EVENTTYPE_RESUMED);
            break;
        case _SOKOL_ANDROID_MSG_PAUSE:
            _SAPP_INFO(ANDROID_MSG_PAUSE);
            _sapp.android.has_resumed = false;
            _sapp_android_app_event(SAPP_EVENTTYPE_SUSPENDED);
            break;
        case _SOKOL_ANDROID_MSG_FOCUS:
            _SAPP_INFO(ANDROID_MSG_FOCUS);
            _sapp.android.has_focus = true;
            break;
        case _SOKOL_ANDROID_MSG_NO_FOCUS:
            _SAPP_INFO(ANDROID_MSG_NO_FOCUS);
            _sapp.android.has_focus = false;
            break;
        case _SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW:
            _SAPP_INFO(ANDROID_MSG_SET_NATIVE_WINDOW);
            if (_sapp.android.current.window != _sapp.android.pending.window) {
                if (_sapp.android.current.window != NULL) {
                    _sapp_android_cleanup_egl_surface();
                }
                if (_sapp.android.pending.window != NULL) {
                    if (_sapp_android_init_egl_surface(_sapp.android.pending.window)) {
                        _sapp_android_update_dimensions(_sapp.android.pending.window, true);
                    } else {
                        _sapp_android_shutdown();
                    }
                }
            }
            _sapp.android.current.window = _sapp.android.pending.window;
            break;
        case _SOKOL_ANDROID_MSG_SET_INPUT_QUEUE:
            _SAPP_INFO(ANDROID_MSG_SET_INPUT_QUEUE);
            if (_sapp.android.current.input != _sapp.android.pending.input) {
                if (_sapp.android.current.input != NULL) {
                    AInputQueue_detachLooper(_sapp.android.current.input);
                }
                if (_sapp.android.pending.input != NULL) {
                    AInputQueue_attachLooper(
                        _sapp.android.pending.input,
                        _sapp.android.looper,
                        ALOOPER_POLL_CALLBACK,
                        _sapp_android_input_cb,
                        NULL); /* data */
                }
            }
            _sapp.android.current.input = _sapp.android.pending.input;
            break;
        case _SOKOL_ANDROID_MSG_DESTROY:
            _SAPP_INFO(ANDROID_MSG_DESTROY);
            _sapp_android_cleanup();
            _sapp.valid = false;
            _sapp.android.is_thread_stopping = true;
            break;
        default:
            _SAPP_WARN(ANDROID_UNKNOWN_MSG);
            break;
    }
    pthread_cond_broadcast(&_sapp.android.pt.cond); /* signal "received" */
    pthread_mutex_unlock(&_sapp.android.pt.mutex);
    return 1;
}

_SOKOL_PRIVATE bool _sapp_android_should_update(void) {
    bool is_in_front = _sapp.android.has_resumed && _sapp.android.has_focus;
    bool has_surface = _sapp.android.surface != EGL_NO_SURFACE;
    return is_in_front && has_surface;
}

_SOKOL_PRIVATE void _sapp_android_show_keyboard(bool shown) {
    SOKOL_ASSERT(_sapp.valid);
    /* This seems to be broken in the NDK, but there is (a very cumbersome) workaround... */
    if (shown) {
        ANativeActivity_showSoftInput(_sapp.android.activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED);
    } else {
        ANativeActivity_hideSoftInput(_sapp.android.activity, ANATIVEACTIVITY_HIDE_SOFT_INPUT_NOT_ALWAYS);
    }
}

_SOKOL_PRIVATE void* _sapp_android_loop(void* arg) {
    _SOKOL_UNUSED(arg);
    _SAPP_INFO(ANDROID_LOOP_THREAD_STARTED);

    _sapp.android.looper = ALooper_prepare(0 /* or ALOOPER_PREPARE_ALLOW_NON_CALLBACKS*/);
    ALooper_addFd(_sapp.android.looper,
        _sapp.android.pt.read_from_main_fd,
        ALOOPER_POLL_CALLBACK,
        ALOOPER_EVENT_INPUT,
        _sapp_android_main_cb,
        NULL); /* data */

    /* signal start to main thread */
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp.android.is_thread_started = true;
    pthread_cond_broadcast(&_sapp.android.pt.cond);
    pthread_mutex_unlock(&_sapp.android.pt.mutex);

    /* main loop */
    while (!_sapp.android.is_thread_stopping) {
        /* sokol frame */
        if (_sapp_android_should_update()) {
            _sapp_android_frame();
        }

        /* process all events (or stop early if app is requested to quit) */
        bool process_events = true;
        while (process_events && !_sapp.android.is_thread_stopping) {
            bool block_until_event = !_sapp.android.is_thread_stopping && !_sapp_android_should_update();
            process_events = ALooper_pollOnce(block_until_event ? -1 : 0, NULL, NULL, NULL) == ALOOPER_POLL_CALLBACK;
        }
    }

    /* cleanup thread */
    if (_sapp.android.current.input != NULL) {
        AInputQueue_detachLooper(_sapp.android.current.input);
    }

    /* the following causes heap corruption on exit, why??
    ALooper_removeFd(_sapp.android.looper, _sapp.android.pt.read_from_main_fd);
    ALooper_release(_sapp.android.looper);*/

    /* signal "destroyed" */
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp.android.is_thread_stopped = true;
    pthread_cond_broadcast(&_sapp.android.pt.cond);
    pthread_mutex_unlock(&_sapp.android.pt.mutex);

    _SAPP_INFO(ANDROID_LOOP_THREAD_DONE);
    return NULL;
}

/* android main/ui thread */
_SOKOL_PRIVATE void _sapp_android_msg(_sapp_android_msg_t msg) {
    if (write(_sapp.android.pt.write_from_main_fd, &msg, sizeof(msg)) != sizeof(msg)) {
        _SAPP_ERROR(ANDROID_WRITE_MSG_FAILED);
    }
}

_SOKOL_PRIVATE void _sapp_android_on_start(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONSTART);
}

_SOKOL_PRIVATE void _sapp_android_on_resume(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONRESUME);
    _sapp_android_msg(_SOKOL_ANDROID_MSG_RESUME);
}

_SOKOL_PRIVATE void* _sapp_android_on_save_instance_state(ANativeActivity* activity, size_t* out_size) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONSAVEINSTANCESTATE);
    *out_size = 0;
    return NULL;
}

_SOKOL_PRIVATE void _sapp_android_on_window_focus_changed(ANativeActivity* activity, int has_focus) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONWINDOWFOCUSCHANGED);
    if (has_focus) {
        _sapp_android_msg(_SOKOL_ANDROID_MSG_FOCUS);
    } else {
        _sapp_android_msg(_SOKOL_ANDROID_MSG_NO_FOCUS);
    }
}

_SOKOL_PRIVATE void _sapp_android_on_pause(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONPAUSE);
    _sapp_android_msg(_SOKOL_ANDROID_MSG_PAUSE);
}

_SOKOL_PRIVATE void _sapp_android_on_stop(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONSTOP);
}

_SOKOL_PRIVATE void _sapp_android_msg_set_native_window(ANativeWindow* window) {
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp.android.pending.window = window;
    _sapp_android_msg(_SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW);
    while (_sapp.android.current.window != window) {
        pthread_cond_wait(&_sapp.android.pt.cond, &_sapp.android.pt.mutex);
    }
    pthread_mutex_unlock(&_sapp.android.pt.mutex);
}

_SOKOL_PRIVATE void _sapp_android_on_native_window_created(ANativeActivity* activity, ANativeWindow* window) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONNATIVEWINDOWCREATED);
    _sapp_android_msg_set_native_window(window);
}

_SOKOL_PRIVATE void _sapp_android_on_native_window_destroyed(ANativeActivity* activity, ANativeWindow* window) {
    _SOKOL_UNUSED(activity);
    _SOKOL_UNUSED(window);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONNATIVEWINDOWDESTROYED);
    _sapp_android_msg_set_native_window(NULL);
}

_SOKOL_PRIVATE void _sapp_android_msg_set_input_queue(AInputQueue* input) {
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp.android.pending.input = input;
    _sapp_android_msg(_SOKOL_ANDROID_MSG_SET_INPUT_QUEUE);
    while (_sapp.android.current.input != input) {
        pthread_cond_wait(&_sapp.android.pt.cond, &_sapp.android.pt.mutex);
    }
    pthread_mutex_unlock(&_sapp.android.pt.mutex);
}

_SOKOL_PRIVATE void _sapp_android_on_input_queue_created(ANativeActivity* activity, AInputQueue* queue) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONINPUTQUEUECREATED);
    _sapp_android_msg_set_input_queue(queue);
}

_SOKOL_PRIVATE void _sapp_android_on_input_queue_destroyed(ANativeActivity* activity, AInputQueue* queue) {
    _SOKOL_UNUSED(activity);
    _SOKOL_UNUSED(queue);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONINPUTQUEUEDESTROYED);
    _sapp_android_msg_set_input_queue(NULL);
}

_SOKOL_PRIVATE void _sapp_android_on_config_changed(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONCONFIGURATIONCHANGED);
    /* see android:configChanges in manifest */
}

_SOKOL_PRIVATE void _sapp_android_on_low_memory(ANativeActivity* activity) {
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONLOWMEMORY);
}

_SOKOL_PRIVATE void _sapp_android_on_destroy(ANativeActivity* activity) {
    /*
     * For some reason even an empty app using nativeactivity.h will crash (WIN DEATH)
     * on my device (Moto X 2nd gen) when the app is removed from the task view
     * (TaskStackView: onTaskViewDismissed).
     *
     * However, if ANativeActivity_finish() is explicitly called from for example
     * _sapp_android_on_stop(), the crash disappears. Is this a bug in NativeActivity?
     */
    _SOKOL_UNUSED(activity);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONDESTROY);

    /* send destroy msg */
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp_android_msg(_SOKOL_ANDROID_MSG_DESTROY);
    while (!_sapp.android.is_thread_stopped) {
        pthread_cond_wait(&_sapp.android.pt.cond, &_sapp.android.pt.mutex);
    }
    pthread_mutex_unlock(&_sapp.android.pt.mutex);

    /* clean up main thread */
    pthread_cond_destroy(&_sapp.android.pt.cond);
    pthread_mutex_destroy(&_sapp.android.pt.mutex);

    close(_sapp.android.pt.read_from_main_fd);
    close(_sapp.android.pt.write_from_main_fd);

    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_DONE);

    /* this is a bit naughty, but causes a clean restart of the app (static globals are reset) */
    exit(0);
}

JNIEXPORT
void ANativeActivity_onCreate(ANativeActivity* activity, void* saved_state, size_t saved_state_size) {
    _SOKOL_UNUSED(saved_state);
    _SOKOL_UNUSED(saved_state_size);
    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_ONCREATE);

    // the NativeActity pointer needs to be available inside sokol_main()
    // (see https://github.com/floooh/sokol/issues/708), however _sapp_init_state()
    // will clear the global _sapp_t struct, so we need to initialize the native
    // activity pointer twice, once before sokol_main() and once after _sapp_init_state()
    _sapp_clear(&_sapp, sizeof(_sapp));
    _sapp.android.activity = activity;
    sapp_desc desc = sokol_main(0, NULL);
    _sapp_init_state(&desc);
    _sapp.android.activity = activity;

    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) {
        _SAPP_ERROR(ANDROID_CREATE_THREAD_PIPE_FAILED);
        return;
    }
    _sapp.android.pt.read_from_main_fd = pipe_fd[0];
    _sapp.android.pt.write_from_main_fd = pipe_fd[1];

    pthread_mutex_init(&_sapp.android.pt.mutex, NULL);
    pthread_cond_init(&_sapp.android.pt.cond, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&_sapp.android.pt.thread, &attr, _sapp_android_loop, 0);
    pthread_attr_destroy(&attr);

    /* wait until main loop has started */
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    while (!_sapp.android.is_thread_started) {
        pthread_cond_wait(&_sapp.android.pt.cond, &_sapp.android.pt.mutex);
    }
    pthread_mutex_unlock(&_sapp.android.pt.mutex);

    /* send create msg */
    pthread_mutex_lock(&_sapp.android.pt.mutex);
    _sapp_android_msg(_SOKOL_ANDROID_MSG_CREATE);
    while (!_sapp.android.has_created) {
        pthread_cond_wait(&_sapp.android.pt.cond, &_sapp.android.pt.mutex);
    }
    pthread_mutex_unlock(&_sapp.android.pt.mutex);

    /* register for callbacks */
    activity->callbacks->onStart = _sapp_android_on_start;
    activity->callbacks->onResume = _sapp_android_on_resume;
    activity->callbacks->onSaveInstanceState = _sapp_android_on_save_instance_state;
    activity->callbacks->onWindowFocusChanged = _sapp_android_on_window_focus_changed;
    activity->callbacks->onPause = _sapp_android_on_pause;
    activity->callbacks->onStop = _sapp_android_on_stop;
    activity->callbacks->onDestroy = _sapp_android_on_destroy;
    activity->callbacks->onNativeWindowCreated = _sapp_android_on_native_window_created;
    /* activity->callbacks->onNativeWindowResized = _sapp_android_on_native_window_resized; */
    /* activity->callbacks->onNativeWindowRedrawNeeded = _sapp_android_on_native_window_redraw_needed; */
    activity->callbacks->onNativeWindowDestroyed = _sapp_android_on_native_window_destroyed;
    activity->callbacks->onInputQueueCreated = _sapp_android_on_input_queue_created;
    activity->callbacks->onInputQueueDestroyed = _sapp_android_on_input_queue_destroyed;
    /* activity->callbacks->onContentRectChanged = _sapp_android_on_content_rect_changed; */
    activity->callbacks->onConfigurationChanged = _sapp_android_on_config_changed;
    activity->callbacks->onLowMemory = _sapp_android_on_low_memory;

    _SAPP_INFO(ANDROID_NATIVE_ACTIVITY_CREATE_SUCCESS);

    /* NOT A BUG: do NOT call sapp_discard_state() */
}
