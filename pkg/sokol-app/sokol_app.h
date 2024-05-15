#ifndef SOKOL_APP_INCLUDED
#define SOKOL_APP_INCLUDED (1)
#include <stddef.h> // size_t
#include <stdint.h>
#include <stdbool.h>

#define SOKOL_APP_API_DECL extern

/* misc constants */
enum {
    SAPP_MAX_TOUCHPOINTS = 8,
    SAPP_MAX_MOUSEBUTTONS = 3,
    SAPP_MAX_KEYCODES = 512,
    SAPP_MAX_ICONIMAGES = 8,
};

/*
    sapp_event_type

    The type of event that's passed to the event handler callback
    in the sapp_event.type field. These are not just "traditional"
    input events, but also notify the application about state changes
    or other user-invoked actions.
*/
typedef enum sapp_event_type {
    SAPP_EVENTTYPE_INVALID,
    SAPP_EVENTTYPE_KEY_DOWN,
    SAPP_EVENTTYPE_KEY_UP,
    SAPP_EVENTTYPE_CHAR,
    SAPP_EVENTTYPE_MOUSE_DOWN,
    SAPP_EVENTTYPE_MOUSE_UP,
    SAPP_EVENTTYPE_MOUSE_SCROLL,
    SAPP_EVENTTYPE_MOUSE_MOVE,
    SAPP_EVENTTYPE_MOUSE_ENTER,
    SAPP_EVENTTYPE_MOUSE_LEAVE,
    SAPP_EVENTTYPE_TOUCHES_BEGAN,
    SAPP_EVENTTYPE_TOUCHES_MOVED,
    SAPP_EVENTTYPE_TOUCHES_ENDED,
    SAPP_EVENTTYPE_TOUCHES_CANCELLED,
    SAPP_EVENTTYPE_RESIZED,
    SAPP_EVENTTYPE_ICONIFIED,
    SAPP_EVENTTYPE_RESTORED,
    SAPP_EVENTTYPE_FOCUSED,
    SAPP_EVENTTYPE_UNFOCUSED,
    SAPP_EVENTTYPE_SUSPENDED,
    SAPP_EVENTTYPE_RESUMED,
    SAPP_EVENTTYPE_QUIT_REQUESTED,
    SAPP_EVENTTYPE_CLIPBOARD_PASTED,
    SAPP_EVENTTYPE_FILES_DROPPED,
    _SAPP_EVENTTYPE_NUM,
    _SAPP_EVENTTYPE_FORCE_U32 = 0x7FFFFFFF
} sapp_event_type;

/*
    sapp_keycode

    The 'virtual keycode' of a KEY_DOWN or KEY_UP event in the
    struct field sapp_event.key_code.

    Note that the keycode values are identical with GLFW.
*/
typedef enum sapp_keycode {
    SAPP_KEYCODE_INVALID          = 0,
    SAPP_KEYCODE_SPACE            = 32,
    SAPP_KEYCODE_APOSTROPHE       = 39,  /* ' */
    SAPP_KEYCODE_COMMA            = 44,  /* , */
    SAPP_KEYCODE_MINUS            = 45,  /* - */
    SAPP_KEYCODE_PERIOD           = 46,  /* . */
    SAPP_KEYCODE_SLASH            = 47,  /* / */
    SAPP_KEYCODE_0                = 48,
    SAPP_KEYCODE_1                = 49,
    SAPP_KEYCODE_2                = 50,
    SAPP_KEYCODE_3                = 51,
    SAPP_KEYCODE_4                = 52,
    SAPP_KEYCODE_5                = 53,
    SAPP_KEYCODE_6                = 54,
    SAPP_KEYCODE_7                = 55,
    SAPP_KEYCODE_8                = 56,
    SAPP_KEYCODE_9                = 57,
    SAPP_KEYCODE_SEMICOLON        = 59,  /* ; */
    SAPP_KEYCODE_EQUAL            = 61,  /* = */
    SAPP_KEYCODE_A                = 65,
    SAPP_KEYCODE_B                = 66,
    SAPP_KEYCODE_C                = 67,
    SAPP_KEYCODE_D                = 68,
    SAPP_KEYCODE_E                = 69,
    SAPP_KEYCODE_F                = 70,
    SAPP_KEYCODE_G                = 71,
    SAPP_KEYCODE_H                = 72,
    SAPP_KEYCODE_I                = 73,
    SAPP_KEYCODE_J                = 74,
    SAPP_KEYCODE_K                = 75,
    SAPP_KEYCODE_L                = 76,
    SAPP_KEYCODE_M                = 77,
    SAPP_KEYCODE_N                = 78,
    SAPP_KEYCODE_O                = 79,
    SAPP_KEYCODE_P                = 80,
    SAPP_KEYCODE_Q                = 81,
    SAPP_KEYCODE_R                = 82,
    SAPP_KEYCODE_S                = 83,
    SAPP_KEYCODE_T                = 84,
    SAPP_KEYCODE_U                = 85,
    SAPP_KEYCODE_V                = 86,
    SAPP_KEYCODE_W                = 87,
    SAPP_KEYCODE_X                = 88,
    SAPP_KEYCODE_Y                = 89,
    SAPP_KEYCODE_Z                = 90,
    SAPP_KEYCODE_LEFT_BRACKET     = 91,  /* [ */
    SAPP_KEYCODE_BACKSLASH        = 92,  /* \ */
    SAPP_KEYCODE_RIGHT_BRACKET    = 93,  /* ] */
    SAPP_KEYCODE_GRAVE_ACCENT     = 96,  /* ` */
    SAPP_KEYCODE_WORLD_1          = 161, /* non-US #1 */
    SAPP_KEYCODE_WORLD_2          = 162, /* non-US #2 */
    SAPP_KEYCODE_ESCAPE           = 256,
    SAPP_KEYCODE_ENTER            = 257,
    SAPP_KEYCODE_TAB              = 258,
    SAPP_KEYCODE_BACKSPACE        = 259,
    SAPP_KEYCODE_INSERT           = 260,
    SAPP_KEYCODE_DELETE           = 261,
    SAPP_KEYCODE_RIGHT            = 262,
    SAPP_KEYCODE_LEFT             = 263,
    SAPP_KEYCODE_DOWN             = 264,
    SAPP_KEYCODE_UP               = 265,
    SAPP_KEYCODE_PAGE_UP          = 266,
    SAPP_KEYCODE_PAGE_DOWN        = 267,
    SAPP_KEYCODE_HOME             = 268,
    SAPP_KEYCODE_END              = 269,
    SAPP_KEYCODE_CAPS_LOCK        = 280,
    SAPP_KEYCODE_SCROLL_LOCK      = 281,
    SAPP_KEYCODE_NUM_LOCK         = 282,
    SAPP_KEYCODE_PRINT_SCREEN     = 283,
    SAPP_KEYCODE_PAUSE            = 284,
    SAPP_KEYCODE_F1               = 290,
    SAPP_KEYCODE_F2               = 291,
    SAPP_KEYCODE_F3               = 292,
    SAPP_KEYCODE_F4               = 293,
    SAPP_KEYCODE_F5               = 294,
    SAPP_KEYCODE_F6               = 295,
    SAPP_KEYCODE_F7               = 296,
    SAPP_KEYCODE_F8               = 297,
    SAPP_KEYCODE_F9               = 298,
    SAPP_KEYCODE_F10              = 299,
    SAPP_KEYCODE_F11              = 300,
    SAPP_KEYCODE_F12              = 301,
    SAPP_KEYCODE_F13              = 302,
    SAPP_KEYCODE_F14              = 303,
    SAPP_KEYCODE_F15              = 304,
    SAPP_KEYCODE_F16              = 305,
    SAPP_KEYCODE_F17              = 306,
    SAPP_KEYCODE_F18              = 307,
    SAPP_KEYCODE_F19              = 308,
    SAPP_KEYCODE_F20              = 309,
    SAPP_KEYCODE_F21              = 310,
    SAPP_KEYCODE_F22              = 311,
    SAPP_KEYCODE_F23              = 312,
    SAPP_KEYCODE_F24              = 313,
    SAPP_KEYCODE_F25              = 314,
    SAPP_KEYCODE_KP_0             = 320,
    SAPP_KEYCODE_KP_1             = 321,
    SAPP_KEYCODE_KP_2             = 322,
    SAPP_KEYCODE_KP_3             = 323,
    SAPP_KEYCODE_KP_4             = 324,
    SAPP_KEYCODE_KP_5             = 325,
    SAPP_KEYCODE_KP_6             = 326,
    SAPP_KEYCODE_KP_7             = 327,
    SAPP_KEYCODE_KP_8             = 328,
    SAPP_KEYCODE_KP_9             = 329,
    SAPP_KEYCODE_KP_DECIMAL       = 330,
    SAPP_KEYCODE_KP_DIVIDE        = 331,
    SAPP_KEYCODE_KP_MULTIPLY      = 332,
    SAPP_KEYCODE_KP_SUBTRACT      = 333,
    SAPP_KEYCODE_KP_ADD           = 334,
    SAPP_KEYCODE_KP_ENTER         = 335,
    SAPP_KEYCODE_KP_EQUAL         = 336,
    SAPP_KEYCODE_LEFT_SHIFT       = 340,
    SAPP_KEYCODE_LEFT_CONTROL     = 341,
    SAPP_KEYCODE_LEFT_ALT         = 342,
    SAPP_KEYCODE_LEFT_SUPER       = 343,
    SAPP_KEYCODE_RIGHT_SHIFT      = 344,
    SAPP_KEYCODE_RIGHT_CONTROL    = 345,
    SAPP_KEYCODE_RIGHT_ALT        = 346,
    SAPP_KEYCODE_RIGHT_SUPER      = 347,
    SAPP_KEYCODE_MENU             = 348,
} sapp_keycode;

/*
    Android specific 'tool type' enum for touch events. This lets the
    application check what type of input device was used for
    touch events.

    NOTE: the values must remain in sync with the corresponding
    Android SDK type, so don't change those.

    See https://developer.android.com/reference/android/view/MotionEvent#TOOL_TYPE_UNKNOWN
*/
typedef enum sapp_android_tooltype {
    SAPP_ANDROIDTOOLTYPE_UNKNOWN = 0,   // TOOL_TYPE_UNKNOWN
    SAPP_ANDROIDTOOLTYPE_FINGER = 1,    // TOOL_TYPE_FINGER
    SAPP_ANDROIDTOOLTYPE_STYLUS = 2,    // TOOL_TYPE_STYLUS
    SAPP_ANDROIDTOOLTYPE_MOUSE = 3,     // TOOL_TYPE_MOUSE
} sapp_android_tooltype;

/*
    sapp_touchpoint

    Describes a single touchpoint in a multitouch event (TOUCHES_BEGAN,
    TOUCHES_MOVED, TOUCHES_ENDED).

    Touch points are stored in the nested array sapp_event.touches[],
    and the number of touches is stored in sapp_event.num_touches.
*/
typedef struct sapp_touchpoint {
    uintptr_t identifier;
    float pos_x;
    float pos_y;
    sapp_android_tooltype android_tooltype; // only valid on Android
    bool changed;
} sapp_touchpoint;

/*
    sapp_mousebutton

    The currently pressed mouse button in the events MOUSE_DOWN
    and MOUSE_UP, stored in the struct field sapp_event.mouse_button.
*/
typedef enum sapp_mousebutton {
    SAPP_MOUSEBUTTON_LEFT = 0x0,
    SAPP_MOUSEBUTTON_RIGHT = 0x1,
    SAPP_MOUSEBUTTON_MIDDLE = 0x2,
    SAPP_MOUSEBUTTON_INVALID = 0x100,
} sapp_mousebutton;

/*
    These are currently pressed modifier keys (and mouse buttons) which are
    passed in the event struct field sapp_event.modifiers.
*/
enum {
    SAPP_MODIFIER_SHIFT = 0x1,      // left or right shift key
    SAPP_MODIFIER_CTRL  = 0x2,      // left or right control key
    SAPP_MODIFIER_ALT   = 0x4,      // left or right alt key
    SAPP_MODIFIER_SUPER = 0x8,      // left or right 'super' key
    SAPP_MODIFIER_LMB   = 0x100,    // left mouse button
    SAPP_MODIFIER_RMB   = 0x200,    // right mouse button
    SAPP_MODIFIER_MMB   = 0x400,    // middle mouse button
};

/*
    sapp_event

    This is an all-in-one event struct passed to the event handler
    user callback function. Note that it depends on the event
    type what struct fields actually contain useful values, so you
    should first check the event type before reading other struct
    fields.
*/
typedef struct sapp_event {
    uint64_t frame_count;               // current frame counter, always valid, useful for checking if two events were issued in the same frame
    sapp_event_type type;               // the event type, always valid
    sapp_keycode key_code;              // the virtual key code, only valid in KEY_UP, KEY_DOWN
    uint32_t char_code;                 // the UTF-32 character code, only valid in CHAR events
    bool key_repeat;                    // true if this is a key-repeat event, valid in KEY_UP, KEY_DOWN and CHAR
    uint32_t modifiers;                 // current modifier keys, valid in all key-, char- and mouse-events
    sapp_mousebutton mouse_button;      // mouse button that was pressed or released, valid in MOUSE_DOWN, MOUSE_UP
    float mouse_x;                      // current horizontal mouse position in pixels, always valid except during mouse lock
    float mouse_y;                      // current vertical mouse position in pixels, always valid except during mouse lock
    float mouse_dx;                     // relative horizontal mouse movement since last frame, always valid
    float mouse_dy;                     // relative vertical mouse movement since last frame, always valid
    float scroll_x;                     // horizontal mouse wheel scroll distance, valid in MOUSE_SCROLL events
    float scroll_y;                     // vertical mouse wheel scroll distance, valid in MOUSE_SCROLL events
    int num_touches;                    // number of valid items in the touches[] array
    sapp_touchpoint touches[SAPP_MAX_TOUCHPOINTS];  // current touch points, valid in TOUCHES_BEGIN, TOUCHES_MOVED, TOUCHES_ENDED
    int window_width;                   // current window- and framebuffer sizes in pixels, always valid
    int window_height;
    int framebuffer_width;              // = window_width * dpi_scale
    int framebuffer_height;             // = window_height * dpi_scale
} sapp_event;

/*
    sg_range

    A general pointer/size-pair struct and constructor macros for passing binary blobs
    into sokol_app.h.
*/
typedef struct sapp_range {
    const void* ptr;
    size_t size;
} sapp_range;
// disabling this for every includer isn't great, but the warnings are also quite pointless

/*
    sapp_image_desc

    This is used to describe image data to sokol_app.h (at first, window
    icons, later maybe cursor images).

    Note that the actual image pixel format depends on the use case:

    - window icon pixels are RGBA8
*/
typedef struct sapp_image_desc {
    int width;
    int height;
    sapp_range pixels;
} sapp_image_desc;

/*
    sapp_icon_desc

    An icon description structure for use in sapp_desc.icon and
    sapp_set_icon().

    When setting a custom image, the application can provide a number of
    candidates differing in size, and sokol_app.h will pick the image(s)
    closest to the size expected by the platform's window system.

    To set sokol-app's default icon, set .sokol_default to true.

    Otherwise provide candidate images of different sizes in the
    images[] array.

    If both the sokol_default flag is set to true, any image candidates
    will be ignored and the sokol_app.h default icon will be set.
*/
typedef struct sapp_icon_desc {
    bool sokol_default;
    sapp_image_desc images[SAPP_MAX_ICONIMAGES];
} sapp_icon_desc;

/*
    sapp_allocator

    Used in sapp_desc to provide custom memory-alloc and -free functions
    to sokol_app.h. If memory management should be overridden, both the
    alloc_fn and free_fn function must be provided (e.g. it's not valid to
    override one function but not the other).
*/
typedef struct sapp_allocator {
    void* (*alloc_fn)(size_t size, void* user_data);
    void (*free_fn)(void* ptr, void* user_data);
    void* user_data;
} sapp_allocator;

/*
    sapp_log_item

    Log items are defined via X-Macros and expanded to an enum
    'sapp_log_item', and in debug mode to corresponding
    human readable error messages.
*/
#define _SAPP_LOG_ITEMS \
    _SAPP_LOGITEM_XMACRO(OK, "Ok") \
    _SAPP_LOGITEM_XMACRO(MALLOC_FAILED, "memory allocation failed") \
    _SAPP_LOGITEM_XMACRO(MACOS_INVALID_NSOPENGL_PROFILE, "macos: invalid NSOpenGLProfile (valid choices are 1.0, 3.2 and 4.1)") \
    _SAPP_LOGITEM_XMACRO(WIN32_LOAD_OPENGL32_DLL_FAILED, "failed loading opengl32.dll") \
    _SAPP_LOGITEM_XMACRO(WIN32_CREATE_HELPER_WINDOW_FAILED, "failed to create helper window") \
    _SAPP_LOGITEM_XMACRO(WIN32_HELPER_WINDOW_GETDC_FAILED, "failed to get helper window DC") \
    _SAPP_LOGITEM_XMACRO(WIN32_DUMMY_CONTEXT_SET_PIXELFORMAT_FAILED, "failed to set pixel format for dummy GL context") \
    _SAPP_LOGITEM_XMACRO(WIN32_CREATE_DUMMY_CONTEXT_FAILED, "failed to create dummy GL context") \
    _SAPP_LOGITEM_XMACRO(WIN32_DUMMY_CONTEXT_MAKE_CURRENT_FAILED, "failed to make dummy GL context current") \
    _SAPP_LOGITEM_XMACRO(WIN32_GET_PIXELFORMAT_ATTRIB_FAILED, "failed to get WGL pixel format attribute") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_FIND_PIXELFORMAT_FAILED, "failed to find matching WGL pixel format") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_DESCRIBE_PIXELFORMAT_FAILED, "failed to get pixel format descriptor") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_SET_PIXELFORMAT_FAILED, "failed to set selected pixel format") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_ARB_CREATE_CONTEXT_REQUIRED, "ARB_create_context required") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_ARB_CREATE_CONTEXT_PROFILE_REQUIRED, "ARB_create_context_profile required") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_OPENGL_3_2_NOT_SUPPORTED, "OpenGL 3.2 not supported by GL driver (ERROR_INVALID_VERSION_ARB)") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_OPENGL_PROFILE_NOT_SUPPORTED, "requested OpenGL profile not support by GL driver (ERROR_INVALID_PROFILE_ARB)") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_INCOMPATIBLE_DEVICE_CONTEXT, "CreateContextAttribsARB failed with ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB") \
    _SAPP_LOGITEM_XMACRO(WIN32_WGL_CREATE_CONTEXT_ATTRIBS_FAILED_OTHER, "CreateContextAttribsARB failed for other reason") \
    _SAPP_LOGITEM_XMACRO(WIN32_D3D11_CREATE_DEVICE_AND_SWAPCHAIN_WITH_DEBUG_FAILED, "D3D11CreateDeviceAndSwapChain() with D3D11_CREATE_DEVICE_DEBUG failed, retrying without debug flag.") \
    _SAPP_LOGITEM_XMACRO(WIN32_D3D11_GET_IDXGIFACTORY_FAILED, "could not obtain IDXGIFactory object") \
    _SAPP_LOGITEM_XMACRO(WIN32_D3D11_GET_IDXGIADAPTER_FAILED, "could not obtain IDXGIAdapter object") \
    _SAPP_LOGITEM_XMACRO(WIN32_D3D11_QUERY_INTERFACE_IDXGIDEVICE1_FAILED, "could not obtain IDXGIDevice1 interface") \
    _SAPP_LOGITEM_XMACRO(WIN32_REGISTER_RAW_INPUT_DEVICES_FAILED_MOUSE_LOCK, "RegisterRawInputDevices() failed (on mouse lock)") \
    _SAPP_LOGITEM_XMACRO(WIN32_REGISTER_RAW_INPUT_DEVICES_FAILED_MOUSE_UNLOCK, "RegisterRawInputDevices() failed (on mouse unlock)") \
    _SAPP_LOGITEM_XMACRO(WIN32_GET_RAW_INPUT_DATA_FAILED, "GetRawInputData() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_LOAD_LIBGL_FAILED, "failed to load libGL") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_LOAD_ENTRY_POINTS_FAILED, "failed to load GLX entry points") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_EXTENSION_NOT_FOUND, "GLX extension not found") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_QUERY_VERSION_FAILED, "failed to query GLX version") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_VERSION_TOO_LOW, "GLX version too low (need at least 1.3)") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_NO_GLXFBCONFIGS, "glXGetFBConfigs() returned no configs") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_NO_SUITABLE_GLXFBCONFIG, "failed to find a suitable GLXFBConfig") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_GET_VISUAL_FROM_FBCONFIG_FAILED, "glXGetVisualFromFBConfig failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_REQUIRED_EXTENSIONS_MISSING, "GLX extensions ARB_create_context and ARB_create_context_profile missing") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_CREATE_CONTEXT_FAILED, "Failed to create GL context via glXCreateContextAttribsARB") \
    _SAPP_LOGITEM_XMACRO(LINUX_GLX_CREATE_WINDOW_FAILED, "glXCreateWindow() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_X11_CREATE_WINDOW_FAILED, "XCreateWindow() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_BIND_OPENGL_API_FAILED, "eglBindAPI(EGL_OPENGL_API) failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_BIND_OPENGL_ES_API_FAILED, "eglBindAPI(EGL_OPENGL_ES_API) failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_GET_DISPLAY_FAILED, "eglGetDisplay() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_INITIALIZE_FAILED, "eglInitialize() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_NO_CONFIGS, "eglChooseConfig() returned no configs") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_NO_NATIVE_VISUAL, "eglGetConfigAttrib() for EGL_NATIVE_VISUAL_ID failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_GET_VISUAL_INFO_FAILED, "XGetVisualInfo() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_CREATE_WINDOW_SURFACE_FAILED, "eglCreateWindowSurface() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_CREATE_CONTEXT_FAILED, "eglCreateContext() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_EGL_MAKE_CURRENT_FAILED, "eglMakeCurrent() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_X11_OPEN_DISPLAY_FAILED, "XOpenDisplay() failed") \
    _SAPP_LOGITEM_XMACRO(LINUX_X11_QUERY_SYSTEM_DPI_FAILED, "failed to query system dpi value, assuming default 96.0") \
    _SAPP_LOGITEM_XMACRO(LINUX_X11_DROPPED_FILE_URI_WRONG_SCHEME, "dropped file URL doesn't start with 'file://'") \
    _SAPP_LOGITEM_XMACRO(ANDROID_UNSUPPORTED_INPUT_EVENT_INPUT_CB, "unsupported input event encountered in _sapp_android_input_cb()") \
    _SAPP_LOGITEM_XMACRO(ANDROID_UNSUPPORTED_INPUT_EVENT_MAIN_CB, "unsupported input event encountered in _sapp_android_main_cb()") \
    _SAPP_LOGITEM_XMACRO(ANDROID_READ_MSG_FAILED, "failed to read message in _sapp_android_main_cb()") \
    _SAPP_LOGITEM_XMACRO(ANDROID_WRITE_MSG_FAILED, "failed to write message in _sapp_android_msg") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_CREATE, "MSG_CREATE") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_RESUME, "MSG_RESUME") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_PAUSE, "MSG_PAUSE") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_FOCUS, "MSG_FOCUS") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_NO_FOCUS, "MSG_NO_FOCUS") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_SET_NATIVE_WINDOW, "MSG_SET_NATIVE_WINDOW") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_SET_INPUT_QUEUE, "MSG_SET_INPUT_QUEUE") \
    _SAPP_LOGITEM_XMACRO(ANDROID_MSG_DESTROY, "MSG_DESTROY") \
    _SAPP_LOGITEM_XMACRO(ANDROID_UNKNOWN_MSG, "unknown msg type received") \
    _SAPP_LOGITEM_XMACRO(ANDROID_LOOP_THREAD_STARTED, "loop thread started") \
    _SAPP_LOGITEM_XMACRO(ANDROID_LOOP_THREAD_DONE, "loop thread done") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONSTART, "NativeActivity onStart()") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONRESUME, "NativeActivity onResume") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONSAVEINSTANCESTATE, "NativeActivity onSaveInstanceState") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONWINDOWFOCUSCHANGED, "NativeActivity onWindowFocusChanged") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONPAUSE, "NativeActivity onPause") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONSTOP, "NativeActivity onStop()") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONNATIVEWINDOWCREATED, "NativeActivity onNativeWindowCreated") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONNATIVEWINDOWDESTROYED, "NativeActivity onNativeWindowDestroyed") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONINPUTQUEUECREATED, "NativeActivity onInputQueueCreated") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONINPUTQUEUEDESTROYED, "NativeActivity onInputQueueDestroyed") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONCONFIGURATIONCHANGED, "NativeActivity onConfigurationChanged") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONLOWMEMORY, "NativeActivity onLowMemory") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONDESTROY, "NativeActivity onDestroy") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_DONE, "NativeActivity done") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_ONCREATE, "NativeActivity onCreate") \
    _SAPP_LOGITEM_XMACRO(ANDROID_CREATE_THREAD_PIPE_FAILED, "failed to create thread pipe") \
    _SAPP_LOGITEM_XMACRO(ANDROID_NATIVE_ACTIVITY_CREATE_SUCCESS, "NativeActivity successfully created") \
    _SAPP_LOGITEM_XMACRO(IMAGE_DATA_SIZE_MISMATCH, "image data size mismatch (must be width*height*4 bytes)") \
    _SAPP_LOGITEM_XMACRO(DROPPED_FILE_PATH_TOO_LONG, "dropped file path too long (sapp_desc.max_dropped_filed_path_length)") \
    _SAPP_LOGITEM_XMACRO(CLIPBOARD_STRING_TOO_BIG, "clipboard string didn't fit into clipboard buffer") \

#define _SAPP_LOGITEM_XMACRO(item,msg) SAPP_LOGITEM_##item,
typedef enum sapp_log_item {
    _SAPP_LOG_ITEMS
} sapp_log_item;
#undef _SAPP_LOGITEM_XMACRO

/*
    sapp_logger

    Used in sapp_desc to provide a logging function. Please be aware that
    without logging function, sokol-app will be completely silent, e.g. it will
    not report errors or warnings. For maximum error verbosity, compile in
    debug mode (e.g. NDEBUG *not* defined) and install a logger (for instance
    the standard logging function from sokol_log.h).
*/
typedef struct sapp_logger {
    void (*func)(
        const char* tag,                // always "sapp"
        uint32_t log_level,             // 0=panic, 1=error, 2=warning, 3=info
        uint32_t log_item_id,           // SAPP_LOGITEM_*
        const char* message_or_null,    // a message string, may be nullptr in release mode
        uint32_t line_nr,               // line number in sokol_app.h
        const char* filename_or_null,   // source filename, may be nullptr in release mode
        void* user_data);
    void* user_data;
} sapp_logger;

typedef struct sapp_desc {
    void (*init_cb)(void);                  // these are the user-provided callbacks without user data
    void (*frame_cb)(void);
    void (*cleanup_cb)(void);
    void (*event_cb)(const sapp_event*);

    void* user_data;                        // these are the user-provided callbacks with user data
    void (*init_userdata_cb)(void*);
    void (*frame_userdata_cb)(void*);
    void (*cleanup_userdata_cb)(void*);
    void (*event_userdata_cb)(const sapp_event*, void*);

    int width;                          // the preferred width of the window / canvas
    int height;                         // the preferred height of the window / canvas
    int sample_count;                   // MSAA sample count
    int swap_interval;                  // the preferred swap interval (ignored on some platforms)
    bool high_dpi;                      // whether the rendering canvas is full-resolution on HighDPI displays
    bool fullscreen;                    // whether the window should be created in fullscreen mode
    bool alpha;                         // whether the framebuffer should have an alpha channel (ignored on some platforms)
    const char* window_title;           // the window title as UTF-8 encoded string
    bool enable_clipboard;              // enable clipboard access, default is false
    int clipboard_size;                 // max size of clipboard content in bytes
    bool enable_dragndrop;              // enable file dropping (drag'n'drop), default is false
    int max_dropped_files;              // max number of dropped files to process (default: 1)
    int max_dropped_file_path_length;   // max length in bytes of a dropped UTF-8 file path (default: 2048)
    sapp_icon_desc icon;                // the initial window icon to set
    sapp_allocator allocator;           // optional memory allocation overrides (default: malloc/free)
    sapp_logger logger;                 // logging callback override (default: NO LOGGING!)

    // backend-specific options
    int gl_major_version;               // override GL major and minor version (the default GL version is 3.2)
    int gl_minor_version;
    bool win32_console_utf8;            // if true, set the output console codepage to UTF-8
    bool win32_console_create;          // if true, attach stdout/stderr to a new console window
    bool win32_console_attach;          // if true, attach stdout/stderr to parent process
    bool ios_keyboard_resizes_canvas;   // if true, showing the iOS keyboard shrinks the canvas
} sapp_desc;

/*
    sapp_mouse_cursor

    Predefined cursor image definitions, set with sapp_set_mouse_cursor(sapp_mouse_cursor cursor)
*/
typedef enum sapp_mouse_cursor {
    SAPP_MOUSECURSOR_DEFAULT = 0,   // equivalent with system default cursor
    SAPP_MOUSECURSOR_ARROW,
    SAPP_MOUSECURSOR_IBEAM,
    SAPP_MOUSECURSOR_CROSSHAIR,
    SAPP_MOUSECURSOR_POINTING_HAND,
    SAPP_MOUSECURSOR_RESIZE_EW,
    SAPP_MOUSECURSOR_RESIZE_NS,
    SAPP_MOUSECURSOR_RESIZE_NWSE,
    SAPP_MOUSECURSOR_RESIZE_NESW,
    SAPP_MOUSECURSOR_RESIZE_ALL,
    SAPP_MOUSECURSOR_NOT_ALLOWED,
    _SAPP_MOUSECURSOR_NUM,
} sapp_mouse_cursor;

/* user-provided functions */
extern sapp_desc sokol_main(int argc, char* argv[]);

/* returns true after sokol-app has been initialized */
SOKOL_APP_API_DECL bool sapp_isvalid(void);
/* returns the current framebuffer width in pixels */
SOKOL_APP_API_DECL int sapp_width(void);
/* same as sapp_width(), but returns float */
SOKOL_APP_API_DECL float sapp_widthf(void);
/* returns the current framebuffer height in pixels */
SOKOL_APP_API_DECL int sapp_height(void);
/* same as sapp_height(), but returns float */
SOKOL_APP_API_DECL float sapp_heightf(void);
/* get default framebuffer color pixel format */
SOKOL_APP_API_DECL int sapp_color_format(void);
/* get default framebuffer depth pixel format */
SOKOL_APP_API_DECL int sapp_depth_format(void);
/* get default framebuffer sample count */
SOKOL_APP_API_DECL int sapp_sample_count(void);
/* returns true when high_dpi was requested and actually running in a high-dpi scenario */
SOKOL_APP_API_DECL bool sapp_high_dpi(void);
/* returns the dpi scaling factor (window pixels to framebuffer pixels) */
SOKOL_APP_API_DECL float sapp_dpi_scale(void);
/* show or hide the mobile device onscreen keyboard */
SOKOL_APP_API_DECL void sapp_show_keyboard(bool show);
/* return true if the mobile device onscreen keyboard is currently shown */
SOKOL_APP_API_DECL bool sapp_keyboard_shown(void);
/* query fullscreen mode */
SOKOL_APP_API_DECL bool sapp_is_fullscreen(void);
/* toggle fullscreen mode */
SOKOL_APP_API_DECL void sapp_toggle_fullscreen(void);
/* show or hide the mouse cursor */
SOKOL_APP_API_DECL void sapp_show_mouse(bool show);
/* show or hide the mouse cursor */
SOKOL_APP_API_DECL bool sapp_mouse_shown(void);
/* enable/disable mouse-pointer-lock mode */
SOKOL_APP_API_DECL void sapp_lock_mouse(bool lock);
/* return true if in mouse-pointer-lock mode (this may toggle a few frames later) */
SOKOL_APP_API_DECL bool sapp_mouse_locked(void);
/* set mouse cursor type */
SOKOL_APP_API_DECL void sapp_set_mouse_cursor(sapp_mouse_cursor cursor);
/* get current mouse cursor type */
SOKOL_APP_API_DECL sapp_mouse_cursor sapp_get_mouse_cursor(void);
/* return the userdata pointer optionally provided in sapp_desc */
SOKOL_APP_API_DECL void* sapp_userdata(void);
/* return a copy of the sapp_desc structure */
SOKOL_APP_API_DECL sapp_desc sapp_query_desc(void);
/* initiate a "soft quit" (sends SAPP_EVENTTYPE_QUIT_REQUESTED) */
SOKOL_APP_API_DECL void sapp_request_quit(void);
/* cancel a pending quit (when SAPP_EVENTTYPE_QUIT_REQUESTED has been received) */
SOKOL_APP_API_DECL void sapp_cancel_quit(void);
/* initiate a "hard quit" (quit application without sending SAPP_EVENTTYPE_QUIT_REQUESTED) */
SOKOL_APP_API_DECL void sapp_quit(void);
/* call from inside event callback to consume the current event (don't forward to platform) */
SOKOL_APP_API_DECL void sapp_consume_event(void);
/* get the current frame counter (for comparison with sapp_event.frame_count) */
SOKOL_APP_API_DECL uint64_t sapp_frame_count(void);
/* get an averaged/smoothed frame duration in seconds */
SOKOL_APP_API_DECL double sapp_frame_duration(void);
/* write string into clipboard */
SOKOL_APP_API_DECL void sapp_set_clipboard_string(const char* str);
/* read string from clipboard (usually during SAPP_EVENTTYPE_CLIPBOARD_PASTED) */
SOKOL_APP_API_DECL const char* sapp_get_clipboard_string(void);
/* set the window title (only on desktop platforms) */
SOKOL_APP_API_DECL void sapp_set_window_title(const char* str);
/* set the window icon (only on Windows and Linux) */
SOKOL_APP_API_DECL void sapp_set_icon(const sapp_icon_desc* icon_desc);
/* gets the total number of dropped files (after an SAPP_EVENTTYPE_FILES_DROPPED event) */
SOKOL_APP_API_DECL int sapp_get_num_dropped_files(void);
/* gets the dropped file paths */
SOKOL_APP_API_DECL const char* sapp_get_dropped_file_path(int index);

/* special run-function for SOKOL_NO_ENTRY (in standard mode this is an empty stub) */
SOKOL_APP_API_DECL void sapp_run(const sapp_desc* desc);

/* EGL: get EGLDisplay object */
SOKOL_APP_API_DECL const void* sapp_egl_get_display(void);
/* EGL: get EGLContext object */
SOKOL_APP_API_DECL const void* sapp_egl_get_context(void);

/* Metal: get bridged pointer to Metal device object */
SOKOL_APP_API_DECL const void* sapp_metal_get_device(void);
/* Metal: get bridged pointer to MTKView's current drawable of type CAMetalDrawable */
SOKOL_APP_API_DECL const void* sapp_metal_get_current_drawable(void);
/* Metal: get bridged pointer to MTKView's depth-stencil texture of type MTLTexture */
SOKOL_APP_API_DECL const void* sapp_metal_get_depth_stencil_texture(void);
/* Metal: get bridged pointer to MTKView's msaa-color-texture of type MTLTexture (may be null) */
SOKOL_APP_API_DECL const void* sapp_metal_get_msaa_color_texture(void);
/* macOS: get bridged pointer to macOS NSWindow */
SOKOL_APP_API_DECL const void* sapp_macos_get_window(void);
/* iOS: get bridged pointer to iOS UIWindow */
SOKOL_APP_API_DECL const void* sapp_ios_get_window(void);

/* D3D11: get pointer to ID3D11Device object */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_device(void);
/* D3D11: get pointer to ID3D11DeviceContext object */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_device_context(void);
/* D3D11: get pointer to IDXGISwapChain object */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_swap_chain(void);
/* D3D11: get pointer to ID3D11RenderTargetView object for rendering */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_render_view(void);
/* D3D11: get pointer ID3D11RenderTargetView object for msaa-resolve (may return null) */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_resolve_view(void);
/* D3D11: get pointer ID3D11DepthStencilView */
SOKOL_APP_API_DECL const void* sapp_d3d11_get_depth_stencil_view(void);
/* Win32: get the HWND window handle */
SOKOL_APP_API_DECL const void* sapp_win32_get_hwnd(void);

/* GL: get framebuffer object */
SOKOL_APP_API_DECL uint32_t sapp_gl_get_framebuffer(void);
/* GL: get major version (only valid for desktop GL) */
SOKOL_APP_API_DECL int sapp_gl_get_major_version(void);
/* GL: get minor version (only valid for desktop GL) */
SOKOL_APP_API_DECL int sapp_gl_get_minor_version(void);

/* Android: get native activity handle */
SOKOL_APP_API_DECL const void* sapp_android_get_native_activity(void);

#endif // SOKOL_APP_INCLUDED
