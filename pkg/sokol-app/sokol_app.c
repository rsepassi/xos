#include "sokol_app.h"

// ██ ███    ███ ██████  ██      ███████ ███    ███ ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████
//
// >>implementation
#ifndef SOKOL_APP_IMPL
#define SOKOL_APP_IMPL

#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <stddef.h> // size_t
#include <math.h>   // roundf

// helper macros
#define _sapp_def(val, def) (((val) == 0) ? (def) : (val))
#define _sapp_absf(a) (((a)<0.0f)?-(a):(a))

#define _SAPP_MAX_TITLE_LENGTH (128)
#define _SAPP_FALLBACK_DEFAULT_WINDOW_WIDTH (640)
#define _SAPP_FALLBACK_DEFAULT_WINDOW_HEIGHT (480)
// NOTE: the pixel format values *must* be compatible with sg_pixel_format
#define _SAPP_PIXELFORMAT_RGBA8 (23)
#define _SAPP_PIXELFORMAT_BGRA8 (28)
#define _SAPP_PIXELFORMAT_DEPTH (43)
#define _SAPP_PIXELFORMAT_DEPTH_STENCIL (44)

// check if the config defines are alright
#if defined(__APPLE__)
    // see https://clang.llvm.org/docs/LanguageExtensions.html#automatic-reference-counting
    #if !defined(__cplusplus)
        #if __has_feature(objc_arc) && !__has_feature(objc_arc_fields)
            #error "sokol_app.h requires __has_feature(objc_arc_field) if ARC is enabled (use a more recent compiler version)"
        #endif
    #endif
    #define _SAPP_APPLE (1)
    #include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
        /* MacOS */
        #define _SAPP_MACOS (1)
        #if !defined(SOKOL_METAL)
        #error("sokol_app.h: unknown 3D API selected for MacOS, must be SOKOL_METAL")
        #endif
    #else
        /* iOS or iOS Simulator */
        #define _SAPP_IOS (1)
        #if !defined(SOKOL_METAL) && !defined(SOKOL_GLES3)
        #error("sokol_app.h: unknown 3D API selected for iOS, must be SOKOL_METAL or SOKOL_GLES3")
        #endif
    #endif
#elif defined(_WIN32)
    /* Windows (D3D11 or GL) */
    #define _SAPP_WIN32 (1)
    #if !defined(SOKOL_D3D11) && !defined(SOKOL_NOAPI)
    #error("sokol_app.h: unknown 3D API selected for Win32, must be SOKOL_D3D11, or SOKOL_NOAPI")
    #endif
#elif defined(__ANDROID__)
    /* Android */
    #define _SAPP_ANDROID (1)
    #if !defined(SOKOL_GLES3)
    #error("sokol_app.h: unknown 3D API selected for Android, must be SOKOL_GLES3")
    #endif
    #if defined(SOKOL_NO_ENTRY)
    #error("sokol_app.h: SOKOL_NO_ENTRY is not supported on Android")
    #endif
#elif defined(__linux__) || defined(__unix__)
    /* Linux */
    #define _SAPP_LINUX (1)
    #if defined(SOKOL_GLES3)
        #include <GLES3/gl3.h>
        #include <GLES3/gl3ext.h>
    #else
        #error("sokol_app.h: unknown 3D API selected for Linux, must be SOKOL_GLES3")
    #endif
#else
#error "sokol_app.h: Unknown platform"
#endif

#if defined(SOKOL_GLES3)
    #define _SAPP_ANY_GL (1)
#endif

#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_DEBUG
    #ifndef NDEBUG
        #define SOKOL_DEBUG
    #endif
#endif
#ifndef SOKOL_ASSERT
    #include <assert.h>
    #define SOKOL_ASSERT(c) assert(c)
#endif
#ifndef SOKOL_UNREACHABLE
    #define SOKOL_UNREACHABLE SOKOL_ASSERT(false)
#endif

#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif
#ifndef _SOKOL_UNUSED
    #define _SOKOL_UNUSED(x) (void)(x)
#endif

#if defined(_SAPP_APPLE)
    #if defined(SOKOL_METAL)
        #import <Metal/Metal.h>
        #import <MetalKit/MetalKit.h>
    #endif
    #if defined(_SAPP_MACOS)
    #elif defined(_SAPP_IOS)
        #import <UIKit/UIKit.h>
    #endif
    #include <AvailabilityMacros.h>
    #include <mach/mach_time.h>
#elif defined(_SAPP_WIN32)
    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable:4201)   /* nonstandard extension used: nameless struct/union */
        #pragma warning(disable:4204)   /* nonstandard extension used: non-constant aggregate initializer */
        #pragma warning(disable:4054)   /* 'type cast': from function pointer */
        #pragma warning(disable:4055)   /* 'type cast': from data pointer */
        #pragma warning(disable:4505)   /* unreferenced local function has been removed */
        #pragma warning(disable:4115)   /* /W4: 'ID3D11ModuleInstance': named type definition in parentheses (in d3d11.h) */
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <windowsx.h>
    #include <shellapi.h>
    #if !defined(SOKOL_NO_ENTRY)    // if SOKOL_NO_ENTRY is defined, it's the applications' responsibility to use the right subsystem
        #if defined(SOKOL_WIN32_FORCE_MAIN)
            #pragma comment (linker, "/subsystem:console")
        #else
            #pragma comment (linker, "/subsystem:windows")
        #endif
    #endif
    #include <stdio.h>  /* freopen_s() */
    #include <wchar.h>  /* wcslen() */

    #pragma comment (lib, "kernel32")
    #pragma comment (lib, "user32")
    #pragma comment (lib, "shell32")    /* CommandLineToArgvW, DragQueryFileW, DragFinished */
    #pragma comment (lib, "gdi32")
    #if defined(SOKOL_D3D11)
        #pragma comment (lib, "dxgi")
        #pragma comment (lib, "d3d11")
    #endif

    #if defined(SOKOL_D3D11)
        #ifndef D3D11_NO_HELPERS
            #define D3D11_NO_HELPERS
        #endif
        #include <d3d11.h>
        #include <dxgi.h>
        // DXGI_SWAP_EFFECT_FLIP_DISCARD is only defined in newer Windows SDKs, so don't depend on it
        #define _SAPP_DXGI_SWAP_EFFECT_FLIP_DISCARD (4)
    #endif
    #ifndef WM_MOUSEHWHEEL /* see https://github.com/floooh/sokol/issues/138 */
        #define WM_MOUSEHWHEEL (0x020E)
    #endif
    #ifndef WM_DPICHANGED
        #define WM_DPICHANGED (0x02E0)
    #endif
#elif defined(_SAPP_ANDROID)
    #include <pthread.h>
    #include <unistd.h>
    #include <time.h>
    #include <android/native_activity.h>
    #include <android/looper.h>
    #include <EGL/egl.h>
    #include <GLES3/gl3.h>
#elif defined(_SAPP_LINUX)
    #define GL_GLEXT_PROTOTYPES
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
    #include <X11/Xresource.h>
    #include <X11/Xatom.h>
    #include <X11/extensions/XInput2.h>
    #include <X11/Xcursor/Xcursor.h>
    #include <X11/cursorfont.h> /* XC_* font cursors */
    #include <X11/Xmd.h> /* CARD32 */
    #if !defined(_SAPP_GLX)
        #include <EGL/egl.h>
    #endif
    #include <dlfcn.h> /* dlopen, dlsym, dlclose */
    #include <limits.h> /* LONG_MAX */
    #include <pthread.h>    /* only used a linker-guard, search for _sapp_linux_run() and see first comment */
    #include <time.h>
#endif

#if defined(_SAPP_APPLE)
    // this is ARC compatible
    #if defined(__cplusplus)
        #define _SAPP_CLEAR_ARC_STRUCT(type, item) { item = type(); }
    #else
        #define _SAPP_CLEAR_ARC_STRUCT(type, item) { item = (type) { 0 }; }
    #endif
#else
    #define _SAPP_CLEAR_ARC_STRUCT(type, item) { _sapp_clear(&item, sizeof(item)); }
#endif


// ███████ ██████   █████  ███    ███ ███████     ████████ ██ ███    ███ ██ ███    ██  ██████
// ██      ██   ██ ██   ██ ████  ████ ██             ██    ██ ████  ████ ██ ████   ██ ██
// █████   ██████  ███████ ██ ████ ██ █████          ██    ██ ██ ████ ██ ██ ██ ██  ██ ██   ███
// ██      ██   ██ ██   ██ ██  ██  ██ ██             ██    ██ ██  ██  ██ ██ ██  ██ ██ ██    ██
// ██      ██   ██ ██   ██ ██      ██ ███████        ██    ██ ██      ██ ██ ██   ████  ██████
//
// >>frame timing
#define _SAPP_RING_NUM_SLOTS (256)
typedef struct {
    int head;
    int tail;
    double buf[_SAPP_RING_NUM_SLOTS];
} _sapp_ring_t;

_SOKOL_PRIVATE int _sapp_ring_idx(int i) {
    return i % _SAPP_RING_NUM_SLOTS;
}

_SOKOL_PRIVATE void _sapp_ring_init(_sapp_ring_t* ring) {
    ring->head = 0;
    ring->tail = 0;
}

_SOKOL_PRIVATE bool _sapp_ring_full(_sapp_ring_t* ring) {
    return _sapp_ring_idx(ring->head + 1) == ring->tail;
}

_SOKOL_PRIVATE bool _sapp_ring_empty(_sapp_ring_t* ring) {
    return ring->head == ring->tail;
}

_SOKOL_PRIVATE int _sapp_ring_count(_sapp_ring_t* ring) {
    int count;
    if (ring->head >= ring->tail) {
        count = ring->head - ring->tail;
    }
    else {
        count = (ring->head + _SAPP_RING_NUM_SLOTS) - ring->tail;
    }
    SOKOL_ASSERT((count >= 0) && (count < _SAPP_RING_NUM_SLOTS));
    return count;
}

_SOKOL_PRIVATE void _sapp_ring_enqueue(_sapp_ring_t* ring, double val) {
    SOKOL_ASSERT(!_sapp_ring_full(ring));
    ring->buf[ring->head] = val;
    ring->head = _sapp_ring_idx(ring->head + 1);
}

_SOKOL_PRIVATE double _sapp_ring_dequeue(_sapp_ring_t* ring) {
    SOKOL_ASSERT(!_sapp_ring_empty(ring));
    double val = ring->buf[ring->tail];
    ring->tail = _sapp_ring_idx(ring->tail + 1);
    return val;
}

/*
    NOTE:

    Q: Why not use CAMetalDrawable.presentedTime on macOS and iOS?
    A: The value appears to be highly unstable during the first few
    seconds, sometimes several frames are dropped in sequence, or
    switch between 120 and 60 Hz for a few frames. Simply measuring
    and averaging the frame time yielded a more stable frame duration.
    Maybe switching to CVDisplayLink would yield better results.
    Until then just measure the time.
*/
typedef struct {
    #if defined(_SAPP_APPLE)
        struct {
            mach_timebase_info_data_t timebase;
            uint64_t start;
        } mach;
    #elif defined(_SAPP_WIN32)
        struct {
            LARGE_INTEGER freq;
            LARGE_INTEGER start;
        } win;
    #else // Linux, Android, ...
        #ifdef CLOCK_MONOTONIC
        #define _SAPP_CLOCK_MONOTONIC CLOCK_MONOTONIC
        #else
        // on some embedded platforms, CLOCK_MONOTONIC isn't defined
        #define _SAPP_CLOCK_MONOTONIC (1)
        #endif
        struct {
            uint64_t start;
        } posix;
    #endif
} _sapp_timestamp_t;

_SOKOL_PRIVATE int64_t _sapp_int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}

_SOKOL_PRIVATE void _sapp_timestamp_init(_sapp_timestamp_t* ts) {
    #if defined(_SAPP_APPLE)
        mach_timebase_info(&ts->mach.timebase);
        ts->mach.start = mach_absolute_time();
    #elif defined(_SAPP_WIN32)
        QueryPerformanceFrequency(&ts->win.freq);
        QueryPerformanceCounter(&ts->win.start);
    #else
        struct timespec tspec;
        clock_gettime(_SAPP_CLOCK_MONOTONIC, &tspec);
        ts->posix.start = (uint64_t)tspec.tv_sec*1000000000 + (uint64_t)tspec.tv_nsec;
    #endif
}

_SOKOL_PRIVATE double _sapp_timestamp_now(_sapp_timestamp_t* ts) {
    #if defined(_SAPP_APPLE)
        const uint64_t traw = mach_absolute_time() - ts->mach.start;
        const uint64_t now = (uint64_t) _sapp_int64_muldiv((int64_t)traw, (int64_t)ts->mach.timebase.numer, (int64_t)ts->mach.timebase.denom);
        return (double)now / 1000000000.0;
    #elif defined(_SAPP_WIN32)
        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        const uint64_t now = (uint64_t)_sapp_int64_muldiv(qpc.QuadPart - ts->win.start.QuadPart, 1000000000, ts->win.freq.QuadPart);
        return (double)now / 1000000000.0;
    #else
        struct timespec tspec;
        clock_gettime(_SAPP_CLOCK_MONOTONIC, &tspec);
        const uint64_t now = ((uint64_t)tspec.tv_sec*1000000000 + (uint64_t)tspec.tv_nsec) - ts->posix.start;
        return (double)now / 1000000000.0;
    #endif
}

typedef struct {
    double last;
    double accum;
    double avg;
    int spike_count;
    int num;
    _sapp_timestamp_t timestamp;
    _sapp_ring_t ring;
} _sapp_timing_t;

_SOKOL_PRIVATE void _sapp_timing_reset(_sapp_timing_t* t) {
    t->last = 0.0;
    t->accum = 0.0;
    t->spike_count = 0;
    t->num = 0;
    _sapp_ring_init(&t->ring);
}

_SOKOL_PRIVATE void _sapp_timing_init(_sapp_timing_t* t) {
    t->avg = 1.0 / 60.0;    // dummy value until first actual value is available
    _sapp_timing_reset(t);
    _sapp_timestamp_init(&t->timestamp);
}

_SOKOL_PRIVATE void _sapp_timing_put(_sapp_timing_t* t, double dur) {
    // arbitrary upper limit to ignore outliers (e.g. during window resizing, or debugging)
    double min_dur = 0.0;
    double max_dur = 0.1;
    // if we have enough samples for a useful average, use a much tighter 'valid window'
    if (_sapp_ring_full(&t->ring)) {
        min_dur = t->avg * 0.8;
        max_dur = t->avg * 1.2;
    }
    if ((dur < min_dur) || (dur > max_dur)) {
        t->spike_count++;
        // if there have been many spikes in a row, the display refresh rate
        // might have changed, so a timing reset is needed
        if (t->spike_count > 20) {
            _sapp_timing_reset(t);
        }
        return;
    }
    if (_sapp_ring_full(&t->ring)) {
        double old_val = _sapp_ring_dequeue(&t->ring);
        t->accum -= old_val;
        t->num -= 1;
    }
    _sapp_ring_enqueue(&t->ring, dur);
    t->accum += dur;
    t->num += 1;
    SOKOL_ASSERT(t->num > 0);
    t->avg = t->accum / t->num;
    t->spike_count = 0;
}

_SOKOL_PRIVATE void _sapp_timing_discontinuity(_sapp_timing_t* t) {
    t->last = 0.0;
}

_SOKOL_PRIVATE void _sapp_timing_measure(_sapp_timing_t* t) {
    const double now = _sapp_timestamp_now(&t->timestamp);
    if (t->last > 0.0) {
        double dur = now - t->last;
        _sapp_timing_put(t, dur);
    }
    t->last = now;
}

_SOKOL_PRIVATE void _sapp_timing_external(_sapp_timing_t* t, double now) {
    if (t->last > 0.0) {
        double dur = now - t->last;
        _sapp_timing_put(t, dur);
    }
    t->last = now;
}

_SOKOL_PRIVATE double _sapp_timing_get_avg(_sapp_timing_t* t) {
    return t->avg;
}

// ███████ ████████ ██████  ██    ██  ██████ ████████ ███████
// ██         ██    ██   ██ ██    ██ ██         ██    ██
// ███████    ██    ██████  ██    ██ ██         ██    ███████
//      ██    ██    ██   ██ ██    ██ ██         ██         ██
// ███████    ██    ██   ██  ██████   ██████    ██    ███████
//
// >> structs
#if defined(_SAPP_MACOS)
@interface _sapp_macos_app_delegate : NSObject<NSApplicationDelegate>
@end
@interface _sapp_macos_window : NSWindow
@end
@interface _sapp_macos_window_delegate : NSObject<NSWindowDelegate>
@end
#if defined(SOKOL_METAL)
    @interface _sapp_macos_view : MTKView
    @end
#endif

typedef struct {
    uint32_t flags_changed_store;
    uint8_t mouse_buttons;
    NSWindow* window;
    NSTrackingArea* tracking_area;
    id keyup_monitor;
    _sapp_macos_app_delegate* app_dlg;
    _sapp_macos_window_delegate* win_dlg;
    _sapp_macos_view* view;
    NSCursor* cursors[_SAPP_MOUSECURSOR_NUM];
    #if defined(SOKOL_METAL)
        id<MTLDevice> mtl_device;
    #endif
} _sapp_macos_t;

#endif // _SAPP_MACOS

#if defined(_SAPP_IOS)

@interface _sapp_app_delegate : NSObject<UIApplicationDelegate>
@end
@interface _sapp_textfield_dlg : NSObject<UITextFieldDelegate>
- (void)keyboardWasShown:(NSNotification*)notif;
- (void)keyboardWillBeHidden:(NSNotification*)notif;
- (void)keyboardDidChangeFrame:(NSNotification*)notif;
@end
#if defined(SOKOL_METAL)
    @interface _sapp_ios_view : MTKView;
    @end
#else
    @interface _sapp_ios_view : GLKView
    @end
#endif

typedef struct {
    UIWindow* window;
    _sapp_ios_view* view;
    UITextField* textfield;
    _sapp_textfield_dlg* textfield_dlg;
    #if defined(SOKOL_METAL)
        UIViewController* view_ctrl;
        id<MTLDevice> mtl_device;
    #else
        GLKViewController* view_ctrl;
        EAGLContext* eagl_ctx;
    #endif
    bool suspended;
} _sapp_ios_t;

#endif // _SAPP_IOS

#if defined(SOKOL_D3D11) && defined(_SAPP_WIN32)
typedef struct {
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    ID3D11Texture2D* rt;
    ID3D11RenderTargetView* rtv;
    ID3D11Texture2D* msaa_rt;
    ID3D11RenderTargetView* msaa_rtv;
    ID3D11Texture2D* ds;
    ID3D11DepthStencilView* dsv;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    IDXGISwapChain* swap_chain;
    IDXGIDevice1* dxgi_device;
    bool use_dxgi_frame_stats;
    UINT sync_refresh_count;
} _sapp_d3d11_t;
#endif

#if defined(_SAPP_WIN32)

#ifndef DPI_ENUMS_DECLARED
typedef enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
#endif // DPI_ENUMS_DECLARED

typedef struct {
    bool aware;
    float content_scale;
    float window_scale;
    float mouse_scale;
} _sapp_win32_dpi_t;

typedef struct {
    HWND hwnd;
    HMONITOR hmonitor;
    HDC dc;
    HICON big_icon;
    HICON small_icon;
    HCURSOR cursors[_SAPP_MOUSECURSOR_NUM];
    UINT orig_codepage;
    LONG mouse_locked_x, mouse_locked_y;
    RECT stored_window_rect;    // used to restore window pos/size when toggling fullscreen => windowed
    bool is_win10_or_greater;
    bool in_create_window;
    bool iconified;
    bool mouse_tracked;
    uint8_t mouse_capture_mask;
    _sapp_win32_dpi_t dpi;
    bool raw_input_mousepos_valid;
    LONG raw_input_mousepos_x;
    LONG raw_input_mousepos_y;
    uint8_t raw_input_data[256];
} _sapp_win32_t;

#endif // _SAPP_WIN32

#if defined(_SAPP_ANDROID)
typedef enum {
    _SOKOL_ANDROID_MSG_CREATE,
    _SOKOL_ANDROID_MSG_RESUME,
    _SOKOL_ANDROID_MSG_PAUSE,
    _SOKOL_ANDROID_MSG_FOCUS,
    _SOKOL_ANDROID_MSG_NO_FOCUS,
    _SOKOL_ANDROID_MSG_SET_NATIVE_WINDOW,
    _SOKOL_ANDROID_MSG_SET_INPUT_QUEUE,
    _SOKOL_ANDROID_MSG_DESTROY,
} _sapp_android_msg_t;

typedef struct {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int read_from_main_fd;
    int write_from_main_fd;
} _sapp_android_pt_t;

typedef struct {
    ANativeWindow* window;
    AInputQueue* input;
} _sapp_android_resources_t;

typedef struct {
    ANativeActivity* activity;
    _sapp_android_pt_t pt;
    _sapp_android_resources_t pending;
    _sapp_android_resources_t current;
    ALooper* looper;
    bool is_thread_started;
    bool is_thread_stopping;
    bool is_thread_stopped;
    bool has_created;
    bool has_resumed;
    bool has_focus;
    EGLConfig config;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
} _sapp_android_t;

#endif // _SAPP_ANDROID

#if defined(_SAPP_LINUX)

#define _SAPP_X11_XDND_VERSION (5)

#define GLX_VENDOR 1
#define GLX_RGBA_BIT 0x00000001
#define GLX_WINDOW_BIT 0x00000001
#define GLX_DRAWABLE_TYPE 0x8010
#define GLX_RENDER_TYPE	0x8011
#define GLX_DOUBLEBUFFER 5
#define GLX_RED_SIZE 8
#define GLX_GREEN_SIZE 9
#define GLX_BLUE_SIZE 10
#define GLX_ALPHA_SIZE 11
#define GLX_DEPTH_SIZE 12
#define GLX_STENCIL_SIZE 13
#define GLX_SAMPLES 0x186a1
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_FLAGS_ARB 0x2094

typedef XID GLXWindow;
typedef XID GLXDrawable;
typedef struct __GLXFBConfig* GLXFBConfig;
typedef struct __GLXcontext* GLXContext;
typedef void (*__GLXextproc)(void);

typedef int (*PFNGLXGETFBCONFIGATTRIBPROC)(Display*,GLXFBConfig,int,int*);
typedef const char* (*PFNGLXGETCLIENTSTRINGPROC)(Display*,int);
typedef Bool (*PFNGLXQUERYEXTENSIONPROC)(Display*,int*,int*);
typedef Bool (*PFNGLXQUERYVERSIONPROC)(Display*,int*,int*);
typedef void (*PFNGLXDESTROYCONTEXTPROC)(Display*,GLXContext);
typedef Bool (*PFNGLXMAKECURRENTPROC)(Display*,GLXDrawable,GLXContext);
typedef void (*PFNGLXSWAPBUFFERSPROC)(Display*,GLXDrawable);
typedef const char* (*PFNGLXQUERYEXTENSIONSSTRINGPROC)(Display*,int);
typedef GLXFBConfig* (*PFNGLXGETFBCONFIGSPROC)(Display*,int,int*);
typedef __GLXextproc (* PFNGLXGETPROCADDRESSPROC)(const char *procName);
typedef void (*PFNGLXSWAPINTERVALEXTPROC)(Display*,GLXDrawable,int);
typedef XVisualInfo* (*PFNGLXGETVISUALFROMFBCONFIGPROC)(Display*,GLXFBConfig);
typedef GLXWindow (*PFNGLXCREATEWINDOWPROC)(Display*,GLXFBConfig,Window,const int*);
typedef void (*PFNGLXDESTROYWINDOWPROC)(Display*,GLXWindow);

typedef int (*PFNGLXSWAPINTERVALMESAPROC)(int);
typedef GLXContext (*PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display*,GLXFBConfig,GLXContext,Bool,const int*);

typedef struct {
    bool available;
    int major_opcode;
    int event_base;
    int error_base;
    int major;
    int minor;
} _sapp_xi_t;

typedef struct {
    int version;
    Window source;
    Atom format;
    Atom XdndAware;
    Atom XdndEnter;
    Atom XdndPosition;
    Atom XdndStatus;
    Atom XdndActionCopy;
    Atom XdndDrop;
    Atom XdndFinished;
    Atom XdndSelection;
    Atom XdndTypeList;
    Atom text_uri_list;
} _sapp_xdnd_t;

typedef struct {
    uint8_t mouse_buttons;
    Display* display;
    int screen;
    Window root;
    Colormap colormap;
    Window window;
    Cursor hidden_cursor;
    Cursor cursors[_SAPP_MOUSECURSOR_NUM];
    int window_state;
    float dpi;
    unsigned char error_code;
    Atom UTF8_STRING;
    Atom WM_PROTOCOLS;
    Atom WM_DELETE_WINDOW;
    Atom WM_STATE;
    Atom NET_WM_NAME;
    Atom NET_WM_ICON_NAME;
    Atom NET_WM_ICON;
    Atom NET_WM_STATE;
    Atom NET_WM_STATE_FULLSCREEN;
    _sapp_xi_t xi;
    _sapp_xdnd_t xdnd;
} _sapp_x11_t;

#if defined(_SAPP_GLX)

typedef struct {
    void* libgl;
    int major;
    int minor;
    int event_base;
    int error_base;
    GLXContext ctx;
    GLXWindow window;

    // GLX 1.3 functions
    PFNGLXGETFBCONFIGSPROC GetFBConfigs;
    PFNGLXGETFBCONFIGATTRIBPROC GetFBConfigAttrib;
    PFNGLXGETCLIENTSTRINGPROC GetClientString;
    PFNGLXQUERYEXTENSIONPROC QueryExtension;
    PFNGLXQUERYVERSIONPROC QueryVersion;
    PFNGLXDESTROYCONTEXTPROC DestroyContext;
    PFNGLXMAKECURRENTPROC MakeCurrent;
    PFNGLXSWAPBUFFERSPROC SwapBuffers;
    PFNGLXQUERYEXTENSIONSSTRINGPROC QueryExtensionsString;
    PFNGLXGETVISUALFROMFBCONFIGPROC GetVisualFromFBConfig;
    PFNGLXCREATEWINDOWPROC CreateWindow;
    PFNGLXDESTROYWINDOWPROC DestroyWindow;

    // GLX 1.4 and extension functions
    PFNGLXGETPROCADDRESSPROC GetProcAddress;
    PFNGLXGETPROCADDRESSPROC GetProcAddressARB;
    PFNGLXSWAPINTERVALEXTPROC SwapIntervalEXT;
    PFNGLXSWAPINTERVALMESAPROC SwapIntervalMESA;
    PFNGLXCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB;

    // special case glGetIntegerv
    void (*GetIntegerv)(uint32_t pname, int32_t* data);

    // extension availability
    bool EXT_swap_control;
    bool MESA_swap_control;
    bool ARB_multisample;
    bool ARB_create_context;
    bool ARB_create_context_profile;
} _sapp_glx_t;

#else

typedef struct {
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
} _sapp_egl_t;

#endif // _SAPP_GLX
#endif // _SAPP_LINUX

#if defined(_SAPP_ANY_GL)
typedef struct {
    uint32_t framebuffer;
} _sapp_gl_t;
#endif

typedef struct {
    bool enabled;
    int buf_size;
    char* buffer;
} _sapp_clipboard_t;

typedef struct {
    bool enabled;
    int max_files;
    int max_path_length;
    int num_files;
    int buf_size;
    char* buffer;
} _sapp_drop_t;

typedef struct {
    float x, y;
    float dx, dy;
    bool shown;
    bool locked;
    bool pos_valid;
    sapp_mouse_cursor current_cursor;
} _sapp_mouse_t;

typedef struct {
    sapp_desc desc;
    bool valid;
    bool fullscreen;
    bool first_frame;
    bool init_called;
    bool cleanup_called;
    bool quit_requested;
    bool quit_ordered;
    bool event_consumed;
    bool onscreen_keyboard_shown;
    int window_width;
    int window_height;
    int framebuffer_width;
    int framebuffer_height;
    int sample_count;
    int swap_interval;
    float dpi_scale;
    uint64_t frame_count;
    _sapp_timing_t timing;
    sapp_event event;
    _sapp_mouse_t mouse;
    _sapp_clipboard_t clipboard;
    _sapp_drop_t drop;
    sapp_icon_desc default_icon_desc;
    uint32_t* default_icon_pixels;
    #if defined(_SAPP_MACOS)
        _sapp_macos_t macos;
    #elif defined(_SAPP_IOS)
        _sapp_ios_t ios;
    #elif defined(_SAPP_WIN32)
        _sapp_win32_t win32;
        #if defined(SOKOL_D3D11)
            _sapp_d3d11_t d3d11;
        #endif
    #elif defined(_SAPP_ANDROID)
        _sapp_android_t android;
    #elif defined(_SAPP_LINUX)
        _sapp_x11_t x11;
        #if defined(_SAPP_GLX)
            _sapp_glx_t glx;
        #else
            _sapp_egl_t egl;
        #endif
    #endif
    #if defined(_SAPP_ANY_GL)
        _sapp_gl_t gl;
    #endif
    char window_title[_SAPP_MAX_TITLE_LENGTH];      // UTF-8
    wchar_t window_title_wide[_SAPP_MAX_TITLE_LENGTH];   // UTF-32 or UCS-2 */
    sapp_keycode keycodes[SAPP_MAX_KEYCODES];
} _sapp_t;
static _sapp_t _sapp;

// ██       ██████   ██████   ██████  ██ ███    ██  ██████
// ██      ██    ██ ██       ██       ██ ████   ██ ██
// ██      ██    ██ ██   ███ ██   ███ ██ ██ ██  ██ ██   ███
// ██      ██    ██ ██    ██ ██    ██ ██ ██  ██ ██ ██    ██
// ███████  ██████   ██████   ██████  ██ ██   ████  ██████
//
// >>logging
#if defined(SOKOL_DEBUG)
#define _SAPP_LOGITEM_XMACRO(item,msg) #item ": " msg,
static const char* _sapp_log_messages[] = {
    _SAPP_LOG_ITEMS
};
#undef _SAPP_LOGITEM_XMACRO
#endif // SOKOL_DEBUG

#define _SAPP_PANIC(code) _sapp_log(SAPP_LOGITEM_ ##code, 0, 0, __LINE__)
#define _SAPP_ERROR(code) _sapp_log(SAPP_LOGITEM_ ##code, 1, 0, __LINE__)
#define _SAPP_WARN(code) _sapp_log(SAPP_LOGITEM_ ##code, 2, 0, __LINE__)
#define _SAPP_INFO(code) _sapp_log(SAPP_LOGITEM_ ##code, 3, 0, __LINE__)

static void _sapp_log(sapp_log_item log_item, uint32_t log_level, const char* msg, uint32_t line_nr) {
    if (_sapp.desc.logger.func) {
        const char* filename = 0;
        #if defined(SOKOL_DEBUG)
            filename = __FILE__;
            if (0 == msg) {
                msg = _sapp_log_messages[log_item];
            }
        #endif
        _sapp.desc.logger.func("sapp", log_level, log_item, msg, line_nr, filename, _sapp.desc.logger.user_data);
    }
    else {
        // for log level PANIC it would be 'undefined behaviour' to continue
        if (log_level == 0) {
            abort();
        }
    }
}

// ███    ███ ███████ ███    ███  ██████  ██████  ██    ██
// ████  ████ ██      ████  ████ ██    ██ ██   ██  ██  ██
// ██ ████ ██ █████   ██ ████ ██ ██    ██ ██████    ████
// ██  ██  ██ ██      ██  ██  ██ ██    ██ ██   ██    ██
// ██      ██ ███████ ██      ██  ██████  ██   ██    ██
//
// >>memory
_SOKOL_PRIVATE void _sapp_clear(void* ptr, size_t size) {
    SOKOL_ASSERT(ptr && (size > 0));
    memset(ptr, 0, size);
}

_SOKOL_PRIVATE void* _sapp_malloc(size_t size) {
    SOKOL_ASSERT(size > 0);
    void* ptr;
    if (_sapp.desc.allocator.alloc_fn) {
        ptr = _sapp.desc.allocator.alloc_fn(size, _sapp.desc.allocator.user_data);
    } else {
        ptr = malloc(size);
    }
    if (0 == ptr) {
        _SAPP_PANIC(MALLOC_FAILED);
    }
    return ptr;
}

_SOKOL_PRIVATE void* _sapp_malloc_clear(size_t size) {
    void* ptr = _sapp_malloc(size);
    _sapp_clear(ptr, size);
    return ptr;
}

_SOKOL_PRIVATE void _sapp_free(void* ptr) {
    if (_sapp.desc.allocator.free_fn) {
        _sapp.desc.allocator.free_fn(ptr, _sapp.desc.allocator.user_data);
    }
    else {
        free(ptr);
    }
}

// ██   ██ ███████ ██      ██████  ███████ ██████  ███████
// ██   ██ ██      ██      ██   ██ ██      ██   ██ ██
// ███████ █████   ██      ██████  █████   ██████  ███████
// ██   ██ ██      ██      ██      ██      ██   ██      ██
// ██   ██ ███████ ███████ ██      ███████ ██   ██ ███████
//
// >>helpers
_SOKOL_PRIVATE void _sapp_call_init(void) {
    if (_sapp.desc.init_cb) {
        _sapp.desc.init_cb();
    }
    else if (_sapp.desc.init_userdata_cb) {
        _sapp.desc.init_userdata_cb(_sapp.desc.user_data);
    }
    _sapp.init_called = true;
}

_SOKOL_PRIVATE void _sapp_call_frame(void) {
    if (_sapp.init_called && !_sapp.cleanup_called) {
        if (_sapp.desc.frame_cb) {
            _sapp.desc.frame_cb();
        }
        else if (_sapp.desc.frame_userdata_cb) {
            _sapp.desc.frame_userdata_cb(_sapp.desc.user_data);
        }
    }
}

_SOKOL_PRIVATE void _sapp_call_cleanup(void) {
    if (!_sapp.cleanup_called) {
        if (_sapp.desc.cleanup_cb) {
            _sapp.desc.cleanup_cb();
        }
        else if (_sapp.desc.cleanup_userdata_cb) {
            _sapp.desc.cleanup_userdata_cb(_sapp.desc.user_data);
        }
        _sapp.cleanup_called = true;
    }
}

_SOKOL_PRIVATE bool _sapp_call_event(const sapp_event* e) {
    if (!_sapp.cleanup_called) {
        if (_sapp.desc.event_cb) {
            _sapp.desc.event_cb(e);
        }
        else if (_sapp.desc.event_userdata_cb) {
            _sapp.desc.event_userdata_cb(e, _sapp.desc.user_data);
        }
    }
    if (_sapp.event_consumed) {
        _sapp.event_consumed = false;
        return true;
    }
    else {
        return false;
    }
}

_SOKOL_PRIVATE char* _sapp_dropped_file_path_ptr(int index) {
    SOKOL_ASSERT(_sapp.drop.buffer);
    SOKOL_ASSERT((index >= 0) && (index <= _sapp.drop.max_files));
    int offset = index * _sapp.drop.max_path_length;
    SOKOL_ASSERT(offset < _sapp.drop.buf_size);
    return &_sapp.drop.buffer[offset];
}

/* Copy a string into a fixed size buffer with guaranteed zero-
   termination.

   Return false if the string didn't fit into the buffer and had to be clamped.

   FIXME: Currently UTF-8 strings might become invalid if the string
   is clamped, because the last zero-byte might be written into
   the middle of a multi-byte sequence.
*/
_SOKOL_PRIVATE bool _sapp_strcpy(const char* src, char* dst, int max_len) {
    SOKOL_ASSERT(src && dst && (max_len > 0));
    char* const end = &(dst[max_len-1]);
    char c = 0;
    for (int i = 0; i < max_len; i++) {
        c = *src;
        if (c != 0) {
            src++;
        }
        *dst++ = c;
    }
    /* truncated? */
    if (c != 0) {
        *end = 0;
        return false;
    }
    else {
        return true;
    }
}

_SOKOL_PRIVATE sapp_desc _sapp_desc_defaults(const sapp_desc* desc) {
    SOKOL_ASSERT((desc->allocator.alloc_fn && desc->allocator.free_fn) || (!desc->allocator.alloc_fn && !desc->allocator.free_fn));
    sapp_desc res = *desc;
    res.sample_count = _sapp_def(res.sample_count, 1);
    res.swap_interval = _sapp_def(res.swap_interval, 1);
    // NOTE: can't patch the default for gl_major_version and gl_minor_version
    // independently, because a desired version 4.0 would be patched to 4.2
    // (or expressed differently: zero is a valid value for gl_minor_version
    // and can't be used to indicate 'default')
    if (0 == res.gl_major_version) {
        #if defined(_SAPP_APPLE)
            res.gl_major_version = 4;
            res.gl_minor_version = 1;
        #else
            res.gl_major_version = 4;
            res.gl_minor_version = 3;
        #endif
    }
    res.clipboard_size = _sapp_def(res.clipboard_size, 8192);
    res.max_dropped_files = _sapp_def(res.max_dropped_files, 1);
    res.max_dropped_file_path_length = _sapp_def(res.max_dropped_file_path_length, 2048);
    res.window_title = _sapp_def(res.window_title, "sokol_app");
    return res;
}

_SOKOL_PRIVATE void _sapp_init_state(const sapp_desc* desc) {
    SOKOL_ASSERT(desc);
    SOKOL_ASSERT(desc->width >= 0);
    SOKOL_ASSERT(desc->height >= 0);
    SOKOL_ASSERT(desc->sample_count >= 0);
    SOKOL_ASSERT(desc->swap_interval >= 0);
    SOKOL_ASSERT(desc->clipboard_size >= 0);
    SOKOL_ASSERT(desc->max_dropped_files >= 0);
    SOKOL_ASSERT(desc->max_dropped_file_path_length >= 0);
    _SAPP_CLEAR_ARC_STRUCT(_sapp_t, _sapp);
    _sapp.desc = _sapp_desc_defaults(desc);
    _sapp.first_frame = true;
    // NOTE: _sapp.desc.width/height may be 0! Platform backends need to deal with this
    _sapp.window_width = _sapp.desc.width;
    _sapp.window_height = _sapp.desc.height;
    _sapp.framebuffer_width = _sapp.window_width;
    _sapp.framebuffer_height = _sapp.window_height;
    _sapp.sample_count = _sapp.desc.sample_count;
    _sapp.swap_interval = _sapp.desc.swap_interval;
    _sapp.clipboard.enabled = _sapp.desc.enable_clipboard;
    if (_sapp.clipboard.enabled) {
        _sapp.clipboard.buf_size = _sapp.desc.clipboard_size;
        _sapp.clipboard.buffer = (char*) _sapp_malloc_clear((size_t)_sapp.clipboard.buf_size);
    }
    _sapp.drop.enabled = _sapp.desc.enable_dragndrop;
    if (_sapp.drop.enabled) {
        _sapp.drop.max_files = _sapp.desc.max_dropped_files;
        _sapp.drop.max_path_length = _sapp.desc.max_dropped_file_path_length;
        _sapp.drop.buf_size = _sapp.drop.max_files * _sapp.drop.max_path_length;
        _sapp.drop.buffer = (char*) _sapp_malloc_clear((size_t)_sapp.drop.buf_size);
    }
    _sapp_strcpy(_sapp.desc.window_title, _sapp.window_title, sizeof(_sapp.window_title));
    _sapp.desc.window_title = _sapp.window_title;
    _sapp.dpi_scale = 1.0f;
    _sapp.fullscreen = _sapp.desc.fullscreen;
    _sapp.mouse.shown = true;
    _sapp_timing_init(&_sapp.timing);
}

_SOKOL_PRIVATE void _sapp_discard_state(void) {
    if (_sapp.clipboard.enabled) {
        SOKOL_ASSERT(_sapp.clipboard.buffer);
        _sapp_free((void*)_sapp.clipboard.buffer);
    }
    if (_sapp.drop.enabled) {
        SOKOL_ASSERT(_sapp.drop.buffer);
        _sapp_free((void*)_sapp.drop.buffer);
    }
    if (_sapp.default_icon_pixels) {
        _sapp_free((void*)_sapp.default_icon_pixels);
    }
    _SAPP_CLEAR_ARC_STRUCT(_sapp_t, _sapp);
}

_SOKOL_PRIVATE void _sapp_init_event(sapp_event_type type) {
    _sapp_clear(&_sapp.event, sizeof(_sapp.event));
    _sapp.event.type = type;
    _sapp.event.frame_count = _sapp.frame_count;
    _sapp.event.mouse_button = SAPP_MOUSEBUTTON_INVALID;
    _sapp.event.window_width = _sapp.window_width;
    _sapp.event.window_height = _sapp.window_height;
    _sapp.event.framebuffer_width = _sapp.framebuffer_width;
    _sapp.event.framebuffer_height = _sapp.framebuffer_height;
    _sapp.event.mouse_x = _sapp.mouse.x;
    _sapp.event.mouse_y = _sapp.mouse.y;
    _sapp.event.mouse_dx = _sapp.mouse.dx;
    _sapp.event.mouse_dy = _sapp.mouse.dy;
}

_SOKOL_PRIVATE bool _sapp_events_enabled(void) {
    /* only send events when an event callback is set, and the init function was called */
    return (_sapp.desc.event_cb || _sapp.desc.event_userdata_cb) && _sapp.init_called;
}

_SOKOL_PRIVATE sapp_keycode _sapp_translate_key(int scan_code) {
    if ((scan_code >= 0) && (scan_code < SAPP_MAX_KEYCODES)) {
        return _sapp.keycodes[scan_code];
    }
    else {
        return SAPP_KEYCODE_INVALID;
    }
}

_SOKOL_PRIVATE void _sapp_clear_drop_buffer(void) {
    if (_sapp.drop.enabled) {
        SOKOL_ASSERT(_sapp.drop.buffer);
        _sapp_clear(_sapp.drop.buffer, (size_t)_sapp.drop.buf_size);
    }
}

_SOKOL_PRIVATE void _sapp_frame(void) {
    if (_sapp.first_frame) {
        _sapp.first_frame = false;
        _sapp_call_init();
    }
    _sapp_call_frame();
    _sapp.frame_count++;
}

_SOKOL_PRIVATE bool _sapp_image_validate(const sapp_image_desc* desc) {
    SOKOL_ASSERT(desc->width > 0);
    SOKOL_ASSERT(desc->height > 0);
    SOKOL_ASSERT(desc->pixels.ptr != 0);
    SOKOL_ASSERT(desc->pixels.size > 0);
    const size_t wh_size = (size_t)(desc->width * desc->height) * sizeof(uint32_t);
    if (wh_size != desc->pixels.size) {
        _SAPP_ERROR(IMAGE_DATA_SIZE_MISMATCH);
        return false;
    }
    return true;
}

_SOKOL_PRIVATE int _sapp_image_bestmatch(const sapp_image_desc image_descs[], int num_images, int width, int height) {
    int least_diff = 0x7FFFFFFF;
    int least_index = 0;
    for (int i = 0; i < num_images; i++) {
        int diff = (image_descs[i].width * image_descs[i].height) - (width * height);
        if (diff < 0) {
            diff = -diff;
        }
        if (diff < least_diff) {
            least_diff = diff;
            least_index = i;
        }
    }
    return least_index;
}

_SOKOL_PRIVATE int _sapp_icon_num_images(const sapp_icon_desc* desc) {
    int index = 0;
    for (; index < SAPP_MAX_ICONIMAGES; index++) {
        if (0 == desc->images[index].pixels.ptr) {
            break;
        }
    }
    return index;
}

_SOKOL_PRIVATE bool _sapp_validate_icon_desc(const sapp_icon_desc* desc, int num_images) {
    SOKOL_ASSERT(num_images <= SAPP_MAX_ICONIMAGES);
    for (int i = 0; i < num_images; i++) {
        const sapp_image_desc* img_desc = &desc->images[i];
        if (!_sapp_image_validate(img_desc)) {
            return false;
        }
    }
    return true;
}

_SOKOL_PRIVATE void _sapp_setup_default_icon(void) {
    SOKOL_ASSERT(0 == _sapp.default_icon_pixels);

    const int num_icons = 3;
    const int icon_sizes[3] = { 16, 32, 64 };   // must be multiple of 8!

    // allocate a pixel buffer for all icon pixels
    int all_num_pixels = 0;
    for (int i = 0; i < num_icons; i++) {
        all_num_pixels += icon_sizes[i] * icon_sizes[i];
    }
    _sapp.default_icon_pixels = (uint32_t*) _sapp_malloc_clear((size_t)all_num_pixels * sizeof(uint32_t));

    // initialize default_icon_desc struct
    uint32_t* dst = _sapp.default_icon_pixels;
    const uint32_t* dst_end = dst + all_num_pixels;
    (void)dst_end; // silence unused warning in release mode
    for (int i = 0; i < num_icons; i++) {
        const int dim = (int) icon_sizes[i];
        const int num_pixels = dim * dim;
        sapp_image_desc* img_desc = &_sapp.default_icon_desc.images[i];
        img_desc->width = dim;
        img_desc->height = dim;
        img_desc->pixels.ptr = dst;
        img_desc->pixels.size = (size_t)num_pixels * sizeof(uint32_t);
        dst += num_pixels;
    }
    SOKOL_ASSERT(dst == dst_end);

    // Amstrad CPC font 'S'
    const uint8_t tile[8] = {
        0x3C,
        0x66,
        0x60,
        0x3C,
        0x06,
        0x66,
        0x3C,
        0x00,
    };
    // rainbow colors
    const uint32_t colors[8] = {
        0xFF4370FF,
        0xFF26A7FF,
        0xFF58EEFF,
        0xFF57E1D4,
        0xFF65CC9C,
        0xFF6ABB66,
        0xFFF5A542,
        0xFFC2577E,
    };
    dst = _sapp.default_icon_pixels;
    const uint32_t blank = 0x00FFFFFF;
    const uint32_t shadow = 0xFF000000;
    for (int i = 0; i < num_icons; i++) {
        const int dim = icon_sizes[i];
        SOKOL_ASSERT((dim % 8) == 0);
        const int scale = dim / 8;
        for (int ty = 0, y = 0; ty < 8; ty++) {
            const uint32_t color = colors[ty];
            for (int sy = 0; sy < scale; sy++, y++) {
                uint8_t bits = tile[ty];
                for (int tx = 0, x = 0; tx < 8; tx++, bits<<=1) {
                    uint32_t pixel = (0 == (bits & 0x80)) ? blank : color;
                    for (int sx = 0; sx < scale; sx++, x++) {
                        SOKOL_ASSERT(dst < dst_end);
                        *dst++ = pixel;
                    }
                }
            }
        }
    }
    SOKOL_ASSERT(dst == dst_end);

    // right shadow
    dst = _sapp.default_icon_pixels;
    for (int i = 0; i < num_icons; i++) {
        const int dim = icon_sizes[i];
        for (int y = 0; y < dim; y++) {
            uint32_t prev_color = blank;
            for (int x = 0; x < dim; x++) {
                const int dst_index = y * dim + x;
                const uint32_t cur_color = dst[dst_index];
                if ((cur_color == blank) && (prev_color != blank)) {
                    dst[dst_index] = shadow;
                }
                prev_color = cur_color;
            }
        }
        dst += dim * dim;
    }
    SOKOL_ASSERT(dst == dst_end);

    // bottom shadow
    dst = _sapp.default_icon_pixels;
    for (int i = 0; i < num_icons; i++) {
        const int dim = icon_sizes[i];
        for (int x = 0; x < dim; x++) {
            uint32_t prev_color = blank;
            for (int y = 0; y < dim; y++) {
                const int dst_index = y * dim + x;
                const uint32_t cur_color = dst[dst_index];
                if ((cur_color == blank) && (prev_color != blank)) {
                    dst[dst_index] = shadow;
                }
                prev_color = cur_color;
            }
        }
        dst += dim * dim;
    }
    SOKOL_ASSERT(dst == dst_end);
}

// >> platform impls
#if defined(_SAPP_APPLE)
#if __has_feature(objc_arc)
#define _SAPP_OBJC_RELEASE(obj) { obj = nil; }
#else
#define _SAPP_OBJC_RELEASE(obj) { [obj release]; obj = nil; }
#endif

#if defined(_SAPP_MACOS)
#include "sokol_app_macos.c"
#endif

#if defined(_SAPP_IOS)
#include "sokol_app_ios.c"
#endif
#endif /* _SAPP_APPLE */

#if defined(_SAPP_WIN32)
#include "sokol_app_windows.c"
#endif

#if defined(_SAPP_ANDROID)
#include "sokol_app_android.c"
#endif

#if defined(_SAPP_LINUX)
#include "sokol_app_linux.c"
#endif

// ██████  ██    ██ ██████  ██      ██  ██████
// ██   ██ ██    ██ ██   ██ ██      ██ ██
// ██████  ██    ██ ██████  ██      ██ ██
// ██      ██    ██ ██   ██ ██      ██ ██
// ██       ██████  ██████  ███████ ██  ██████
//
// >>public
#if defined(SOKOL_NO_ENTRY)
SOKOL_API_IMPL void sapp_run(const sapp_desc* desc) {
    SOKOL_ASSERT(desc);
    #if defined(_SAPP_MACOS)
        _sapp_macos_run(desc);
    #elif defined(_SAPP_IOS)
        _sapp_ios_run(desc);
    #elif defined(_SAPP_WIN32)
        _sapp_win32_run(desc);
    #elif defined(_SAPP_LINUX)
        _sapp_linux_run(desc);
    #else
    #error "sapp_run() not supported on this platform"
    #endif
}

/* this is just a stub so the linker doesn't complain */
sapp_desc sokol_main(int argc, char* argv[]) {
    _SOKOL_UNUSED(argc);
    _SOKOL_UNUSED(argv);
    sapp_desc desc;
    _sapp_clear(&desc, sizeof(desc));
    return desc;
}
#else
/* likewise, in normal mode, sapp_run() is just an empty stub */
SOKOL_API_IMPL void sapp_run(const sapp_desc* desc) {
    _SOKOL_UNUSED(desc);
}
#endif

SOKOL_API_IMPL bool sapp_isvalid(void) {
    return _sapp.valid;
}

SOKOL_API_IMPL void* sapp_userdata(void) {
    return _sapp.desc.user_data;
}

SOKOL_API_IMPL sapp_desc sapp_query_desc(void) {
    return _sapp.desc;
}

SOKOL_API_IMPL uint64_t sapp_frame_count(void) {
    return _sapp.frame_count;
}

SOKOL_API_IMPL double sapp_frame_duration(void) {
    return _sapp_timing_get_avg(&_sapp.timing);
}

SOKOL_API_IMPL int sapp_width(void) {
    return (_sapp.framebuffer_width > 0) ? _sapp.framebuffer_width : 1;
}

SOKOL_API_IMPL float sapp_widthf(void) {
    return (float)sapp_width();
}

SOKOL_API_IMPL int sapp_height(void) {
    return (_sapp.framebuffer_height > 0) ? _sapp.framebuffer_height : 1;
}

SOKOL_API_IMPL float sapp_heightf(void) {
    return (float)sapp_height();
}

SOKOL_API_IMPL int sapp_color_format(void) {
    #if defined(SOKOL_METAL) || defined(SOKOL_D3D11)
        return _SAPP_PIXELFORMAT_BGRA8;
    #else
        return _SAPP_PIXELFORMAT_RGBA8;
    #endif
}

SOKOL_API_IMPL int sapp_depth_format(void) {
    return _SAPP_PIXELFORMAT_DEPTH_STENCIL;
}

SOKOL_API_IMPL int sapp_sample_count(void) {
    return _sapp.sample_count;
}

SOKOL_API_IMPL bool sapp_high_dpi(void) {
    return _sapp.desc.high_dpi && (_sapp.dpi_scale >= 1.5f);
}

SOKOL_API_IMPL float sapp_dpi_scale(void) {
    return _sapp.dpi_scale;
}

SOKOL_API_IMPL const void* sapp_egl_get_display(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(_SAPP_ANDROID)
        return _sapp.android.display;
    #elif defined(_SAPP_LINUX) && !defined(_SAPP_GLX)
        return _sapp.egl.display;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_egl_get_context(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(_SAPP_ANDROID)
        return _sapp.android.context;
    #elif defined(_SAPP_LINUX) && !defined(_SAPP_GLX)
        return _sapp.egl.context;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL void sapp_show_keyboard(bool show) {
    #if defined(_SAPP_IOS)
    _sapp_ios_show_keyboard(show);
    #elif defined(_SAPP_ANDROID)
    _sapp_android_show_keyboard(show);
    #else
    _SOKOL_UNUSED(show);
    #endif
}

SOKOL_API_IMPL bool sapp_keyboard_shown(void) {
    return _sapp.onscreen_keyboard_shown;
}

SOKOL_API_IMPL bool sapp_is_fullscreen(void) {
    return _sapp.fullscreen;
}

SOKOL_API_IMPL void sapp_toggle_fullscreen(void) {
    #if defined(_SAPP_MACOS)
    _sapp_macos_toggle_fullscreen();
    #elif defined(_SAPP_WIN32)
    _sapp_win32_toggle_fullscreen();
    #elif defined(_SAPP_LINUX)
    _sapp_x11_toggle_fullscreen();
    #endif
}

/* NOTE that sapp_show_mouse() does not "stack" like the Win32 or macOS API functions! */
SOKOL_API_IMPL void sapp_show_mouse(bool show) {
    if (_sapp.mouse.shown != show) {
        #if defined(_SAPP_MACOS)
        _sapp_macos_update_cursor(_sapp.mouse.current_cursor, show);
        #elif defined(_SAPP_WIN32)
        _sapp_win32_update_cursor(_sapp.mouse.current_cursor, show, false);
        #elif defined(_SAPP_LINUX)
        _sapp_x11_update_cursor(_sapp.mouse.current_cursor, show);
        #endif
        _sapp.mouse.shown = show;
    }
}

SOKOL_API_IMPL bool sapp_mouse_shown(void) {
    return _sapp.mouse.shown;
}

SOKOL_API_IMPL void sapp_lock_mouse(bool lock) {
    #if defined(_SAPP_MACOS)
    _sapp_macos_lock_mouse(lock);
    #elif defined(_SAPP_WIN32)
    _sapp_win32_lock_mouse(lock);
    #elif defined(_SAPP_LINUX)
    _sapp_x11_lock_mouse(lock);
    #else
    _sapp.mouse.locked = lock;
    #endif
}

SOKOL_API_IMPL bool sapp_mouse_locked(void) {
    return _sapp.mouse.locked;
}

SOKOL_API_IMPL void sapp_set_mouse_cursor(sapp_mouse_cursor cursor) {
    SOKOL_ASSERT((cursor >= 0) && (cursor < _SAPP_MOUSECURSOR_NUM));
    if (_sapp.mouse.current_cursor != cursor) {
        #if defined(_SAPP_MACOS)
        _sapp_macos_update_cursor(cursor, _sapp.mouse.shown);
        #elif defined(_SAPP_WIN32)
        _sapp_win32_update_cursor(cursor, _sapp.mouse.shown, false);
        #elif defined(_SAPP_LINUX)
        _sapp_x11_update_cursor(cursor, _sapp.mouse.shown);
        #endif
        _sapp.mouse.current_cursor = cursor;
    }
}

SOKOL_API_IMPL sapp_mouse_cursor sapp_get_mouse_cursor(void) {
    return _sapp.mouse.current_cursor;
}

SOKOL_API_IMPL void sapp_request_quit(void) {
    _sapp.quit_requested = true;
}

SOKOL_API_IMPL void sapp_cancel_quit(void) {
    _sapp.quit_requested = false;
}

SOKOL_API_IMPL void sapp_quit(void) {
    _sapp.quit_ordered = true;
}

SOKOL_API_IMPL void sapp_consume_event(void) {
    _sapp.event_consumed = true;
}

/* NOTE: on HTML5, sapp_set_clipboard_string() must be called from within event handler! */
SOKOL_API_IMPL void sapp_set_clipboard_string(const char* str) {
    if (!_sapp.clipboard.enabled) {
        return;
    }
    SOKOL_ASSERT(str);
    #if defined(_SAPP_MACOS)
        _sapp_macos_set_clipboard_string(str);
    #elif defined(_SAPP_WIN32)
        _sapp_win32_set_clipboard_string(str);
    #else
        /* not implemented */
    #endif
    _sapp_strcpy(str, _sapp.clipboard.buffer, _sapp.clipboard.buf_size);
}

SOKOL_API_IMPL const char* sapp_get_clipboard_string(void) {
    if (!_sapp.clipboard.enabled) {
        return "";
    }
    #if defined(_SAPP_MACOS)
        return _sapp_macos_get_clipboard_string();
    #elif defined(_SAPP_WIN32)
        return _sapp_win32_get_clipboard_string();
    #else
        /* not implemented */
        return _sapp.clipboard.buffer;
    #endif
}

SOKOL_API_IMPL void sapp_set_window_title(const char* title) {
    SOKOL_ASSERT(title);
    _sapp_strcpy(title, _sapp.window_title, sizeof(_sapp.window_title));
    #if defined(_SAPP_MACOS)
        _sapp_macos_update_window_title();
    #elif defined(_SAPP_WIN32)
        _sapp_win32_update_window_title();
    #elif defined(_SAPP_LINUX)
        _sapp_x11_update_window_title();
    #endif
}

SOKOL_API_IMPL void sapp_set_icon(const sapp_icon_desc* desc) {
    SOKOL_ASSERT(desc);
    if (desc->sokol_default) {
        if (0 == _sapp.default_icon_pixels) {
            _sapp_setup_default_icon();
        }
        SOKOL_ASSERT(0 != _sapp.default_icon_pixels);
        desc = &_sapp.default_icon_desc;
    }
    const int num_images = _sapp_icon_num_images(desc);
    if (num_images == 0) {
        return;
    }
    SOKOL_ASSERT((num_images > 0) && (num_images <= SAPP_MAX_ICONIMAGES));
    if (!_sapp_validate_icon_desc(desc, num_images)) {
        return;
    }
    #if defined(_SAPP_MACOS)
        _sapp_macos_set_icon(desc, num_images);
    #elif defined(_SAPP_WIN32)
        _sapp_win32_set_icon(desc, num_images);
    #elif defined(_SAPP_LINUX)
        _sapp_x11_set_icon(desc, num_images);
    #endif
}

SOKOL_API_IMPL int sapp_get_num_dropped_files(void) {
    SOKOL_ASSERT(_sapp.drop.enabled);
    return _sapp.drop.num_files;
}

SOKOL_API_IMPL const char* sapp_get_dropped_file_path(int index) {
    SOKOL_ASSERT(_sapp.drop.enabled);
    SOKOL_ASSERT((index >= 0) && (index < _sapp.drop.num_files));
    SOKOL_ASSERT(_sapp.drop.buffer);
    if (!_sapp.drop.enabled) {
        return "";
    }
    if ((index < 0) || (index >= _sapp.drop.max_files)) {
        return "";
    }
    return (const char*) _sapp_dropped_file_path_ptr(index);
}

SOKOL_API_IMPL const void* sapp_metal_get_device(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        #if defined(_SAPP_MACOS)
            const void* obj = (__bridge const void*) _sapp.macos.mtl_device;
        #else
            const void* obj = (__bridge const void*) _sapp.ios.mtl_device;
        #endif
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_current_drawable(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        #if defined(_SAPP_MACOS)
            const void* obj = (__bridge const void*) [_sapp.macos.view currentDrawable];
        #else
            const void* obj = (__bridge const void*) [_sapp.ios.view currentDrawable];
        #endif
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_depth_stencil_texture(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        #if defined(_SAPP_MACOS)
            const void* obj = (__bridge const void*) [_sapp.macos.view depthStencilTexture];
        #else
            const void* obj = (__bridge const void*) [_sapp.ios.view depthStencilTexture];
        #endif
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_metal_get_msaa_color_texture(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_METAL)
        #if defined(_SAPP_MACOS)
            const void* obj = (__bridge const void*) [_sapp.macos.view multisampleColorTexture];
        #else
            const void* obj = (__bridge const void*) [_sapp.ios.view multisampleColorTexture];
        #endif
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_macos_get_window(void) {
    #if defined(_SAPP_MACOS)
        const void* obj = (__bridge const void*) _sapp.macos.window;
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_ios_get_window(void) {
    #if defined(_SAPP_IOS)
        const void* obj = (__bridge const void*) _sapp.ios.window;
        SOKOL_ASSERT(obj);
        return obj;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_device(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp.d3d11.device;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_device_context(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp.d3d11.device_context;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_swap_chain(void) {
    SOKOL_ASSERT(_sapp.valid);
#if defined(SOKOL_D3D11)
    return _sapp.d3d11.swap_chain;
#else
    return 0;
#endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_render_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        if (_sapp.sample_count > 1) {
            SOKOL_ASSERT(_sapp.d3d11.msaa_rtv);
            return _sapp.d3d11.msaa_rtv;
        } else {
            SOKOL_ASSERT(_sapp.d3d11.rtv);
            return _sapp.d3d11.rtv;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_resolve_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        if (_sapp.sample_count > 1) {
            SOKOL_ASSERT(_sapp.d3d11.rtv);
            return _sapp.d3d11.rtv;
        } else {
            return 0;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_d3d11_get_depth_stencil_view(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(SOKOL_D3D11)
        return _sapp.d3d11.dsv;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sapp_win32_get_hwnd(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(_SAPP_WIN32)
        return _sapp.win32.hwnd;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL uint32_t sapp_gl_get_framebuffer(void) {
    SOKOL_ASSERT(_sapp.valid);
    #if defined(_SAPP_ANY_GL)
        return _sapp.gl.framebuffer;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL int sapp_gl_get_major_version(void) {
    SOKOL_ASSERT(_sapp.valid);
    return 0;
}

SOKOL_API_IMPL int sapp_gl_get_minor_version(void) {
    SOKOL_ASSERT(_sapp.valid);
    return 0;
}

SOKOL_API_IMPL const void* sapp_android_get_native_activity(void) {
    // NOTE: _sapp.valid is not asserted here because sapp_android_get_native_activity()
    // needs to be callable from within sokol_main() (see: https://github.com/floooh/sokol/issues/708)
    #if defined(_SAPP_ANDROID)
        return (void*)_sapp.android.activity;
    #else
        return 0;
    #endif
}

#endif /* SOKOL_APP_IMPL */
