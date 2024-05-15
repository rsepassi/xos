#include "sokol_gfx.h"

// ██ ███    ███ ██████  ██      ███████ ███    ███ ███████ ███    ██ ████████  █████  ████████ ██  ██████  ███    ██
// ██ ████  ████ ██   ██ ██      ██      ████  ████ ██      ████   ██    ██    ██   ██    ██    ██ ██    ██ ████   ██
// ██ ██ ████ ██ ██████  ██      █████   ██ ████ ██ █████   ██ ██  ██    ██    ███████    ██    ██ ██    ██ ██ ██  ██
// ██ ██  ██  ██ ██      ██      ██      ██  ██  ██ ██      ██  ██ ██    ██    ██   ██    ██    ██ ██    ██ ██  ██ ██
// ██ ██      ██ ██      ███████ ███████ ██      ██ ███████ ██   ████    ██    ██   ██    ██    ██  ██████  ██   ████
//
// >>implementation

#if !(defined(SOKOL_GLES3)||defined(SOKOL_D3D11)||defined(SOKOL_METAL)||defined(SOKOL_DUMMY_BACKEND))
#error "Please select a backend with SOKOL_GLES3, SOKOL_D3D11, SOKOL_METAL, SOKOL_DUMMY_BACKEND"
#endif
#if defined(SOKOL_MALLOC) || defined(SOKOL_CALLOC) || defined(SOKOL_FREE)
#error "SOKOL_MALLOC/CALLOC/FREE macros are no longer supported, please use sg_desc.allocator to override memory allocation functions"
#endif

#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <float.h> // FLT_MAX

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

#if defined(SOKOL_TRACE_HOOKS)
#define _SG_TRACE_ARGS(fn, ...) if (_sg.hooks.fn) { _sg.hooks.fn(__VA_ARGS__, _sg.hooks.user_data); }
#define _SG_TRACE_NOARGS(fn) if (_sg.hooks.fn) { _sg.hooks.fn(_sg.hooks.user_data); }
#else
#define _SG_TRACE_ARGS(fn, ...)
#define _SG_TRACE_NOARGS(fn)
#endif

// default clear values
#ifndef SG_DEFAULT_CLEAR_RED
#define SG_DEFAULT_CLEAR_RED (0.5f)
#endif
#ifndef SG_DEFAULT_CLEAR_GREEN
#define SG_DEFAULT_CLEAR_GREEN (0.5f)
#endif
#ifndef SG_DEFAULT_CLEAR_BLUE
#define SG_DEFAULT_CLEAR_BLUE (0.5f)
#endif
#ifndef SG_DEFAULT_CLEAR_ALPHA
#define SG_DEFAULT_CLEAR_ALPHA (1.0f)
#endif
#ifndef SG_DEFAULT_CLEAR_DEPTH
#define SG_DEFAULT_CLEAR_DEPTH (1.0f)
#endif
#ifndef SG_DEFAULT_CLEAR_STENCIL
#define SG_DEFAULT_CLEAR_STENCIL (0)
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4505)   // unreferenced local function has been removed
#pragma warning(disable:4201)   // nonstandard extension used: nameless struct/union (needed by d3d11.h)
#pragma warning(disable:4054)   // 'type cast': from function pointer
#pragma warning(disable:4055)   // 'type cast': from data pointer
#endif

#if defined(SOKOL_D3D11)
    #ifndef D3D11_NO_HELPERS
    #define D3D11_NO_HELPERS
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <d3d11.h>
    #include <d3dcompiler.h>
    #ifdef _MSC_VER
    #pragma comment (lib, "kernel32")
    #pragma comment (lib, "user32")
    #pragma comment (lib, "dxgi")
    #pragma comment (lib, "d3d11")
    #endif
#elif defined(SOKOL_METAL)
    // see https://clang.llvm.org/docs/LanguageExtensions.html#automatic-reference-counting
    #if !defined(__cplusplus)
        #if __has_feature(objc_arc) && !__has_feature(objc_arc_fields)
            #error "sokol_gfx.h requires __has_feature(objc_arc_field) if ARC is enabled (use a more recent compiler version)"
        #endif
    #endif
    #include <TargetConditionals.h>
    #include <AvailabilityMacros.h>
    #if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
        #define _SG_TARGET_MACOS (1)
    #else
        #define _SG_TARGET_IOS (1)
        #if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
            #define _SG_TARGET_IOS_SIMULATOR (1)
        #endif
    #endif
    #import <Metal/Metal.h>
    #import <QuartzCore/CoreAnimation.h> // needed for CAMetalDrawable
#elif defined(SOKOL_GLES3)
    #define _SOKOL_ANY_GL (1)

    // include platform specific GL headers (or on Win32: use an embedded GL loader)
    #if !defined(SOKOL_EXTERNAL_GL_LOADER)
        #if defined(_WIN32)
        #elif defined(__APPLE__)
            #include <TargetConditionals.h>
            #ifndef GL_SILENCE_DEPRECATION
                #define GL_SILENCE_DEPRECATION
            #endif
            #if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
                #include <OpenGL/gl3.h>
            #else
                #include <OpenGLES/ES3/gl.h>
                #include <OpenGLES/ES3/glext.h>
            #endif
        #elif defined(__EMSCRIPTEN__) || defined(__ANDROID__)
            #if defined(SOKOL_GLES3)
                #include <GLES3/gl3.h>
            #endif
        #elif defined(__linux__) || defined(__unix__)
            #include <GLES3/gl3.h>
            #include <GLES3/gl3ext.h>
        #endif
    #endif

    // optional GL loader definitions (only on Win32)
    #if defined(_SOKOL_USE_WIN32_GL_LOADER)
        #define __gl_h_ 1
        #define __gl32_h_ 1
        #define __gl31_h_ 1
        #define __GL_H__ 1
        #define __glext_h_ 1
        #define __GLEXT_H_ 1
        #define __gltypes_h_ 1
        #define __glcorearb_h_ 1
        #define __gl_glcorearb_h_ 1
        #define GL_APIENTRY APIENTRY

        typedef unsigned int  GLenum;
        typedef unsigned int  GLuint;
        typedef int  GLsizei;
        typedef char  GLchar;
        typedef ptrdiff_t  GLintptr;
        typedef ptrdiff_t  GLsizeiptr;
        typedef double  GLclampd;
        typedef unsigned short  GLushort;
        typedef unsigned char  GLubyte;
        typedef unsigned char  GLboolean;
        typedef uint64_t  GLuint64;
        typedef double  GLdouble;
        typedef unsigned short  GLhalf;
        typedef float  GLclampf;
        typedef unsigned int  GLbitfield;
        typedef signed char  GLbyte;
        typedef short  GLshort;
        typedef void  GLvoid;
        typedef int64_t  GLint64;
        typedef float  GLfloat;
        typedef int  GLint;
        #define GL_INT_2_10_10_10_REV 0x8D9F
        #define GL_R32F 0x822E
        #define GL_PROGRAM_POINT_SIZE 0x8642
        #define GL_DEPTH_ATTACHMENT 0x8D00
        #define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
        #define GL_COLOR_ATTACHMENT2 0x8CE2
        #define GL_COLOR_ATTACHMENT0 0x8CE0
        #define GL_R16F 0x822D
        #define GL_COLOR_ATTACHMENT22 0x8CF6
        #define GL_DRAW_FRAMEBUFFER 0x8CA9
        #define GL_FRAMEBUFFER_COMPLETE 0x8CD5
        #define GL_NUM_EXTENSIONS 0x821D
        #define GL_INFO_LOG_LENGTH 0x8B84
        #define GL_VERTEX_SHADER 0x8B31
        #define GL_INCR 0x1E02
        #define GL_DYNAMIC_DRAW 0x88E8
        #define GL_STATIC_DRAW 0x88E4
        #define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
        #define GL_TEXTURE_CUBE_MAP 0x8513
        #define GL_FUNC_SUBTRACT 0x800A
        #define GL_FUNC_REVERSE_SUBTRACT 0x800B
        #define GL_CONSTANT_COLOR 0x8001
        #define GL_DECR_WRAP 0x8508
        #define GL_R8 0x8229
        #define GL_LINEAR_MIPMAP_LINEAR 0x2703
        #define GL_ELEMENT_ARRAY_BUFFER 0x8893
        #define GL_SHORT 0x1402
        #define GL_DEPTH_TEST 0x0B71
        #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
        #define GL_LINK_STATUS 0x8B82
        #define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
        #define GL_SAMPLE_ALPHA_TO_COVERAGE 0x809E
        #define GL_RGBA16F 0x881A
        #define GL_CONSTANT_ALPHA 0x8003
        #define GL_READ_FRAMEBUFFER 0x8CA8
        #define GL_TEXTURE0 0x84C0
        #define GL_TEXTURE_MIN_LOD 0x813A
        #define GL_CLAMP_TO_EDGE 0x812F
        #define GL_UNSIGNED_SHORT_5_6_5 0x8363
        #define GL_TEXTURE_WRAP_R 0x8072
        #define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
        #define GL_NEAREST_MIPMAP_NEAREST 0x2700
        #define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
        #define GL_SRC_ALPHA_SATURATE 0x0308
        #define GL_STREAM_DRAW 0x88E0
        #define GL_ONE 1
        #define GL_NEAREST_MIPMAP_LINEAR 0x2702
        #define GL_RGB10_A2 0x8059
        #define GL_RGBA8 0x8058
        #define GL_SRGB8_ALPHA8 0x8C43
        #define GL_COLOR_ATTACHMENT1 0x8CE1
        #define GL_RGBA4 0x8056
        #define GL_RGB8 0x8051
        #define GL_ARRAY_BUFFER 0x8892
        #define GL_STENCIL 0x1802
        #define GL_TEXTURE_2D 0x0DE1
        #define GL_DEPTH 0x1801
        #define GL_FRONT 0x0404
        #define GL_STENCIL_BUFFER_BIT 0x00000400
        #define GL_REPEAT 0x2901
        #define GL_RGBA 0x1908
        #define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
        #define GL_DECR 0x1E03
        #define GL_FRAGMENT_SHADER 0x8B30
        #define GL_FLOAT 0x1406
        #define GL_TEXTURE_MAX_LOD 0x813B
        #define GL_DEPTH_COMPONENT 0x1902
        #define GL_ONE_MINUS_DST_ALPHA 0x0305
        #define GL_COLOR 0x1800
        #define GL_TEXTURE_2D_ARRAY 0x8C1A
        #define GL_TRIANGLES 0x0004
        #define GL_UNSIGNED_BYTE 0x1401
        #define GL_TEXTURE_MAG_FILTER 0x2800
        #define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
        #define GL_NONE 0
        #define GL_SRC_COLOR 0x0300
        #define GL_BYTE 0x1400
        #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
        #define GL_LINE_STRIP 0x0003
        #define GL_TEXTURE_3D 0x806F
        #define GL_CW 0x0900
        #define GL_LINEAR 0x2601
        #define GL_RENDERBUFFER 0x8D41
        #define GL_GEQUAL 0x0206
        #define GL_COLOR_BUFFER_BIT 0x00004000
        #define GL_RGBA32F 0x8814
        #define GL_BLEND 0x0BE2
        #define GL_ONE_MINUS_SRC_ALPHA 0x0303
        #define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
        #define GL_TEXTURE_WRAP_T 0x2803
        #define GL_TEXTURE_WRAP_S 0x2802
        #define GL_TEXTURE_MIN_FILTER 0x2801
        #define GL_LINEAR_MIPMAP_NEAREST 0x2701
        #define GL_EXTENSIONS 0x1F03
        #define GL_NO_ERROR 0
        #define GL_REPLACE 0x1E01
        #define GL_KEEP 0x1E00
        #define GL_CCW 0x0901
        #define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
        #define GL_RGB 0x1907
        #define GL_TRIANGLE_STRIP 0x0005
        #define GL_FALSE 0
        #define GL_ZERO 0
        #define GL_CULL_FACE 0x0B44
        #define GL_INVERT 0x150A
        #define GL_INT 0x1404
        #define GL_UNSIGNED_INT 0x1405
        #define GL_UNSIGNED_SHORT 0x1403
        #define GL_NEAREST 0x2600
        #define GL_SCISSOR_TEST 0x0C11
        #define GL_LEQUAL 0x0203
        #define GL_STENCIL_TEST 0x0B90
        #define GL_DITHER 0x0BD0
        #define GL_DEPTH_COMPONENT32F 0x8CAC
        #define GL_EQUAL 0x0202
        #define GL_FRAMEBUFFER 0x8D40
        #define GL_RGB5 0x8050
        #define GL_LINES 0x0001
        #define GL_DEPTH_BUFFER_BIT 0x00000100
        #define GL_SRC_ALPHA 0x0302
        #define GL_INCR_WRAP 0x8507
        #define GL_LESS 0x0201
        #define GL_MULTISAMPLE 0x809D
        #define GL_FRAMEBUFFER_BINDING 0x8CA6
        #define GL_BACK 0x0405
        #define GL_ALWAYS 0x0207
        #define GL_FUNC_ADD 0x8006
        #define GL_ONE_MINUS_DST_COLOR 0x0307
        #define GL_NOTEQUAL 0x0205
        #define GL_DST_COLOR 0x0306
        #define GL_COMPILE_STATUS 0x8B81
        #define GL_RED 0x1903
        #define GL_COLOR_ATTACHMENT3 0x8CE3
        #define GL_DST_ALPHA 0x0304
        #define GL_RGB5_A1 0x8057
        #define GL_GREATER 0x0204
        #define GL_POLYGON_OFFSET_FILL 0x8037
        #define GL_TRUE 1
        #define GL_NEVER 0x0200
        #define GL_POINTS 0x0000
        #define GL_ONE_MINUS_SRC_COLOR 0x0301
        #define GL_MIRRORED_REPEAT 0x8370
        #define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
        #define GL_R11F_G11F_B10F 0x8C3A
        #define GL_UNSIGNED_INT_10F_11F_11F_REV 0x8C3B
        #define GL_RGB9_E5 0x8C3D
        #define GL_UNSIGNED_INT_5_9_9_9_REV 0x8C3E
        #define GL_RGBA32UI 0x8D70
        #define GL_RGB32UI 0x8D71
        #define GL_RGBA16UI 0x8D76
        #define GL_RGB16UI 0x8D77
        #define GL_RGBA8UI 0x8D7C
        #define GL_RGB8UI 0x8D7D
        #define GL_RGBA32I 0x8D82
        #define GL_RGB32I 0x8D83
        #define GL_RGBA16I 0x8D88
        #define GL_RGB16I 0x8D89
        #define GL_RGBA8I 0x8D8E
        #define GL_RGB8I 0x8D8F
        #define GL_RED_INTEGER 0x8D94
        #define GL_RG 0x8227
        #define GL_RG_INTEGER 0x8228
        #define GL_R8 0x8229
        #define GL_R16 0x822A
        #define GL_RG8 0x822B
        #define GL_RG16 0x822C
        #define GL_R16F 0x822D
        #define GL_R32F 0x822E
        #define GL_RG16F 0x822F
        #define GL_RG32F 0x8230
        #define GL_R8I 0x8231
        #define GL_R8UI 0x8232
        #define GL_R16I 0x8233
        #define GL_R16UI 0x8234
        #define GL_R32I 0x8235
        #define GL_R32UI 0x8236
        #define GL_RG8I 0x8237
        #define GL_RG8UI 0x8238
        #define GL_RG16I 0x8239
        #define GL_RG16UI 0x823A
        #define GL_RG32I 0x823B
        #define GL_RG32UI 0x823C
        #define GL_RGBA_INTEGER 0x8D99
        #define GL_R8_SNORM 0x8F94
        #define GL_RG8_SNORM 0x8F95
        #define GL_RGB8_SNORM 0x8F96
        #define GL_RGBA8_SNORM 0x8F97
        #define GL_R16_SNORM 0x8F98
        #define GL_RG16_SNORM 0x8F99
        #define GL_RGB16_SNORM 0x8F9A
        #define GL_RGBA16_SNORM 0x8F9B
        #define GL_RGBA16 0x805B
        #define GL_MAX_TEXTURE_SIZE 0x0D33
        #define GL_MAX_CUBE_MAP_TEXTURE_SIZE 0x851C
        #define GL_MAX_3D_TEXTURE_SIZE 0x8073
        #define GL_MAX_ARRAY_TEXTURE_LAYERS 0x88FF
        #define GL_MAX_VERTEX_ATTRIBS 0x8869
        #define GL_CLAMP_TO_BORDER 0x812D
        #define GL_TEXTURE_BORDER_COLOR 0x1004
        #define GL_CURRENT_PROGRAM 0x8B8D
        #define GL_MAX_VERTEX_UNIFORM_COMPONENTS  0x8B4A
        #define GL_UNPACK_ALIGNMENT 0x0CF5
        #define GL_FRAMEBUFFER_SRGB 0x8DB9
        #define GL_TEXTURE_COMPARE_MODE 0x884C
        #define GL_TEXTURE_COMPARE_FUNC 0x884D
        #define GL_COMPARE_REF_TO_TEXTURE 0x884E
        #define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
        #define GL_TEXTURE_MAX_LEVEL 0x813D
        #define GL_FRAMEBUFFER_UNDEFINED 0x8219
        #define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
        #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
        #define GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD
        #define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE 0x8D56
        #define GL_MAJOR_VERSION 0x821B
        #define GL_MINOR_VERSION 0x821C
    #endif

    #ifndef GL_UNSIGNED_INT_2_10_10_10_REV
    #define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
    #endif
    #ifndef GL_UNSIGNED_INT_24_8
    #define GL_UNSIGNED_INT_24_8 0x84FA
    #endif
    #ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
    #define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
    #endif
    #ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
    #endif
    #ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
    #endif
    #ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
    #endif
    #ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    #define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
    #endif
    #ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    #define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
    #endif
    #ifndef GL_COMPRESSED_RED_RGTC1
    #define GL_COMPRESSED_RED_RGTC1 0x8DBB
    #endif
    #ifndef GL_COMPRESSED_SIGNED_RED_RGTC1
    #define GL_COMPRESSED_SIGNED_RED_RGTC1 0x8DBC
    #endif
    #ifndef GL_COMPRESSED_RED_GREEN_RGTC2
    #define GL_COMPRESSED_RED_GREEN_RGTC2 0x8DBD
    #endif
    #ifndef GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2
    #define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2 0x8DBE
    #endif
    #ifndef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
    #define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
    #endif
    #ifndef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
    #define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
    #endif
    #ifndef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    #define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
    #endif
    #ifndef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    #define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
    #endif
    #ifndef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
    #define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
    #endif
    #ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
    #define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
    #endif
    #ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
    #define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
    #endif
    #ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
    #define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
    #endif
    #ifndef GL_COMPRESSED_RGB8_ETC2
    #define GL_COMPRESSED_RGB8_ETC2 0x9274
    #endif
    #ifndef GL_COMPRESSED_SRGB8_ETC2
    #define GL_COMPRESSED_SRGB8_ETC2 0x9275
    #endif
    #ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
    #define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
    #endif
    #ifndef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    #define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
    #endif
    #ifndef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    #define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
    #endif
    #ifndef GL_COMPRESSED_R11_EAC
    #define GL_COMPRESSED_R11_EAC 0x9270
    #endif
    #ifndef GL_COMPRESSED_SIGNED_R11_EAC
    #define GL_COMPRESSED_SIGNED_R11_EAC 0x9271
    #endif
    #ifndef GL_COMPRESSED_RG11_EAC
    #define GL_COMPRESSED_RG11_EAC 0x9272
    #endif
    #ifndef GL_COMPRESSED_SIGNED_RG11_EAC
    #define GL_COMPRESSED_SIGNED_RG11_EAC 0x9273
    #endif
    #ifndef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    #define GL_COMPRESSED_RGBA_ASTC_4x4_KHR 0x93B0
    #endif
    #ifndef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    #define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
    #endif
    #ifndef GL_DEPTH24_STENCIL8
    #define GL_DEPTH24_STENCIL8 0x88F0
    #endif
    #ifndef GL_HALF_FLOAT
    #define GL_HALF_FLOAT 0x140B
    #endif
    #ifndef GL_DEPTH_STENCIL
    #define GL_DEPTH_STENCIL 0x84F9
    #endif
    #ifndef GL_LUMINANCE
    #define GL_LUMINANCE 0x1909
    #endif
    #ifndef _SG_GL_CHECK_ERROR
    #define _SG_GL_CHECK_ERROR() { SOKOL_ASSERT(glGetError() == GL_NO_ERROR); }
    #endif
#endif

#if defined(SOKOL_GLES3)
    // on WebGL2, GL_FRAMEBUFFER_UNDEFINED technically doesn't exist (it is defined
    // in the Emscripten headers, but may not exist in other WebGL2 shims)
    // see: https://github.com/floooh/sokol/pull/933
    #ifndef GL_FRAMEBUFFER_UNDEFINED
    #define GL_FRAMEBUFFER_UNDEFINED 0x8219
    #endif
#endif

// make some GL constants generally available to simplify compilation,
// use of those constants will be filtered by runtime flags
#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#endif

// ███████ ████████ ██████  ██    ██  ██████ ████████ ███████
// ██         ██    ██   ██ ██    ██ ██         ██    ██
// ███████    ██    ██████  ██    ██ ██         ██    ███████
//      ██    ██    ██   ██ ██    ██ ██         ██         ██
// ███████    ██    ██   ██  ██████   ██████    ██    ███████
//
// >>structs
// resource pool slots
typedef struct {
    uint32_t id;
    sg_resource_state state;
} _sg_slot_t;

// resource pool housekeeping struct
typedef struct {
    int size;
    int queue_top;
    uint32_t* gen_ctrs;
    int* free_queue;
} _sg_pool_t;

_SOKOL_PRIVATE void _sg_init_pool(_sg_pool_t* pool, int num);
_SOKOL_PRIVATE void _sg_discard_pool(_sg_pool_t* pool);
_SOKOL_PRIVATE int _sg_pool_alloc_index(_sg_pool_t* pool);
_SOKOL_PRIVATE void _sg_pool_free_index(_sg_pool_t* pool, int slot_index);
_SOKOL_PRIVATE void _sg_reset_slot(_sg_slot_t* slot);
_SOKOL_PRIVATE uint32_t _sg_slot_alloc(_sg_pool_t* pool, _sg_slot_t* slot, int slot_index);
_SOKOL_PRIVATE int _sg_slot_index(uint32_t id);

// constants
enum {
    _SG_STRING_SIZE = 32,
    _SG_SLOT_SHIFT = 16,
    _SG_SLOT_MASK = (1<<_SG_SLOT_SHIFT)-1,
    _SG_MAX_POOL_SIZE = (1<<_SG_SLOT_SHIFT),
    _SG_DEFAULT_BUFFER_POOL_SIZE = 128,
    _SG_DEFAULT_IMAGE_POOL_SIZE = 128,
    _SG_DEFAULT_SAMPLER_POOL_SIZE = 64,
    _SG_DEFAULT_SHADER_POOL_SIZE = 32,
    _SG_DEFAULT_PIPELINE_POOL_SIZE = 64,
    _SG_DEFAULT_ATTACHMENTS_POOL_SIZE = 16,
    _SG_DEFAULT_UB_SIZE = 4 * 1024 * 1024,
    _SG_DEFAULT_MAX_COMMIT_LISTENERS = 1024,
};

// fixed-size string
typedef struct {
    char buf[_SG_STRING_SIZE];
} _sg_str_t;

// helper macros
#define _sg_def(val, def) (((val) == 0) ? (def) : (val))
#define _sg_def_flt(val, def) (((val) == 0.0f) ? (def) : (val))
#define _sg_min(a,b) (((a)<(b))?(a):(b))
#define _sg_max(a,b) (((a)>(b))?(a):(b))
#define _sg_clamp(v,v0,v1) (((v)<(v0))?(v0):(((v)>(v1))?(v1):(v)))
#define _sg_fequal(val,cmp,delta) ((((val)-(cmp))> -(delta))&&(((val)-(cmp))<(delta)))
#define _sg_ispow2(val) ((val&(val-1))==0)
#define _sg_stats_add(key,val) {if(_sg.stats_enabled){ _sg.stats.key+=val;}}

_SOKOL_PRIVATE void* _sg_malloc_clear(size_t size);
_SOKOL_PRIVATE void _sg_free(void* ptr);
_SOKOL_PRIVATE void _sg_clear(void* ptr, size_t size);

typedef struct {
    int size;
    int append_pos;
    bool append_overflow;
    uint32_t update_frame_index;
    uint32_t append_frame_index;
    int num_slots;
    int active_slot;
    sg_buffer_type type;
    sg_usage usage;
} _sg_buffer_common_t;

_SOKOL_PRIVATE void _sg_buffer_common_init(_sg_buffer_common_t* cmn, const sg_buffer_desc* desc) {
    cmn->size = (int)desc->size;
    cmn->append_pos = 0;
    cmn->append_overflow = false;
    cmn->update_frame_index = 0;
    cmn->append_frame_index = 0;
    cmn->num_slots = (desc->usage == SG_USAGE_IMMUTABLE) ? 1 : SG_NUM_INFLIGHT_FRAMES;
    cmn->active_slot = 0;
    cmn->type = desc->type;
    cmn->usage = desc->usage;
}

typedef struct {
    uint32_t upd_frame_index;
    int num_slots;
    int active_slot;
    sg_image_type type;
    bool render_target;
    int width;
    int height;
    int num_slices;
    int num_mipmaps;
    sg_usage usage;
    sg_pixel_format pixel_format;
    int sample_count;
} _sg_image_common_t;

_SOKOL_PRIVATE void _sg_image_common_init(_sg_image_common_t* cmn, const sg_image_desc* desc) {
    cmn->upd_frame_index = 0;
    cmn->num_slots = (desc->usage == SG_USAGE_IMMUTABLE) ? 1 : SG_NUM_INFLIGHT_FRAMES;
    cmn->active_slot = 0;
    cmn->type = desc->type;
    cmn->render_target = desc->render_target;
    cmn->width = desc->width;
    cmn->height = desc->height;
    cmn->num_slices = desc->num_slices;
    cmn->num_mipmaps = desc->num_mipmaps;
    cmn->usage = desc->usage;
    cmn->pixel_format = desc->pixel_format;
    cmn->sample_count = desc->sample_count;
}

typedef struct {
    sg_filter min_filter;
    sg_filter mag_filter;
    sg_filter mipmap_filter;
    sg_wrap wrap_u;
    sg_wrap wrap_v;
    sg_wrap wrap_w;
    float min_lod;
    float max_lod;
    sg_border_color border_color;
    sg_compare_func compare;
    uint32_t max_anisotropy;
} _sg_sampler_common_t;

_SOKOL_PRIVATE void _sg_sampler_common_init(_sg_sampler_common_t* cmn, const sg_sampler_desc* desc) {
    cmn->min_filter = desc->min_filter;
    cmn->mag_filter = desc->mag_filter;
    cmn->mipmap_filter = desc->mipmap_filter;
    cmn->wrap_u = desc->wrap_u;
    cmn->wrap_v = desc->wrap_v;
    cmn->wrap_w = desc->wrap_w;
    cmn->min_lod = desc->min_lod;
    cmn->max_lod = desc->max_lod;
    cmn->border_color = desc->border_color;
    cmn->compare = desc->compare;
    cmn->max_anisotropy = desc->max_anisotropy;
}

typedef struct {
    size_t size;
} _sg_shader_uniform_block_t;

typedef struct {
    bool used;
    bool readonly;
} _sg_shader_storage_buffer_t;

typedef struct {
    sg_image_type image_type;
    sg_image_sample_type sample_type;
    bool multisampled;
} _sg_shader_image_t;

typedef struct {
    sg_sampler_type sampler_type;
} _sg_shader_sampler_t;

// combined image sampler mappings, only needed on GL
typedef struct {
    int image_slot;
    int sampler_slot;
} _sg_shader_image_sampler_t;

typedef struct {
    int num_uniform_blocks;
    int num_storage_buffers;
    int num_images;
    int num_samplers;
    int num_image_samplers;
    _sg_shader_uniform_block_t uniform_blocks[SG_MAX_SHADERSTAGE_UBS];
    _sg_shader_storage_buffer_t storage_buffers[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
    _sg_shader_image_t images[SG_MAX_SHADERSTAGE_IMAGES];
    _sg_shader_sampler_t samplers[SG_MAX_SHADERSTAGE_SAMPLERS];
    _sg_shader_image_sampler_t image_samplers[SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS];
} _sg_shader_stage_t;

typedef struct {
    _sg_shader_stage_t stage[SG_NUM_SHADER_STAGES];
} _sg_shader_common_t;

_SOKOL_PRIVATE void _sg_shader_common_init(_sg_shader_common_t* cmn, const sg_shader_desc* desc) {
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        const sg_shader_stage_desc* stage_desc = (stage_index == SG_SHADERSTAGE_VS) ? &desc->vs : &desc->fs;
        _sg_shader_stage_t* stage = &cmn->stage[stage_index];
        SOKOL_ASSERT(stage->num_uniform_blocks == 0);
        for (int ub_index = 0; ub_index < SG_MAX_SHADERSTAGE_UBS; ub_index++) {
            const sg_shader_uniform_block_desc* ub_desc = &stage_desc->uniform_blocks[ub_index];
            if (0 == ub_desc->size) {
                break;
            }
            stage->uniform_blocks[ub_index].size = ub_desc->size;
            stage->num_uniform_blocks++;
        }
        SOKOL_ASSERT(stage->num_images == 0);
        for (int img_index = 0; img_index < SG_MAX_SHADERSTAGE_IMAGES; img_index++) {
            const sg_shader_image_desc* img_desc = &stage_desc->images[img_index];
            if (!img_desc->used) {
                break;
            }
            stage->images[img_index].multisampled = img_desc->multisampled;
            stage->images[img_index].image_type = img_desc->image_type;
            stage->images[img_index].sample_type = img_desc->sample_type;
            stage->num_images++;
        }
        SOKOL_ASSERT(stage->num_samplers == 0);
        for (int smp_index = 0; smp_index < SG_MAX_SHADERSTAGE_SAMPLERS; smp_index++) {
            const sg_shader_sampler_desc* smp_desc = &stage_desc->samplers[smp_index];
            if (!smp_desc->used) {
                break;
            }
            stage->samplers[smp_index].sampler_type = smp_desc->sampler_type;
            stage->num_samplers++;
        }
        SOKOL_ASSERT(stage->num_image_samplers == 0);
        for (int img_smp_index = 0; img_smp_index < SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS; img_smp_index++) {
            const sg_shader_image_sampler_pair_desc* img_smp_desc = &stage_desc->image_sampler_pairs[img_smp_index];
            if (!img_smp_desc->used) {
                break;
            }
            SOKOL_ASSERT((img_smp_desc->image_slot >= 0) && (img_smp_desc->image_slot < stage->num_images));
            stage->image_samplers[img_smp_index].image_slot = img_smp_desc->image_slot;
            SOKOL_ASSERT((img_smp_desc->sampler_slot >= 0) && (img_smp_desc->sampler_slot < stage->num_samplers));
            stage->image_samplers[img_smp_index].sampler_slot = img_smp_desc->sampler_slot;
            stage->num_image_samplers++;
        }
        SOKOL_ASSERT(stage->num_storage_buffers == 0);
        for (int sbuf_index = 0; sbuf_index < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; sbuf_index++) {
            const sg_shader_storage_buffer_desc* sbuf_desc = &stage_desc->storage_buffers[sbuf_index];
            if (!sbuf_desc->used) {
                break;
            }
            stage->storage_buffers[sbuf_index].used = sbuf_desc->used;
            stage->storage_buffers[sbuf_index].readonly = sbuf_desc->readonly;
            stage->num_storage_buffers++;
        }
    }
}

typedef struct {
    bool vertex_buffer_layout_active[SG_MAX_VERTEX_BUFFERS];
    bool use_instanced_draw;
    sg_shader shader_id;
    sg_vertex_layout_state layout;
    sg_depth_state depth;
    sg_stencil_state stencil;
    int color_count;
    sg_color_target_state colors[SG_MAX_COLOR_ATTACHMENTS];
    sg_primitive_type primitive_type;
    sg_index_type index_type;
    sg_cull_mode cull_mode;
    sg_face_winding face_winding;
    int sample_count;
    sg_color blend_color;
    bool alpha_to_coverage_enabled;
} _sg_pipeline_common_t;

_SOKOL_PRIVATE void _sg_pipeline_common_init(_sg_pipeline_common_t* cmn, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT((desc->color_count >= 0) && (desc->color_count <= SG_MAX_COLOR_ATTACHMENTS));
    for (int i = 0; i < SG_MAX_VERTEX_BUFFERS; i++) {
        cmn->vertex_buffer_layout_active[i] = false;
    }
    cmn->use_instanced_draw = false;
    cmn->shader_id = desc->shader;
    cmn->layout = desc->layout;
    cmn->depth = desc->depth;
    cmn->stencil = desc->stencil;
    cmn->color_count = desc->color_count;
    for (int i = 0; i < desc->color_count; i++) {
        cmn->colors[i] = desc->colors[i];
    }
    cmn->primitive_type = desc->primitive_type;
    cmn->index_type = desc->index_type;
    cmn->cull_mode = desc->cull_mode;
    cmn->face_winding = desc->face_winding;
    cmn->sample_count = desc->sample_count;
    cmn->blend_color = desc->blend_color;
    cmn->alpha_to_coverage_enabled = desc->alpha_to_coverage_enabled;
}

typedef struct {
    sg_image image_id;
    int mip_level;
    int slice;
} _sg_attachment_common_t;

typedef struct {
    int width;
    int height;
    int num_colors;
    _sg_attachment_common_t colors[SG_MAX_COLOR_ATTACHMENTS];
    _sg_attachment_common_t resolves[SG_MAX_COLOR_ATTACHMENTS];
    _sg_attachment_common_t depth_stencil;
} _sg_attachments_common_t;

_SOKOL_PRIVATE void _sg_attachment_common_init(_sg_attachment_common_t* cmn, const sg_attachment_desc* desc) {
    cmn->image_id = desc->image;
    cmn->mip_level = desc->mip_level;
    cmn->slice = desc->slice;
}

_SOKOL_PRIVATE void _sg_attachments_common_init(_sg_attachments_common_t* cmn, const sg_attachments_desc* desc, int width, int height) {
    SOKOL_ASSERT((width > 0) && (height > 0));
    cmn->width = width;
    cmn->height = height;
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        if (desc->colors[i].image.id != SG_INVALID_ID) {
            cmn->num_colors++;
            _sg_attachment_common_init(&cmn->colors[i], &desc->colors[i]);
            _sg_attachment_common_init(&cmn->resolves[i], &desc->resolves[i]);
        }
    }
    if (desc->depth_stencil.image.id != SG_INVALID_ID) {
        _sg_attachment_common_init(&cmn->depth_stencil, &desc->depth_stencil);
    }
}

#if defined(SOKOL_DUMMY_BACKEND)
typedef struct {
    _sg_slot_t slot;
    _sg_buffer_common_t cmn;
} _sg_dummy_buffer_t;
typedef _sg_dummy_buffer_t _sg_buffer_t;

typedef struct {
    _sg_slot_t slot;
    _sg_image_common_t cmn;
} _sg_dummy_image_t;
typedef _sg_dummy_image_t _sg_image_t;

typedef struct {
    _sg_slot_t slot;
    _sg_sampler_common_t cmn;
} _sg_dummy_sampler_t;
typedef _sg_dummy_sampler_t _sg_sampler_t;

typedef struct {
    _sg_slot_t slot;
    _sg_shader_common_t cmn;
} _sg_dummy_shader_t;
typedef _sg_dummy_shader_t _sg_shader_t;

typedef struct {
    _sg_slot_t slot;
    _sg_shader_t* shader;
    _sg_pipeline_common_t cmn;
} _sg_dummy_pipeline_t;
typedef _sg_dummy_pipeline_t _sg_pipeline_t;

typedef struct {
    _sg_image_t* image;
} _sg_dummy_attachment_t;

typedef struct {
    _sg_slot_t slot;
    _sg_attachments_common_t cmn;
    struct {
        _sg_dummy_attachment_t colors[SG_MAX_COLOR_ATTACHMENTS];
        _sg_dummy_attachment_t resolves[SG_MAX_COLOR_ATTACHMENTS];
        _sg_dummy_attachment_t depth_stencil;
    } dmy;
} _sg_dummy_attachments_t;
typedef _sg_dummy_attachments_t _sg_attachments_t;

#elif defined(_SOKOL_ANY_GL)

#define _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE (SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS * SG_NUM_SHADER_STAGES)
#define _SG_GL_STORAGEBUFFER_STAGE_INDEX_PITCH (SG_MAX_SHADERSTAGE_STORAGEBUFFERS)

typedef struct {
    _sg_slot_t slot;
    _sg_buffer_common_t cmn;
    struct {
        GLuint buf[SG_NUM_INFLIGHT_FRAMES];
        bool injected;  // if true, external buffers were injected with sg_buffer_desc.gl_buffers
    } gl;
} _sg_gl_buffer_t;
typedef _sg_gl_buffer_t _sg_buffer_t;

typedef struct {
    _sg_slot_t slot;
    _sg_image_common_t cmn;
    struct {
        GLenum target;
        GLuint msaa_render_buffer;
        GLuint tex[SG_NUM_INFLIGHT_FRAMES];
        bool injected;  // if true, external textures were injected with sg_image_desc.gl_textures
    } gl;
} _sg_gl_image_t;
typedef _sg_gl_image_t _sg_image_t;

typedef struct {
    _sg_slot_t slot;
    _sg_sampler_common_t cmn;
    struct {
        GLuint smp;
        bool injected;  // true if external sampler was injects in sg_sampler_desc.gl_sampler
    } gl;
} _sg_gl_sampler_t;
typedef _sg_gl_sampler_t _sg_sampler_t;

typedef struct {
    GLint gl_loc;
    sg_uniform_type type;
    uint16_t count;
    uint16_t offset;
} _sg_gl_uniform_t;

typedef struct {
    int num_uniforms;
    _sg_gl_uniform_t uniforms[SG_MAX_UB_MEMBERS];
} _sg_gl_uniform_block_t;

typedef struct {
    int gl_tex_slot;
} _sg_gl_shader_image_sampler_t;

typedef struct {
    _sg_str_t name;
} _sg_gl_shader_attr_t;

typedef struct {
    _sg_gl_uniform_block_t uniform_blocks[SG_MAX_SHADERSTAGE_UBS];
    _sg_gl_shader_image_sampler_t image_samplers[SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS];
} _sg_gl_shader_stage_t;

typedef struct {
    _sg_slot_t slot;
    _sg_shader_common_t cmn;
    struct {
        GLuint prog;
        _sg_gl_shader_attr_t attrs[SG_MAX_VERTEX_ATTRIBUTES];
        _sg_gl_shader_stage_t stage[SG_NUM_SHADER_STAGES];
    } gl;
} _sg_gl_shader_t;
typedef _sg_gl_shader_t _sg_shader_t;

typedef struct {
    int8_t vb_index;        // -1 if attr is not enabled
    int8_t divisor;         // -1 if not initialized
    uint8_t stride;
    uint8_t size;
    uint8_t normalized;
    int offset;
    GLenum type;
} _sg_gl_attr_t;

typedef struct {
    _sg_slot_t slot;
    _sg_pipeline_common_t cmn;
    _sg_shader_t* shader;
    struct {
        _sg_gl_attr_t attrs[SG_MAX_VERTEX_ATTRIBUTES];
        sg_depth_state depth;
        sg_stencil_state stencil;
        sg_primitive_type primitive_type;
        sg_blend_state blend;
        sg_color_mask color_write_mask[SG_MAX_COLOR_ATTACHMENTS];
        sg_cull_mode cull_mode;
        sg_face_winding face_winding;
        int sample_count;
        bool alpha_to_coverage_enabled;
    } gl;
} _sg_gl_pipeline_t;
typedef _sg_gl_pipeline_t _sg_pipeline_t;

typedef struct {
    _sg_image_t* image;
} _sg_gl_attachment_t;

typedef struct {
    _sg_slot_t slot;
    _sg_attachments_common_t cmn;
    struct {
        GLuint fb;
        _sg_gl_attachment_t colors[SG_MAX_COLOR_ATTACHMENTS];
        _sg_gl_attachment_t resolves[SG_MAX_COLOR_ATTACHMENTS];
        _sg_gl_attachment_t depth_stencil;
        GLuint msaa_resolve_framebuffer[SG_MAX_COLOR_ATTACHMENTS];
    } gl;
} _sg_gl_attachments_t;
typedef _sg_gl_attachments_t _sg_attachments_t;

typedef struct {
    _sg_gl_attr_t gl_attr;
    GLuint gl_vbuf;
} _sg_gl_cache_attr_t;

typedef struct {
    GLenum target;
    GLuint texture;
    GLuint sampler;
} _sg_gl_cache_texture_sampler_bind_slot;

typedef struct {
    sg_depth_state depth;
    sg_stencil_state stencil;
    sg_blend_state blend;
    sg_color_mask color_write_mask[SG_MAX_COLOR_ATTACHMENTS];
    sg_cull_mode cull_mode;
    sg_face_winding face_winding;
    bool polygon_offset_enabled;
    int sample_count;
    sg_color blend_color;
    bool alpha_to_coverage_enabled;
    _sg_gl_cache_attr_t attrs[SG_MAX_VERTEX_ATTRIBUTES];
    GLuint vertex_buffer;
    GLuint index_buffer;
    GLuint storage_buffer;  // general bind point
    GLuint stage_storage_buffers[SG_NUM_SHADER_STAGES][SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
    GLuint stored_vertex_buffer;
    GLuint stored_index_buffer;
    GLuint stored_storage_buffer;
    GLuint prog;
    _sg_gl_cache_texture_sampler_bind_slot texture_samplers[_SG_GL_TEXTURE_SAMPLER_CACHE_SIZE];
    _sg_gl_cache_texture_sampler_bind_slot stored_texture_sampler;
    int cur_ib_offset;
    GLenum cur_primitive_type;
    GLenum cur_index_type;
    GLenum cur_active_texture;
    _sg_pipeline_t* cur_pipeline;
    sg_pipeline cur_pipeline_id;
} _sg_gl_state_cache_t;

typedef struct {
    bool valid;
    GLuint vao;
    _sg_gl_state_cache_t cache;
    bool ext_anisotropic;
    GLint max_anisotropy;
    sg_store_action color_store_actions[SG_MAX_COLOR_ATTACHMENTS];
    sg_store_action depth_store_action;
    sg_store_action stencil_store_action;
    #if _SOKOL_USE_WIN32_GL_LOADER
    HINSTANCE opengl32_dll;
    #endif
} _sg_gl_backend_t;

#elif defined(SOKOL_D3D11)

typedef struct {
    _sg_slot_t slot;
    _sg_buffer_common_t cmn;
    struct {
        ID3D11Buffer* buf;
        ID3D11ShaderResourceView* srv;
    } d3d11;
} _sg_d3d11_buffer_t;
typedef _sg_d3d11_buffer_t _sg_buffer_t;

typedef struct {
    _sg_slot_t slot;
    _sg_image_common_t cmn;
    struct {
        DXGI_FORMAT format;
        ID3D11Texture2D* tex2d;
        ID3D11Texture3D* tex3d;
        ID3D11Resource* res;    // either tex2d or tex3d
        ID3D11ShaderResourceView* srv;
    } d3d11;
} _sg_d3d11_image_t;
typedef _sg_d3d11_image_t _sg_image_t;

typedef struct {
    _sg_slot_t slot;
    _sg_sampler_common_t cmn;
    struct {
        ID3D11SamplerState* smp;
    } d3d11;
} _sg_d3d11_sampler_t;
typedef _sg_d3d11_sampler_t _sg_sampler_t;

typedef struct {
    _sg_str_t sem_name;
    int sem_index;
} _sg_d3d11_shader_attr_t;

typedef struct {
    ID3D11Buffer* cbufs[SG_MAX_SHADERSTAGE_UBS];
} _sg_d3d11_shader_stage_t;

typedef struct {
    _sg_slot_t slot;
    _sg_shader_common_t cmn;
    struct {
        _sg_d3d11_shader_attr_t attrs[SG_MAX_VERTEX_ATTRIBUTES];
        _sg_d3d11_shader_stage_t stage[SG_NUM_SHADER_STAGES];
        ID3D11VertexShader* vs;
        ID3D11PixelShader* fs;
        void* vs_blob;
        size_t vs_blob_length;
    } d3d11;
} _sg_d3d11_shader_t;
typedef _sg_d3d11_shader_t _sg_shader_t;

typedef struct {
    _sg_slot_t slot;
    _sg_pipeline_common_t cmn;
    _sg_shader_t* shader;
    struct {
        UINT stencil_ref;
        UINT vb_strides[SG_MAX_VERTEX_BUFFERS];
        D3D_PRIMITIVE_TOPOLOGY topology;
        DXGI_FORMAT index_format;
        ID3D11InputLayout* il;
        ID3D11RasterizerState* rs;
        ID3D11DepthStencilState* dss;
        ID3D11BlendState* bs;
    } d3d11;
} _sg_d3d11_pipeline_t;
typedef _sg_d3d11_pipeline_t _sg_pipeline_t;

typedef struct {
    _sg_image_t* image;
    union {
        ID3D11RenderTargetView* rtv;
        ID3D11DepthStencilView* dsv;
    } view;
} _sg_d3d11_attachment_t;

typedef struct {
    _sg_slot_t slot;
    _sg_attachments_common_t cmn;
    struct {
        _sg_d3d11_attachment_t colors[SG_MAX_COLOR_ATTACHMENTS];
        _sg_d3d11_attachment_t resolves[SG_MAX_COLOR_ATTACHMENTS];
        _sg_d3d11_attachment_t depth_stencil;
    } d3d11;
} _sg_d3d11_attachments_t;
typedef _sg_d3d11_attachments_t _sg_attachments_t;

typedef struct {
    bool valid;
    ID3D11Device* dev;
    ID3D11DeviceContext* ctx;
    bool use_indexed_draw;
    bool use_instanced_draw;
    _sg_pipeline_t* cur_pipeline;
    sg_pipeline cur_pipeline_id;
    struct {
        ID3D11RenderTargetView* render_view;
        ID3D11RenderTargetView* resolve_view;
    } cur_pass;
    // on-demand loaded d3dcompiler_47.dll handles
    HINSTANCE d3dcompiler_dll;
    bool d3dcompiler_dll_load_failed;
    pD3DCompile D3DCompile_func;
    // global subresourcedata array for texture updates
    D3D11_SUBRESOURCE_DATA subres_data[SG_MAX_MIPMAPS * SG_MAX_TEXTUREARRAY_LAYERS];
} _sg_d3d11_backend_t;

#elif defined(SOKOL_METAL)

#if defined(_SG_TARGET_MACOS) || defined(_SG_TARGET_IOS_SIMULATOR)
#define _SG_MTL_UB_ALIGN (256)
#else
#define _SG_MTL_UB_ALIGN (16)
#endif
#define _SG_MTL_INVALID_SLOT_INDEX (0)

typedef struct {
    uint32_t frame_index;   // frame index at which it is safe to release this resource
    int slot_index;
} _sg_mtl_release_item_t;

typedef struct {
    NSMutableArray* pool;
    int num_slots;
    int free_queue_top;
    int* free_queue;
    int release_queue_front;
    int release_queue_back;
    _sg_mtl_release_item_t* release_queue;
} _sg_mtl_idpool_t;

typedef struct {
    _sg_slot_t slot;
    _sg_buffer_common_t cmn;
    struct {
        int buf[SG_NUM_INFLIGHT_FRAMES];  // index into _sg_mtl_pool
    } mtl;
} _sg_mtl_buffer_t;
typedef _sg_mtl_buffer_t _sg_buffer_t;

typedef struct {
    _sg_slot_t slot;
    _sg_image_common_t cmn;
    struct {
        int tex[SG_NUM_INFLIGHT_FRAMES];
    } mtl;
} _sg_mtl_image_t;
typedef _sg_mtl_image_t _sg_image_t;

typedef struct {
    _sg_slot_t slot;
    _sg_sampler_common_t cmn;
    struct {
        int sampler_state;
    } mtl;
} _sg_mtl_sampler_t;
typedef _sg_mtl_sampler_t _sg_sampler_t;

typedef struct {
    int mtl_lib;
    int mtl_func;
} _sg_mtl_shader_stage_t;

typedef struct {
    _sg_slot_t slot;
    _sg_shader_common_t cmn;
    struct {
        _sg_mtl_shader_stage_t stage[SG_NUM_SHADER_STAGES];
    } mtl;
} _sg_mtl_shader_t;
typedef _sg_mtl_shader_t _sg_shader_t;

typedef struct {
    _sg_slot_t slot;
    _sg_pipeline_common_t cmn;
    _sg_shader_t* shader;
    struct {
        MTLPrimitiveType prim_type;
        int index_size;
        MTLIndexType index_type;
        MTLCullMode cull_mode;
        MTLWinding winding;
        uint32_t stencil_ref;
        int rps;
        int dss;
    } mtl;
} _sg_mtl_pipeline_t;
typedef _sg_mtl_pipeline_t _sg_pipeline_t;

typedef struct {
    _sg_image_t* image;
} _sg_mtl_attachment_t;

typedef struct {
    _sg_slot_t slot;
    _sg_attachments_common_t cmn;
    struct {
        _sg_mtl_attachment_t colors[SG_MAX_COLOR_ATTACHMENTS];
        _sg_mtl_attachment_t resolves[SG_MAX_COLOR_ATTACHMENTS];
        _sg_mtl_attachment_t depth_stencil;
    } mtl;
} _sg_mtl_attachments_t;
typedef _sg_mtl_attachments_t _sg_attachments_t;

// resource binding state cache
typedef struct {
    const _sg_pipeline_t* cur_pipeline;
    sg_pipeline cur_pipeline_id;
    const _sg_buffer_t* cur_indexbuffer;
    sg_buffer cur_indexbuffer_id;
    int cur_indexbuffer_offset;
    int cur_vertexbuffer_offsets[SG_MAX_VERTEX_BUFFERS];
    sg_buffer cur_vertexbuffer_ids[SG_MAX_VERTEX_BUFFERS];
    sg_image cur_vs_image_ids[SG_MAX_SHADERSTAGE_IMAGES];
    sg_image cur_fs_image_ids[SG_MAX_SHADERSTAGE_IMAGES];
    sg_sampler cur_vs_sampler_ids[SG_MAX_SHADERSTAGE_SAMPLERS];
    sg_sampler cur_fs_sampler_ids[SG_MAX_SHADERSTAGE_SAMPLERS];
    sg_buffer cur_vs_storagebuffer_ids[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
    sg_buffer cur_fs_storagebuffer_ids[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
} _sg_mtl_state_cache_t;

typedef struct {
    bool valid;
    bool use_shared_storage_mode;
    uint32_t cur_frame_rotate_index;
    int ub_size;
    int cur_ub_offset;
    uint8_t* cur_ub_base_ptr;
    _sg_mtl_state_cache_t state_cache;
    _sg_mtl_idpool_t idpool;
    dispatch_semaphore_t sem;
    id<MTLDevice> device;
    id<MTLCommandQueue> cmd_queue;
    id<MTLCommandBuffer> cmd_buffer;
    id<MTLRenderCommandEncoder> cmd_encoder;
    id<CAMetalDrawable> cur_drawable;
    id<MTLBuffer> uniform_buffers[SG_NUM_INFLIGHT_FRAMES];
} _sg_mtl_backend_t;
#endif

// POOL STRUCTS

// this *MUST* remain 0
#define _SG_INVALID_SLOT_INDEX (0)

typedef struct {
    _sg_pool_t buffer_pool;
    _sg_pool_t image_pool;
    _sg_pool_t sampler_pool;
    _sg_pool_t shader_pool;
    _sg_pool_t pipeline_pool;
    _sg_pool_t attachments_pool;
    _sg_buffer_t* buffers;
    _sg_image_t* images;
    _sg_sampler_t* samplers;
    _sg_shader_t* shaders;
    _sg_pipeline_t* pipelines;
    _sg_attachments_t* attachments;
} _sg_pools_t;

typedef struct {
    int num;        // number of allocated commit listener items
    int upper;      // the current upper index (no valid items past this point)
    sg_commit_listener* items;
} _sg_commit_listeners_t;

// resolved resource bindings struct
typedef struct {
    _sg_pipeline_t* pip;
    int num_vbs;
    int num_vs_imgs;
    int num_vs_smps;
    int num_vs_sbufs;
    int num_fs_imgs;
    int num_fs_smps;
    int num_fs_sbufs;
    int vb_offsets[SG_MAX_VERTEX_BUFFERS];
    int ib_offset;
    _sg_buffer_t* vbs[SG_MAX_VERTEX_BUFFERS];
    _sg_buffer_t* ib;
    _sg_image_t* vs_imgs[SG_MAX_SHADERSTAGE_IMAGES];
    _sg_sampler_t* vs_smps[SG_MAX_SHADERSTAGE_SAMPLERS];
    _sg_buffer_t* vs_sbufs[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
    _sg_image_t* fs_imgs[SG_MAX_SHADERSTAGE_IMAGES];
    _sg_sampler_t* fs_smps[SG_MAX_SHADERSTAGE_SAMPLERS];
    _sg_buffer_t* fs_sbufs[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
} _sg_bindings_t;

typedef struct {
    bool sample;
    bool filter;
    bool render;
    bool blend;
    bool msaa;
    bool depth;
} _sg_pixelformat_info_t;

typedef struct {
    bool valid;
    sg_desc desc;       // original desc with default values patched in
    uint32_t frame_index;
    struct {
        bool valid;
        bool in_pass;
        sg_attachments atts_id;     // SG_INVALID_ID in a swapchain pass
        _sg_attachments_t* atts;    // 0 in a swapchain pass
        int width;
        int height;
        struct {
            sg_pixel_format color_fmt;
            sg_pixel_format depth_fmt;
            int sample_count;
        } swapchain;
    } cur_pass;
    sg_pipeline cur_pipeline;
    bool next_draw_valid;
    #if defined(SOKOL_DEBUG)
    sg_log_item validate_error;
    #endif
    _sg_pools_t pools;
    sg_backend backend;
    sg_features features;
    sg_limits limits;
    _sg_pixelformat_info_t formats[_SG_PIXELFORMAT_NUM];
    bool stats_enabled;
    sg_frame_stats stats;
    sg_frame_stats prev_stats;
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_backend_t gl;
    #elif defined(SOKOL_METAL)
    _sg_mtl_backend_t mtl;
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_backend_t d3d11;
    #endif
    #if defined(SOKOL_TRACE_HOOKS)
    sg_trace_hooks hooks;
    #endif
    _sg_commit_listeners_t commit_listeners;
} _sg_state_t;
static _sg_state_t _sg;

// ██       ██████   ██████   ██████  ██ ███    ██  ██████
// ██      ██    ██ ██       ██       ██ ████   ██ ██
// ██      ██    ██ ██   ███ ██   ███ ██ ██ ██  ██ ██   ███
// ██      ██    ██ ██    ██ ██    ██ ██ ██  ██ ██ ██    ██
// ███████  ██████   ██████   ██████  ██ ██   ████  ██████
//
// >>logging
#if defined(SOKOL_DEBUG)
#define _SG_LOGITEM_XMACRO(item,msg) #item ": " msg,
static const char* _sg_log_messages[] = {
    _SG_LOG_ITEMS
};
#undef _SG_LOGITEM_XMACRO
#endif // SOKOL_DEBUG

#define _SG_PANIC(code) _sg_log(SG_LOGITEM_ ##code, 0, 0, __LINE__)
#define _SG_ERROR(code) _sg_log(SG_LOGITEM_ ##code, 1, 0, __LINE__)
#define _SG_WARN(code) _sg_log(SG_LOGITEM_ ##code, 2, 0, __LINE__)
#define _SG_INFO(code) _sg_log(SG_LOGITEM_ ##code, 3, 0, __LINE__)
#define _SG_LOGMSG(code,msg) _sg_log(SG_LOGITEM_ ##code, 3, msg, __LINE__)
#define _SG_VALIDATE(cond,code) if (!(cond)){ _sg.validate_error = SG_LOGITEM_ ##code; _sg_log(SG_LOGITEM_ ##code, 1, 0, __LINE__); }

static void _sg_log(sg_log_item log_item, uint32_t log_level, const char* msg, uint32_t line_nr) {
    if (_sg.desc.logger.func) {
        const char* filename = 0;
        #if defined(SOKOL_DEBUG)
            filename = __FILE__;
            if (0 == msg) {
                msg = _sg_log_messages[log_item];
            }
        #endif
        _sg.desc.logger.func("sg", log_level, log_item, msg, line_nr, filename, _sg.desc.logger.user_data);
    } else {
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

// a helper macro to clear a struct with potentially ARC'ed ObjC references
#if defined(SOKOL_METAL)
    #if defined(__cplusplus)
        #define _SG_CLEAR_ARC_STRUCT(type, item) { item = type(); }
    #else
        #define _SG_CLEAR_ARC_STRUCT(type, item) { item = (type) { 0 }; }
    #endif
#else
    #define _SG_CLEAR_ARC_STRUCT(type, item) { _sg_clear(&item, sizeof(item)); }
#endif

_SOKOL_PRIVATE void _sg_clear(void* ptr, size_t size) {
    SOKOL_ASSERT(ptr && (size > 0));
    memset(ptr, 0, size);
}

_SOKOL_PRIVATE void* _sg_malloc(size_t size) {
    SOKOL_ASSERT(size > 0);
    void* ptr;
    if (_sg.desc.allocator.alloc_fn) {
        ptr = _sg.desc.allocator.alloc_fn(size, _sg.desc.allocator.user_data);
    } else {
        ptr = malloc(size);
    }
    if (0 == ptr) {
        _SG_PANIC(MALLOC_FAILED);
    }
    return ptr;
}

_SOKOL_PRIVATE void* _sg_malloc_clear(size_t size) {
    void* ptr = _sg_malloc(size);
    _sg_clear(ptr, size);
    return ptr;
}

_SOKOL_PRIVATE void _sg_free(void* ptr) {
    if (_sg.desc.allocator.free_fn) {
        _sg.desc.allocator.free_fn(ptr, _sg.desc.allocator.user_data);
    } else {
        free(ptr);
    }
}

_SOKOL_PRIVATE bool _sg_strempty(const _sg_str_t* str) {
    return 0 == str->buf[0];
}

_SOKOL_PRIVATE const char* _sg_strptr(const _sg_str_t* str) {
    return &str->buf[0];
}

_SOKOL_PRIVATE void _sg_strcpy(_sg_str_t* dst, const char* src) {
    SOKOL_ASSERT(dst);
    if (src) {
        #if defined(_MSC_VER)
        strncpy_s(dst->buf, _SG_STRING_SIZE, src, (_SG_STRING_SIZE-1));
        #else
        strncpy(dst->buf, src, _SG_STRING_SIZE);
        #endif
        dst->buf[_SG_STRING_SIZE-1] = 0;
    } else {
        _sg_clear(dst->buf, _SG_STRING_SIZE);
    }
}

// ██   ██ ███████ ██      ██████  ███████ ██████  ███████
// ██   ██ ██      ██      ██   ██ ██      ██   ██ ██
// ███████ █████   ██      ██████  █████   ██████  ███████
// ██   ██ ██      ██      ██      ██      ██   ██      ██
// ██   ██ ███████ ███████ ██      ███████ ██   ██ ███████
//
// >>helpers
_SOKOL_PRIVATE uint32_t _sg_align_u32(uint32_t val, uint32_t align) {
    SOKOL_ASSERT((align > 0) && ((align & (align - 1)) == 0));
    return (val + (align - 1)) & ~(align - 1);
}

typedef struct { int x, y, w, h; } _sg_recti_t;

_SOKOL_PRIVATE _sg_recti_t _sg_clipi(int x, int y, int w, int h, int clip_width, int clip_height) {
    x = _sg_min(_sg_max(0, x), clip_width-1);
    y = _sg_min(_sg_max(0, y), clip_height-1);
    if ((x + w) > clip_width) {
        w = clip_width - x;
    }
    if ((y + h) > clip_height) {
        h = clip_height - y;
    }
    w = _sg_max(w, 1);
    h = _sg_max(h, 1);
    const _sg_recti_t res = { x, y, w, h };
    return res;
}

_SOKOL_PRIVATE int _sg_vertexformat_bytesize(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_FLOAT:     return 4;
        case SG_VERTEXFORMAT_FLOAT2:    return 8;
        case SG_VERTEXFORMAT_FLOAT3:    return 12;
        case SG_VERTEXFORMAT_FLOAT4:    return 16;
        case SG_VERTEXFORMAT_BYTE4:     return 4;
        case SG_VERTEXFORMAT_BYTE4N:    return 4;
        case SG_VERTEXFORMAT_UBYTE4:    return 4;
        case SG_VERTEXFORMAT_UBYTE4N:   return 4;
        case SG_VERTEXFORMAT_SHORT2:    return 4;
        case SG_VERTEXFORMAT_SHORT2N:   return 4;
        case SG_VERTEXFORMAT_USHORT2N:  return 4;
        case SG_VERTEXFORMAT_SHORT4:    return 8;
        case SG_VERTEXFORMAT_SHORT4N:   return 8;
        case SG_VERTEXFORMAT_USHORT4N:  return 8;
        case SG_VERTEXFORMAT_UINT10_N2: return 4;
        case SG_VERTEXFORMAT_HALF2:     return 4;
        case SG_VERTEXFORMAT_HALF4:     return 8;
        case SG_VERTEXFORMAT_INVALID:   return 0;
        default:
            SOKOL_UNREACHABLE;
            return -1;
    }
}

_SOKOL_PRIVATE uint32_t _sg_uniform_alignment(sg_uniform_type type, int array_count, sg_uniform_layout ub_layout) {
    if (ub_layout == SG_UNIFORMLAYOUT_NATIVE) {
        return 1;
    } else {
        SOKOL_ASSERT(array_count > 0);
        if (array_count == 1) {
            switch (type) {
                case SG_UNIFORMTYPE_FLOAT:
                case SG_UNIFORMTYPE_INT:
                    return 4;
                case SG_UNIFORMTYPE_FLOAT2:
                case SG_UNIFORMTYPE_INT2:
                    return 8;
                case SG_UNIFORMTYPE_FLOAT3:
                case SG_UNIFORMTYPE_FLOAT4:
                case SG_UNIFORMTYPE_INT3:
                case SG_UNIFORMTYPE_INT4:
                    return 16;
                case SG_UNIFORMTYPE_MAT4:
                    return 16;
                default:
                    SOKOL_UNREACHABLE;
                    return 1;
            }
        } else {
            return 16;
        }
    }
}

_SOKOL_PRIVATE uint32_t _sg_uniform_size(sg_uniform_type type, int array_count, sg_uniform_layout ub_layout) {
    SOKOL_ASSERT(array_count > 0);
    if (array_count == 1) {
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
            case SG_UNIFORMTYPE_INT:
                return 4;
            case SG_UNIFORMTYPE_FLOAT2:
            case SG_UNIFORMTYPE_INT2:
                return 8;
            case SG_UNIFORMTYPE_FLOAT3:
            case SG_UNIFORMTYPE_INT3:
                return 12;
            case SG_UNIFORMTYPE_FLOAT4:
            case SG_UNIFORMTYPE_INT4:
                return 16;
            case SG_UNIFORMTYPE_MAT4:
                return 64;
            default:
                SOKOL_UNREACHABLE;
                return 0;
        }
    } else {
        if (ub_layout == SG_UNIFORMLAYOUT_NATIVE) {
            switch (type) {
                case SG_UNIFORMTYPE_FLOAT:
                case SG_UNIFORMTYPE_INT:
                    return 4 * (uint32_t)array_count;
                case SG_UNIFORMTYPE_FLOAT2:
                case SG_UNIFORMTYPE_INT2:
                    return 8 * (uint32_t)array_count;
                case SG_UNIFORMTYPE_FLOAT3:
                case SG_UNIFORMTYPE_INT3:
                    return 12 * (uint32_t)array_count;
                case SG_UNIFORMTYPE_FLOAT4:
                case SG_UNIFORMTYPE_INT4:
                    return 16 * (uint32_t)array_count;
                case SG_UNIFORMTYPE_MAT4:
                    return 64 * (uint32_t)array_count;
                default:
                    SOKOL_UNREACHABLE;
                    return 0;
            }
        } else {
            switch (type) {
                case SG_UNIFORMTYPE_FLOAT:
                case SG_UNIFORMTYPE_FLOAT2:
                case SG_UNIFORMTYPE_FLOAT3:
                case SG_UNIFORMTYPE_FLOAT4:
                case SG_UNIFORMTYPE_INT:
                case SG_UNIFORMTYPE_INT2:
                case SG_UNIFORMTYPE_INT3:
                case SG_UNIFORMTYPE_INT4:
                    return 16 * (uint32_t)array_count;
                case SG_UNIFORMTYPE_MAT4:
                    return 64 * (uint32_t)array_count;
                default:
                    SOKOL_UNREACHABLE;
                    return 0;
            }
        }
    }
}

_SOKOL_PRIVATE bool _sg_is_compressed_pixel_format(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_BC1_RGBA:
        case SG_PIXELFORMAT_BC2_RGBA:
        case SG_PIXELFORMAT_BC3_RGBA:
        case SG_PIXELFORMAT_BC3_SRGBA:
        case SG_PIXELFORMAT_BC4_R:
        case SG_PIXELFORMAT_BC4_RSN:
        case SG_PIXELFORMAT_BC5_RG:
        case SG_PIXELFORMAT_BC5_RGSN:
        case SG_PIXELFORMAT_BC6H_RGBF:
        case SG_PIXELFORMAT_BC6H_RGBUF:
        case SG_PIXELFORMAT_BC7_RGBA:
        case SG_PIXELFORMAT_BC7_SRGBA:
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
        case SG_PIXELFORMAT_ETC2_RGB8:
        case SG_PIXELFORMAT_ETC2_SRGB8:
        case SG_PIXELFORMAT_ETC2_RGB8A1:
        case SG_PIXELFORMAT_ETC2_RGBA8:
        case SG_PIXELFORMAT_ETC2_SRGB8A8:
        case SG_PIXELFORMAT_EAC_R11:
        case SG_PIXELFORMAT_EAC_R11SN:
        case SG_PIXELFORMAT_EAC_RG11:
        case SG_PIXELFORMAT_EAC_RG11SN:
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:
            return true;
        default:
            return false;
    }
}

_SOKOL_PRIVATE bool _sg_is_valid_rendertarget_color_format(sg_pixel_format fmt) {
    const int fmt_index = (int) fmt;
    SOKOL_ASSERT((fmt_index >= 0) && (fmt_index < _SG_PIXELFORMAT_NUM));
    return _sg.formats[fmt_index].render && !_sg.formats[fmt_index].depth;
}

_SOKOL_PRIVATE bool _sg_is_valid_rendertarget_depth_format(sg_pixel_format fmt) {
    const int fmt_index = (int) fmt;
    SOKOL_ASSERT((fmt_index >= 0) && (fmt_index < _SG_PIXELFORMAT_NUM));
    return _sg.formats[fmt_index].render && _sg.formats[fmt_index].depth;
}

_SOKOL_PRIVATE bool _sg_is_depth_or_depth_stencil_format(sg_pixel_format fmt) {
    return (SG_PIXELFORMAT_DEPTH == fmt) || (SG_PIXELFORMAT_DEPTH_STENCIL == fmt);
}

_SOKOL_PRIVATE bool _sg_is_depth_stencil_format(sg_pixel_format fmt) {
    return (SG_PIXELFORMAT_DEPTH_STENCIL == fmt);
}

_SOKOL_PRIVATE int _sg_pixelformat_bytesize(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:
        case SG_PIXELFORMAT_R8SN:
        case SG_PIXELFORMAT_R8UI:
        case SG_PIXELFORMAT_R8SI:
            return 1;
        case SG_PIXELFORMAT_R16:
        case SG_PIXELFORMAT_R16SN:
        case SG_PIXELFORMAT_R16UI:
        case SG_PIXELFORMAT_R16SI:
        case SG_PIXELFORMAT_R16F:
        case SG_PIXELFORMAT_RG8:
        case SG_PIXELFORMAT_RG8SN:
        case SG_PIXELFORMAT_RG8UI:
        case SG_PIXELFORMAT_RG8SI:
            return 2;
        case SG_PIXELFORMAT_R32UI:
        case SG_PIXELFORMAT_R32SI:
        case SG_PIXELFORMAT_R32F:
        case SG_PIXELFORMAT_RG16:
        case SG_PIXELFORMAT_RG16SN:
        case SG_PIXELFORMAT_RG16UI:
        case SG_PIXELFORMAT_RG16SI:
        case SG_PIXELFORMAT_RG16F:
        case SG_PIXELFORMAT_RGBA8:
        case SG_PIXELFORMAT_SRGB8A8:
        case SG_PIXELFORMAT_RGBA8SN:
        case SG_PIXELFORMAT_RGBA8UI:
        case SG_PIXELFORMAT_RGBA8SI:
        case SG_PIXELFORMAT_BGRA8:
        case SG_PIXELFORMAT_RGB10A2:
        case SG_PIXELFORMAT_RG11B10F:
        case SG_PIXELFORMAT_RGB9E5:
            return 4;
        case SG_PIXELFORMAT_RG32UI:
        case SG_PIXELFORMAT_RG32SI:
        case SG_PIXELFORMAT_RG32F:
        case SG_PIXELFORMAT_RGBA16:
        case SG_PIXELFORMAT_RGBA16SN:
        case SG_PIXELFORMAT_RGBA16UI:
        case SG_PIXELFORMAT_RGBA16SI:
        case SG_PIXELFORMAT_RGBA16F:
            return 8;
        case SG_PIXELFORMAT_RGBA32UI:
        case SG_PIXELFORMAT_RGBA32SI:
        case SG_PIXELFORMAT_RGBA32F:
            return 16;
        case SG_PIXELFORMAT_DEPTH:
        case SG_PIXELFORMAT_DEPTH_STENCIL:
            return 4;
        default:
            SOKOL_UNREACHABLE;
            return 0;
    }
}

_SOKOL_PRIVATE int _sg_roundup(int val, int round_to) {
    return (val+(round_to-1)) & ~(round_to-1);
}

_SOKOL_PRIVATE uint32_t _sg_roundup_u32(uint32_t val, uint32_t round_to) {
    return (val+(round_to-1)) & ~(round_to-1);
}

_SOKOL_PRIVATE uint64_t _sg_roundup_u64(uint64_t val, uint64_t round_to) {
    return (val+(round_to-1)) & ~(round_to-1);
}

_SOKOL_PRIVATE bool _sg_multiple_u64(uint64_t val, uint64_t of) {
    return (val & (of-1)) == 0;
}

/* return row pitch for an image

    see ComputePitch in https://github.com/microsoft/DirectXTex/blob/master/DirectXTex/DirectXTexUtil.cpp

    For the special PVRTC pitch computation, see:
    GL extension requirement (https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt)

    Quote:

    6) How is the imageSize argument calculated for the CompressedTexImage2D
       and CompressedTexSubImage2D functions.

       Resolution: For PVRTC 4BPP formats the imageSize is calculated as:
          ( max(width, 8) * max(height, 8) * 4 + 7) / 8
       For PVRTC 2BPP formats the imageSize is calculated as:
          ( max(width, 16) * max(height, 8) * 2 + 7) / 8
*/
_SOKOL_PRIVATE int _sg_row_pitch(sg_pixel_format fmt, int width, int row_align) {
    int pitch;
    switch (fmt) {
        case SG_PIXELFORMAT_BC1_RGBA:
        case SG_PIXELFORMAT_BC4_R:
        case SG_PIXELFORMAT_BC4_RSN:
        case SG_PIXELFORMAT_ETC2_RGB8:
        case SG_PIXELFORMAT_ETC2_SRGB8:
        case SG_PIXELFORMAT_ETC2_RGB8A1:
        case SG_PIXELFORMAT_EAC_R11:
        case SG_PIXELFORMAT_EAC_R11SN:
            pitch = ((width + 3) / 4) * 8;
            pitch = pitch < 8 ? 8 : pitch;
            break;
        case SG_PIXELFORMAT_BC2_RGBA:
        case SG_PIXELFORMAT_BC3_RGBA:
        case SG_PIXELFORMAT_BC3_SRGBA:
        case SG_PIXELFORMAT_BC5_RG:
        case SG_PIXELFORMAT_BC5_RGSN:
        case SG_PIXELFORMAT_BC6H_RGBF:
        case SG_PIXELFORMAT_BC6H_RGBUF:
        case SG_PIXELFORMAT_BC7_RGBA:
        case SG_PIXELFORMAT_BC7_SRGBA:
        case SG_PIXELFORMAT_ETC2_RGBA8:
        case SG_PIXELFORMAT_ETC2_SRGB8A8:
        case SG_PIXELFORMAT_EAC_RG11:
        case SG_PIXELFORMAT_EAC_RG11SN:
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:
            pitch = ((width + 3) / 4) * 16;
            pitch = pitch < 16 ? 16 : pitch;
            break;
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
            pitch = (_sg_max(width, 8) * 4 + 7) / 8;
            break;
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
            pitch = (_sg_max(width, 16) * 2 + 7) / 8;
            break;
        default:
            pitch = width * _sg_pixelformat_bytesize(fmt);
            break;
    }
    pitch = _sg_roundup(pitch, row_align);
    return pitch;
}

// compute the number of rows in a surface depending on pixel format
_SOKOL_PRIVATE int _sg_num_rows(sg_pixel_format fmt, int height) {
    int num_rows;
    switch (fmt) {
        case SG_PIXELFORMAT_BC1_RGBA:
        case SG_PIXELFORMAT_BC4_R:
        case SG_PIXELFORMAT_BC4_RSN:
        case SG_PIXELFORMAT_ETC2_RGB8:
        case SG_PIXELFORMAT_ETC2_SRGB8:
        case SG_PIXELFORMAT_ETC2_RGB8A1:
        case SG_PIXELFORMAT_ETC2_RGBA8:
        case SG_PIXELFORMAT_ETC2_SRGB8A8:
        case SG_PIXELFORMAT_EAC_R11:
        case SG_PIXELFORMAT_EAC_R11SN:
        case SG_PIXELFORMAT_EAC_RG11:
        case SG_PIXELFORMAT_EAC_RG11SN:
        case SG_PIXELFORMAT_BC2_RGBA:
        case SG_PIXELFORMAT_BC3_RGBA:
        case SG_PIXELFORMAT_BC3_SRGBA:
        case SG_PIXELFORMAT_BC5_RG:
        case SG_PIXELFORMAT_BC5_RGSN:
        case SG_PIXELFORMAT_BC6H_RGBF:
        case SG_PIXELFORMAT_BC6H_RGBUF:
        case SG_PIXELFORMAT_BC7_RGBA:
        case SG_PIXELFORMAT_BC7_SRGBA:
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:
            num_rows = ((height + 3) / 4);
            break;
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
            /* NOTE: this is most likely not correct because it ignores any
                PVCRTC block size, but multiplied with _sg_row_pitch()
                it gives the correct surface pitch.

                See: https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt
            */
            num_rows = ((_sg_max(height, 8) + 7) / 8) * 8;
            break;
        default:
            num_rows = height;
            break;
    }
    if (num_rows < 1) {
        num_rows = 1;
    }
    return num_rows;
}

// return size of a mipmap level
_SOKOL_PRIVATE int _sg_miplevel_dim(int base_dim, int mip_level) {
    return _sg_max(base_dim >> mip_level, 1);
}

/* return pitch of a 2D subimage / texture slice
    see ComputePitch in https://github.com/microsoft/DirectXTex/blob/master/DirectXTex/DirectXTexUtil.cpp
*/
_SOKOL_PRIVATE int _sg_surface_pitch(sg_pixel_format fmt, int width, int height, int row_align) {
    int num_rows = _sg_num_rows(fmt, height);
    return num_rows * _sg_row_pitch(fmt, width, row_align);
}

// capability table pixel format helper functions
_SOKOL_PRIVATE void _sg_pixelformat_all(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->filter = true;
    pfi->blend = true;
    pfi->render = true;
    pfi->msaa = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_s(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sf(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->filter = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sr(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->render = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sfr(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->filter = true;
    pfi->render = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_srmd(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->render = true;
    pfi->msaa = true;
    pfi->depth = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_srm(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->render = true;
    pfi->msaa = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sfrm(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->filter = true;
    pfi->render = true;
    pfi->msaa = true;
}
_SOKOL_PRIVATE void _sg_pixelformat_sbrm(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->blend = true;
    pfi->render = true;
    pfi->msaa = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sbr(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->blend = true;
    pfi->render = true;
}

_SOKOL_PRIVATE void _sg_pixelformat_sfbr(_sg_pixelformat_info_t* pfi) {
    pfi->sample = true;
    pfi->filter = true;
    pfi->blend = true;
    pfi->render = true;
}

_SOKOL_PRIVATE sg_pass_action _sg_pass_action_defaults(const sg_pass_action* action) {
    SOKOL_ASSERT(action);
    sg_pass_action res = *action;
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        if (res.colors[i].load_action == _SG_LOADACTION_DEFAULT) {
            res.colors[i].load_action = SG_LOADACTION_CLEAR;
            res.colors[i].clear_value.r = SG_DEFAULT_CLEAR_RED;
            res.colors[i].clear_value.g = SG_DEFAULT_CLEAR_GREEN;
            res.colors[i].clear_value.b = SG_DEFAULT_CLEAR_BLUE;
            res.colors[i].clear_value.a = SG_DEFAULT_CLEAR_ALPHA;
        }
        if (res.colors[i].store_action == _SG_STOREACTION_DEFAULT) {
            res.colors[i].store_action = SG_STOREACTION_STORE;
        }
    }
    if (res.depth.load_action == _SG_LOADACTION_DEFAULT) {
        res.depth.load_action = SG_LOADACTION_CLEAR;
        res.depth.clear_value = SG_DEFAULT_CLEAR_DEPTH;
    }
    if (res.depth.store_action == _SG_STOREACTION_DEFAULT) {
        res.depth.store_action = SG_STOREACTION_DONTCARE;
    }
    if (res.stencil.load_action == _SG_LOADACTION_DEFAULT) {
        res.stencil.load_action = SG_LOADACTION_CLEAR;
        res.stencil.clear_value = SG_DEFAULT_CLEAR_STENCIL;
    }
    if (res.stencil.store_action == _SG_STOREACTION_DEFAULT) {
        res.stencil.store_action = SG_STOREACTION_DONTCARE;
    }
    return res;
}

#if defined(SOKOL_DUMMY_BACKEND)
#include "sokol_gfx_dummy.c"
#elif defined(_SOKOL_ANY_GL)
#include "sokol_gfx_gl.c"
#elif defined(SOKOL_D3D11)
#include "sokol_gfx_d3d11.c"
#elif defined(SOKOL_METAL)
#include "sokol_gfx_metal.c"
#endif

//  ██████  ███████ ███    ██ ███████ ██████  ██  ██████     ██████   █████   ██████ ██   ██ ███████ ███    ██ ██████
// ██       ██      ████   ██ ██      ██   ██ ██ ██          ██   ██ ██   ██ ██      ██  ██  ██      ████   ██ ██   ██
// ██   ███ █████   ██ ██  ██ █████   ██████  ██ ██          ██████  ███████ ██      █████   █████   ██ ██  ██ ██   ██
// ██    ██ ██      ██  ██ ██ ██      ██   ██ ██ ██          ██   ██ ██   ██ ██      ██  ██  ██      ██  ██ ██ ██   ██
//  ██████  ███████ ██   ████ ███████ ██   ██ ██  ██████     ██████  ██   ██  ██████ ██   ██ ███████ ██   ████ ██████
//
// >>generic backend
static inline void _sg_setup_backend(const sg_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_setup_backend(desc);
    #elif defined(SOKOL_METAL)
    _sg_mtl_setup_backend(desc);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_setup_backend(desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_setup_backend(desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_backend(void) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_backend();
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_backend();
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_backend();
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_backend();
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_reset_state_cache(void) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_reset_state_cache();
    #elif defined(SOKOL_METAL)
    _sg_mtl_reset_state_cache();
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_reset_state_cache();
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_reset_state_cache();
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_buffer(buf, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_buffer(buf, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_buffer(buf, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_buffer(buf, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_buffer(_sg_buffer_t* buf) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_buffer(buf);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_buffer(buf);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_buffer(buf);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_buffer(buf);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_image(_sg_image_t* img, const sg_image_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_image(img, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_image(img, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_image(img, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_image(img, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_image(_sg_image_t* img) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_image(img);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_image(img);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_image(img);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_image(img);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_sampler(smp, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_sampler(smp, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_sampler(smp, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_sampler(smp, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_sampler(_sg_sampler_t* smp) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_sampler(smp);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_sampler(smp);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_sampler(smp);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_sampler(smp);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_shader(shd, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_shader(shd, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_shader(shd, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_shader(shd, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_shader(_sg_shader_t* shd) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_shader(shd);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_shader(shd);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_shader(shd);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_shader(shd);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_pipeline(_sg_pipeline_t* pip, _sg_shader_t* shd, const sg_pipeline_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_pipeline(pip, shd, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_pipeline(pip, shd, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_pipeline(pip, shd, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_pipeline(pip, shd, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_pipeline(_sg_pipeline_t* pip) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_pipeline(pip);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_pipeline(pip);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_pipeline(pip);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_pipeline(pip);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline sg_resource_state _sg_create_attachments(_sg_attachments_t* atts, _sg_image_t** color_images, _sg_image_t** resolve_images, _sg_image_t* ds_image, const sg_attachments_desc* desc) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_create_attachments(atts, color_images, resolve_images, ds_image, desc);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_create_attachments(atts, color_images, resolve_images, ds_image, desc);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_create_attachments(atts, color_images, resolve_images, ds_image, desc);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_create_attachments(atts, color_images, resolve_images, ds_image, desc);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_discard_attachments(_sg_attachments_t* atts) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_discard_attachments(atts);
    #elif defined(SOKOL_METAL)
    _sg_mtl_discard_attachments(atts);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_discard_attachments(atts);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_discard_attachments(atts);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline _sg_image_t* _sg_attachments_color_image(const _sg_attachments_t* atts, int index) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_attachments_color_image(atts, index);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_attachments_color_image(atts, index);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_attachments_color_image(atts, index);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_attachments_color_image(atts, index);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline _sg_image_t* _sg_attachments_resolve_image(const _sg_attachments_t* atts, int index) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_attachments_resolve_image(atts, index);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_attachments_resolve_image(atts, index);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_attachments_resolve_image(atts, index);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_attachments_resolve_image(atts, index);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline _sg_image_t* _sg_attachments_ds_image(const _sg_attachments_t* atts) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_attachments_ds_image(atts);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_attachments_ds_image(atts);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_attachments_ds_image(atts);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_attachments_ds_image(atts);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_begin_pass(const sg_pass* pass) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_begin_pass(pass);
    #elif defined(SOKOL_METAL)
    _sg_mtl_begin_pass(pass);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_begin_pass(pass);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_begin_pass(pass);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_end_pass(void) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_end_pass();
    #elif defined(SOKOL_METAL)
    _sg_mtl_end_pass();
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_end_pass();
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_end_pass();
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_apply_viewport(int x, int y, int w, int h, bool origin_top_left) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_apply_viewport(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_METAL)
    _sg_mtl_apply_viewport(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_apply_viewport(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_apply_viewport(x, y, w, h, origin_top_left);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_apply_scissor_rect(int x, int y, int w, int h, bool origin_top_left) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_apply_scissor_rect(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_METAL)
    _sg_mtl_apply_scissor_rect(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_apply_scissor_rect(x, y, w, h, origin_top_left);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_apply_scissor_rect(x, y, w, h, origin_top_left);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_apply_pipeline(_sg_pipeline_t* pip) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_apply_pipeline(pip);
    #elif defined(SOKOL_METAL)
    _sg_mtl_apply_pipeline(pip);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_apply_pipeline(pip);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_apply_pipeline(pip);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline bool _sg_apply_bindings(_sg_bindings_t* bnd) {
    #if defined(_SOKOL_ANY_GL)
    return _sg_gl_apply_bindings(bnd);
    #elif defined(SOKOL_METAL)
    return _sg_mtl_apply_bindings(bnd);
    #elif defined(SOKOL_D3D11)
    return _sg_d3d11_apply_bindings(bnd);
    #elif defined(SOKOL_DUMMY_BACKEND)
    return _sg_dummy_apply_bindings(bnd);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_apply_uniforms(stage_index, ub_index, data);
    #elif defined(SOKOL_METAL)
    _sg_mtl_apply_uniforms(stage_index, ub_index, data);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_apply_uniforms(stage_index, ub_index, data);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_apply_uniforms(stage_index, ub_index, data);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_draw(int base_element, int num_elements, int num_instances) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_draw(base_element, num_elements, num_instances);
    #elif defined(SOKOL_METAL)
    _sg_mtl_draw(base_element, num_elements, num_instances);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_draw(base_element, num_elements, num_instances);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_draw(base_element, num_elements, num_instances);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_commit(void) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_commit();
    #elif defined(SOKOL_METAL)
    _sg_mtl_commit();
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_commit();
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_commit();
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_update_buffer(_sg_buffer_t* buf, const sg_range* data) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_update_buffer(buf, data);
    #elif defined(SOKOL_METAL)
    _sg_mtl_update_buffer(buf, data);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_update_buffer(buf, data);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_update_buffer(buf, data);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_append_buffer(_sg_buffer_t* buf, const sg_range* data, bool new_frame) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_append_buffer(buf, data, new_frame);
    #elif defined(SOKOL_METAL)
    _sg_mtl_append_buffer(buf, data, new_frame);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_append_buffer(buf, data, new_frame);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_append_buffer(buf, data, new_frame);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_update_image(_sg_image_t* img, const sg_image_data* data) {
    #if defined(_SOKOL_ANY_GL)
    _sg_gl_update_image(img, data);
    #elif defined(SOKOL_METAL)
    _sg_mtl_update_image(img, data);
    #elif defined(SOKOL_D3D11)
    _sg_d3d11_update_image(img, data);
    #elif defined(SOKOL_DUMMY_BACKEND)
    _sg_dummy_update_image(img, data);
    #else
    #error("INVALID BACKEND");
    #endif
}

static inline void _sg_push_debug_group(const char* name) {
    #if defined(SOKOL_METAL)
    _sg_mtl_push_debug_group(name);
    #else
    _SOKOL_UNUSED(name);
    #endif
}

static inline void _sg_pop_debug_group(void) {
    #if defined(SOKOL_METAL)
    _sg_mtl_pop_debug_group();
    #endif
}

// ██████   ██████   ██████  ██
// ██   ██ ██    ██ ██    ██ ██
// ██████  ██    ██ ██    ██ ██
// ██      ██    ██ ██    ██ ██
// ██       ██████   ██████  ███████
//
// >>pool
_SOKOL_PRIVATE void _sg_init_pool(_sg_pool_t* pool, int num) {
    SOKOL_ASSERT(pool && (num >= 1));
    // slot 0 is reserved for the 'invalid id', so bump the pool size by 1
    pool->size = num + 1;
    pool->queue_top = 0;
    // generation counters indexable by pool slot index, slot 0 is reserved
    size_t gen_ctrs_size = sizeof(uint32_t) * (size_t)pool->size;
    pool->gen_ctrs = (uint32_t*)_sg_malloc_clear(gen_ctrs_size);
    // it's not a bug to only reserve 'num' here
    pool->free_queue = (int*) _sg_malloc_clear(sizeof(int) * (size_t)num);
    // never allocate the zero-th pool item since the invalid id is 0
    for (int i = pool->size-1; i >= 1; i--) {
        pool->free_queue[pool->queue_top++] = i;
    }
}

_SOKOL_PRIVATE void _sg_discard_pool(_sg_pool_t* pool) {
    SOKOL_ASSERT(pool);
    SOKOL_ASSERT(pool->free_queue);
    _sg_free(pool->free_queue);
    pool->free_queue = 0;
    SOKOL_ASSERT(pool->gen_ctrs);
    _sg_free(pool->gen_ctrs);
    pool->gen_ctrs = 0;
    pool->size = 0;
    pool->queue_top = 0;
}

_SOKOL_PRIVATE int _sg_pool_alloc_index(_sg_pool_t* pool) {
    SOKOL_ASSERT(pool);
    SOKOL_ASSERT(pool->free_queue);
    if (pool->queue_top > 0) {
        int slot_index = pool->free_queue[--pool->queue_top];
        SOKOL_ASSERT((slot_index > 0) && (slot_index < pool->size));
        return slot_index;
    } else {
        // pool exhausted
        return _SG_INVALID_SLOT_INDEX;
    }
}

_SOKOL_PRIVATE void _sg_pool_free_index(_sg_pool_t* pool, int slot_index) {
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < pool->size));
    SOKOL_ASSERT(pool);
    SOKOL_ASSERT(pool->free_queue);
    SOKOL_ASSERT(pool->queue_top < pool->size);
    #ifdef SOKOL_DEBUG
    // debug check against double-free
    for (int i = 0; i < pool->queue_top; i++) {
        SOKOL_ASSERT(pool->free_queue[i] != slot_index);
    }
    #endif
    pool->free_queue[pool->queue_top++] = slot_index;
    SOKOL_ASSERT(pool->queue_top <= (pool->size-1));
}

_SOKOL_PRIVATE void _sg_reset_slot(_sg_slot_t* slot) {
    SOKOL_ASSERT(slot);
    _sg_clear(slot, sizeof(_sg_slot_t));
}

_SOKOL_PRIVATE void _sg_reset_buffer_to_alloc_state(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf);
    _sg_slot_t slot = buf->slot;
    _sg_clear(buf, sizeof(*buf));
    buf->slot = slot;
    buf->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_reset_image_to_alloc_state(_sg_image_t* img) {
    SOKOL_ASSERT(img);
    _sg_slot_t slot = img->slot;
    _sg_clear(img, sizeof(*img));
    img->slot = slot;
    img->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_reset_sampler_to_alloc_state(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp);
    _sg_slot_t slot = smp->slot;
    _sg_clear(smp, sizeof(*smp));
    smp->slot = slot;
    smp->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_reset_shader_to_alloc_state(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd);
    _sg_slot_t slot = shd->slot;
    _sg_clear(shd, sizeof(*shd));
    shd->slot = slot;
    shd->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_reset_pipeline_to_alloc_state(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    _sg_slot_t slot = pip->slot;
    _sg_clear(pip, sizeof(*pip));
    pip->slot = slot;
    pip->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_reset_attachments_to_alloc_state(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    _sg_slot_t slot = atts->slot;
    _sg_clear(atts, sizeof(*atts));
    atts->slot = slot;
    atts->slot.state = SG_RESOURCESTATE_ALLOC;
}

_SOKOL_PRIVATE void _sg_setup_pools(_sg_pools_t* p, const sg_desc* desc) {
    SOKOL_ASSERT(p);
    SOKOL_ASSERT(desc);
    // note: the pools here will have an additional item, since slot 0 is reserved
    SOKOL_ASSERT((desc->buffer_pool_size > 0) && (desc->buffer_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->buffer_pool, desc->buffer_pool_size);
    size_t buffer_pool_byte_size = sizeof(_sg_buffer_t) * (size_t)p->buffer_pool.size;
    p->buffers = (_sg_buffer_t*) _sg_malloc_clear(buffer_pool_byte_size);

    SOKOL_ASSERT((desc->image_pool_size > 0) && (desc->image_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->image_pool, desc->image_pool_size);
    size_t image_pool_byte_size = sizeof(_sg_image_t) * (size_t)p->image_pool.size;
    p->images = (_sg_image_t*) _sg_malloc_clear(image_pool_byte_size);

    SOKOL_ASSERT((desc->sampler_pool_size > 0) && (desc->sampler_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->sampler_pool, desc->sampler_pool_size);
    size_t sampler_pool_byte_size = sizeof(_sg_sampler_t) * (size_t)p->sampler_pool.size;
    p->samplers = (_sg_sampler_t*) _sg_malloc_clear(sampler_pool_byte_size);

    SOKOL_ASSERT((desc->shader_pool_size > 0) && (desc->shader_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->shader_pool, desc->shader_pool_size);
    size_t shader_pool_byte_size = sizeof(_sg_shader_t) * (size_t)p->shader_pool.size;
    p->shaders = (_sg_shader_t*) _sg_malloc_clear(shader_pool_byte_size);

    SOKOL_ASSERT((desc->pipeline_pool_size > 0) && (desc->pipeline_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->pipeline_pool, desc->pipeline_pool_size);
    size_t pipeline_pool_byte_size = sizeof(_sg_pipeline_t) * (size_t)p->pipeline_pool.size;
    p->pipelines = (_sg_pipeline_t*) _sg_malloc_clear(pipeline_pool_byte_size);

    SOKOL_ASSERT((desc->attachments_pool_size > 0) && (desc->attachments_pool_size < _SG_MAX_POOL_SIZE));
    _sg_init_pool(&p->attachments_pool, desc->attachments_pool_size);
    size_t attachments_pool_byte_size = sizeof(_sg_attachments_t) * (size_t)p->attachments_pool.size;
    p->attachments = (_sg_attachments_t*) _sg_malloc_clear(attachments_pool_byte_size);
}

_SOKOL_PRIVATE void _sg_discard_pools(_sg_pools_t* p) {
    SOKOL_ASSERT(p);
    _sg_free(p->attachments); p->attachments = 0;
    _sg_free(p->pipelines);   p->pipelines = 0;
    _sg_free(p->shaders);     p->shaders = 0;
    _sg_free(p->samplers);    p->samplers = 0;
    _sg_free(p->images);      p->images = 0;
    _sg_free(p->buffers);     p->buffers = 0;
    _sg_discard_pool(&p->attachments_pool);
    _sg_discard_pool(&p->pipeline_pool);
    _sg_discard_pool(&p->shader_pool);
    _sg_discard_pool(&p->sampler_pool);
    _sg_discard_pool(&p->image_pool);
    _sg_discard_pool(&p->buffer_pool);
}

/* allocate the slot at slot_index:
    - bump the slot's generation counter
    - create a resource id from the generation counter and slot index
    - set the slot's id to this id
    - set the slot's state to ALLOC
    - return the resource id
*/
_SOKOL_PRIVATE uint32_t _sg_slot_alloc(_sg_pool_t* pool, _sg_slot_t* slot, int slot_index) {
    /* FIXME: add handling for an overflowing generation counter,
       for now, just overflow (another option is to disable
       the slot)
    */
    SOKOL_ASSERT(pool && pool->gen_ctrs);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < pool->size));
    SOKOL_ASSERT(slot->id == SG_INVALID_ID);
    SOKOL_ASSERT(slot->state == SG_RESOURCESTATE_INITIAL);
    uint32_t ctr = ++pool->gen_ctrs[slot_index];
    slot->id = (ctr<<_SG_SLOT_SHIFT)|(slot_index & _SG_SLOT_MASK);
    slot->state = SG_RESOURCESTATE_ALLOC;
    return slot->id;
}

// extract slot index from id
_SOKOL_PRIVATE int _sg_slot_index(uint32_t id) {
    int slot_index = (int) (id & _SG_SLOT_MASK);
    SOKOL_ASSERT(_SG_INVALID_SLOT_INDEX != slot_index);
    return slot_index;
}

// returns pointer to resource by id without matching id check
_SOKOL_PRIVATE _sg_buffer_t* _sg_buffer_at(const _sg_pools_t* p, uint32_t buf_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != buf_id));
    int slot_index = _sg_slot_index(buf_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->buffer_pool.size));
    return &p->buffers[slot_index];
}

_SOKOL_PRIVATE _sg_image_t* _sg_image_at(const _sg_pools_t* p, uint32_t img_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != img_id));
    int slot_index = _sg_slot_index(img_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->image_pool.size));
    return &p->images[slot_index];
}

_SOKOL_PRIVATE _sg_sampler_t* _sg_sampler_at(const _sg_pools_t* p, uint32_t smp_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != smp_id));
    int slot_index = _sg_slot_index(smp_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->sampler_pool.size));
    return &p->samplers[slot_index];
}

_SOKOL_PRIVATE _sg_shader_t* _sg_shader_at(const _sg_pools_t* p, uint32_t shd_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != shd_id));
    int slot_index = _sg_slot_index(shd_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->shader_pool.size));
    return &p->shaders[slot_index];
}

_SOKOL_PRIVATE _sg_pipeline_t* _sg_pipeline_at(const _sg_pools_t* p, uint32_t pip_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != pip_id));
    int slot_index = _sg_slot_index(pip_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->pipeline_pool.size));
    return &p->pipelines[slot_index];
}

_SOKOL_PRIVATE _sg_attachments_t* _sg_attachments_at(const _sg_pools_t* p, uint32_t atts_id) {
    SOKOL_ASSERT(p && (SG_INVALID_ID != atts_id));
    int slot_index = _sg_slot_index(atts_id);
    SOKOL_ASSERT((slot_index > _SG_INVALID_SLOT_INDEX) && (slot_index < p->attachments_pool.size));
    return &p->attachments[slot_index];
}

// returns pointer to resource with matching id check, may return 0
_SOKOL_PRIVATE _sg_buffer_t* _sg_lookup_buffer(const _sg_pools_t* p, uint32_t buf_id) {
    if (SG_INVALID_ID != buf_id) {
        _sg_buffer_t* buf = _sg_buffer_at(p, buf_id);
        if (buf->slot.id == buf_id) {
            return buf;
        }
    }
    return 0;
}

_SOKOL_PRIVATE _sg_image_t* _sg_lookup_image(const _sg_pools_t* p, uint32_t img_id) {
    if (SG_INVALID_ID != img_id) {
        _sg_image_t* img = _sg_image_at(p, img_id);
        if (img->slot.id == img_id) {
            return img;
        }
    }
    return 0;
}

_SOKOL_PRIVATE _sg_sampler_t* _sg_lookup_sampler(const _sg_pools_t* p, uint32_t smp_id) {
    if (SG_INVALID_ID != smp_id) {
        _sg_sampler_t* smp = _sg_sampler_at(p, smp_id);
        if (smp->slot.id == smp_id) {
            return smp;
        }
    }
    return 0;
}

_SOKOL_PRIVATE _sg_shader_t* _sg_lookup_shader(const _sg_pools_t* p, uint32_t shd_id) {
    SOKOL_ASSERT(p);
    if (SG_INVALID_ID != shd_id) {
        _sg_shader_t* shd = _sg_shader_at(p, shd_id);
        if (shd->slot.id == shd_id) {
            return shd;
        }
    }
    return 0;
}

_SOKOL_PRIVATE _sg_pipeline_t* _sg_lookup_pipeline(const _sg_pools_t* p, uint32_t pip_id) {
    SOKOL_ASSERT(p);
    if (SG_INVALID_ID != pip_id) {
        _sg_pipeline_t* pip = _sg_pipeline_at(p, pip_id);
        if (pip->slot.id == pip_id) {
            return pip;
        }
    }
    return 0;
}

_SOKOL_PRIVATE _sg_attachments_t* _sg_lookup_attachments(const _sg_pools_t* p, uint32_t atts_id) {
    SOKOL_ASSERT(p);
    if (SG_INVALID_ID != atts_id) {
        _sg_attachments_t* atts = _sg_attachments_at(p, atts_id);
        if (atts->slot.id == atts_id) {
            return atts;
        }
    }
    return 0;
}

_SOKOL_PRIVATE void _sg_discard_all_resources(_sg_pools_t* p) {
    /*  this is a bit dumb since it loops over all pool slots to
        find the occupied slots, on the other hand it is only ever
        executed at shutdown
        NOTE: ONLY EXECUTE THIS AT SHUTDOWN
              ...because the free queues will not be reset
              and the resource slots not be cleared!
    */
    for (int i = 1; i < p->buffer_pool.size; i++) {
        sg_resource_state state = p->buffers[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_buffer(&p->buffers[i]);
        }
    }
    for (int i = 1; i < p->image_pool.size; i++) {
        sg_resource_state state = p->images[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_image(&p->images[i]);
        }
    }
    for (int i = 1; i < p->sampler_pool.size; i++) {
        sg_resource_state state = p->samplers[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_sampler(&p->samplers[i]);
        }
    }
    for (int i = 1; i < p->shader_pool.size; i++) {
        sg_resource_state state = p->shaders[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_shader(&p->shaders[i]);
        }
    }
    for (int i = 1; i < p->pipeline_pool.size; i++) {
        sg_resource_state state = p->pipelines[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_pipeline(&p->pipelines[i]);
        }
    }
    for (int i = 1; i < p->attachments_pool.size; i++) {
        sg_resource_state state = p->attachments[i].slot.state;
        if ((state == SG_RESOURCESTATE_VALID) || (state == SG_RESOURCESTATE_FAILED)) {
            _sg_discard_attachments(&p->attachments[i]);
        }
    }
}

// ██    ██  █████  ██      ██ ██████   █████  ████████ ██  ██████  ███    ██
// ██    ██ ██   ██ ██      ██ ██   ██ ██   ██    ██    ██ ██    ██ ████   ██
// ██    ██ ███████ ██      ██ ██   ██ ███████    ██    ██ ██    ██ ██ ██  ██
//  ██  ██  ██   ██ ██      ██ ██   ██ ██   ██    ██    ██ ██    ██ ██  ██ ██
//   ████   ██   ██ ███████ ██ ██████  ██   ██    ██    ██  ██████  ██   ████
//
// >>validation
#if defined(SOKOL_DEBUG)
_SOKOL_PRIVATE void _sg_validate_begin(void) {
    _sg.validate_error = SG_LOGITEM_OK;
}

_SOKOL_PRIVATE bool _sg_validate_end(void) {
    if (_sg.validate_error != SG_LOGITEM_OK) {
        #if !defined(SOKOL_VALIDATE_NON_FATAL)
            _SG_PANIC(VALIDATION_FAILED);
            return false;
        #else
            return false;
        #endif
    } else {
        return true;
    }
}
#endif

_SOKOL_PRIVATE bool _sg_validate_buffer_desc(const sg_buffer_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_BUFFERDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_BUFFERDESC_CANARY);
        _SG_VALIDATE(desc->size > 0, VALIDATE_BUFFERDESC_SIZE);
        bool injected = (0 != desc->gl_buffers[0]) ||
                        (0 != desc->mtl_buffers[0]) ||
                        (0 != desc->d3d11_buffer);
        if (!injected && (desc->usage == SG_USAGE_IMMUTABLE)) {
            _SG_VALIDATE((0 != desc->data.ptr) && (desc->data.size > 0), VALIDATE_BUFFERDESC_DATA);
            _SG_VALIDATE(desc->size == desc->data.size, VALIDATE_BUFFERDESC_DATA_SIZE);
        } else {
            _SG_VALIDATE(0 == desc->data.ptr, VALIDATE_BUFFERDESC_NO_DATA);
        }
        if (desc->type == SG_BUFFERTYPE_STORAGEBUFFER) {
            _SG_VALIDATE(_sg.features.storage_buffer, VALIDATE_BUFFERDESC_STORAGEBUFFER_SUPPORTED);
            _SG_VALIDATE(_sg_multiple_u64(desc->size, 4), VALIDATE_BUFFERDESC_STORAGEBUFFER_SIZE_MULTIPLE_4);
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE void _sg_validate_image_data(const sg_image_data* data, sg_pixel_format fmt, int width, int height, int num_faces, int num_mips, int num_slices) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(data);
        _SOKOL_UNUSED(fmt);
        _SOKOL_UNUSED(width);
        _SOKOL_UNUSED(height);
        _SOKOL_UNUSED(num_faces);
        _SOKOL_UNUSED(num_mips);
        _SOKOL_UNUSED(num_slices);
    #else
        for (int face_index = 0; face_index < num_faces; face_index++) {
            for (int mip_index = 0; mip_index < num_mips; mip_index++) {
                const bool has_data = data->subimage[face_index][mip_index].ptr != 0;
                const bool has_size = data->subimage[face_index][mip_index].size > 0;
                _SG_VALIDATE(has_data && has_size, VALIDATE_IMAGEDATA_NODATA);
                const int mip_width = _sg_miplevel_dim(width, mip_index);
                const int mip_height = _sg_miplevel_dim(height, mip_index);
                const int bytes_per_slice = _sg_surface_pitch(fmt, mip_width, mip_height, 1);
                const int expected_size = bytes_per_slice * num_slices;
                _SG_VALIDATE(expected_size == (int)data->subimage[face_index][mip_index].size, VALIDATE_IMAGEDATA_DATA_SIZE);
            }
        }
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_image_desc(const sg_image_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_IMAGEDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_IMAGEDESC_CANARY);
        _SG_VALIDATE(desc->width > 0, VALIDATE_IMAGEDESC_WIDTH);
        _SG_VALIDATE(desc->height > 0, VALIDATE_IMAGEDESC_HEIGHT);
        const sg_pixel_format fmt = desc->pixel_format;
        const sg_usage usage = desc->usage;
        const bool injected = (0 != desc->gl_textures[0]) ||
                              (0 != desc->mtl_textures[0]) ||
                              (0 != desc->d3d11_texture);
        if (_sg_is_depth_or_depth_stencil_format(fmt)) {
            _SG_VALIDATE(desc->type != SG_IMAGETYPE_3D, VALIDATE_IMAGEDESC_DEPTH_3D_IMAGE);
        }
        if (desc->render_target) {
            SOKOL_ASSERT(((int)fmt >= 0) && ((int)fmt < _SG_PIXELFORMAT_NUM));
            _SG_VALIDATE(_sg.formats[fmt].render, VALIDATE_IMAGEDESC_RT_PIXELFORMAT);
            _SG_VALIDATE(usage == SG_USAGE_IMMUTABLE, VALIDATE_IMAGEDESC_RT_IMMUTABLE);
            _SG_VALIDATE(desc->data.subimage[0][0].ptr==0, VALIDATE_IMAGEDESC_RT_NO_DATA);
            if (desc->sample_count > 1) {
                _SG_VALIDATE(_sg.formats[fmt].msaa, VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT);
                _SG_VALIDATE(desc->num_mipmaps == 1, VALIDATE_IMAGEDESC_MSAA_NUM_MIPMAPS);
                _SG_VALIDATE(desc->type != SG_IMAGETYPE_3D, VALIDATE_IMAGEDESC_MSAA_3D_IMAGE);
            }
        } else {
            _SG_VALIDATE(desc->sample_count == 1, VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT);
            const bool valid_nonrt_fmt = !_sg_is_valid_rendertarget_depth_format(fmt);
            _SG_VALIDATE(valid_nonrt_fmt, VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT);
            const bool is_compressed = _sg_is_compressed_pixel_format(desc->pixel_format);
            const bool is_immutable = (usage == SG_USAGE_IMMUTABLE);
            if (is_compressed) {
                _SG_VALIDATE(is_immutable, VALIDATE_IMAGEDESC_COMPRESSED_IMMUTABLE);
            }
            if (!injected && is_immutable) {
                // image desc must have valid data
                _sg_validate_image_data(&desc->data,
                    desc->pixel_format,
                    desc->width,
                    desc->height,
                    (desc->type == SG_IMAGETYPE_CUBE) ? 6 : 1,
                    desc->num_mipmaps,
                    desc->num_slices);
            } else {
                // image desc must not have data
                for (int face_index = 0; face_index < SG_CUBEFACE_NUM; face_index++) {
                    for (int mip_index = 0; mip_index < SG_MAX_MIPMAPS; mip_index++) {
                        const bool no_data = 0 == desc->data.subimage[face_index][mip_index].ptr;
                        const bool no_size = 0 == desc->data.subimage[face_index][mip_index].size;
                        if (injected) {
                            _SG_VALIDATE(no_data && no_size, VALIDATE_IMAGEDESC_INJECTED_NO_DATA);
                        }
                        if (!is_immutable) {
                            _SG_VALIDATE(no_data && no_size, VALIDATE_IMAGEDESC_DYNAMIC_NO_DATA);
                        }
                    }
                }
            }
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_sampler_desc(const sg_sampler_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_SAMPLERDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_SAMPLERDESC_CANARY);
        _SG_VALIDATE(desc->min_filter != SG_FILTER_NONE, VALIDATE_SAMPLERDESC_MINFILTER_NONE);
        _SG_VALIDATE(desc->mag_filter != SG_FILTER_NONE, VALIDATE_SAMPLERDESC_MAGFILTER_NONE);
        // restriction from WebGPU: when anisotropy > 1, all filters must be linear
        if (desc->max_anisotropy > 1) {
            _SG_VALIDATE((desc->min_filter == SG_FILTER_LINEAR)
                      && (desc->mag_filter == SG_FILTER_LINEAR)
                      && (desc->mipmap_filter == SG_FILTER_LINEAR),
                      VALIDATE_SAMPLERDESC_ANISTROPIC_REQUIRES_LINEAR_FILTERING);
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_shader_desc(const sg_shader_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_SHADERDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_SHADERDESC_CANARY);
        #if defined(SOKOL_GLES3)
            // on GL or WebGPU, must provide shader source code
            _SG_VALIDATE(0 != desc->vs.source, VALIDATE_SHADERDESC_SOURCE);
            _SG_VALIDATE(0 != desc->fs.source, VALIDATE_SHADERDESC_SOURCE);
        #elif defined(SOKOL_METAL) || defined(SOKOL_D3D11)
            // on Metal or D3D11, must provide shader source code or byte code
            _SG_VALIDATE((0 != desc->vs.source)||(0 != desc->vs.bytecode.ptr), VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE);
            _SG_VALIDATE((0 != desc->fs.source)||(0 != desc->fs.bytecode.ptr), VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE);
        #else
            // Dummy Backend, don't require source or bytecode
        #endif
        for (int i = 0; i < SG_MAX_VERTEX_ATTRIBUTES; i++) {
            if (desc->attrs[i].name) {
                _SG_VALIDATE(strlen(desc->attrs[i].name) < _SG_STRING_SIZE, VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
            }
            if (desc->attrs[i].sem_name) {
                _SG_VALIDATE(strlen(desc->attrs[i].sem_name) < _SG_STRING_SIZE, VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG);
            }
        }
        // if shader byte code, the size must also be provided
        if (0 != desc->vs.bytecode.ptr) {
            _SG_VALIDATE(desc->vs.bytecode.size > 0, VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
        }
        if (0 != desc->fs.bytecode.ptr) {
            _SG_VALIDATE(desc->fs.bytecode.size > 0, VALIDATE_SHADERDESC_NO_BYTECODE_SIZE);
        }
        for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
            const sg_shader_stage_desc* stage_desc = (stage_index == 0)? &desc->vs : &desc->fs;
            bool uniform_blocks_continuous = true;
            for (int ub_index = 0; ub_index < SG_MAX_SHADERSTAGE_UBS; ub_index++) {
                const sg_shader_uniform_block_desc* ub_desc = &stage_desc->uniform_blocks[ub_index];
                if (ub_desc->size > 0) {
                    _SG_VALIDATE(uniform_blocks_continuous, VALIDATE_SHADERDESC_NO_CONT_UBS);
                    #if defined(_SOKOL_ANY_GL)
                    bool uniforms_continuous = true;
                    uint32_t uniform_offset = 0;
                    int num_uniforms = 0;
                    for (int u_index = 0; u_index < SG_MAX_UB_MEMBERS; u_index++) {
                        const sg_shader_uniform_desc* u_desc = &ub_desc->uniforms[u_index];
                        if (u_desc->type != SG_UNIFORMTYPE_INVALID) {
                            _SG_VALIDATE(uniforms_continuous, VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS);
                            #if defined(SOKOL_GLES3)
                            _SG_VALIDATE(0 != u_desc->name, VALIDATE_SHADERDESC_UB_MEMBER_NAME);
                            #endif
                            const int array_count = u_desc->array_count;
                            _SG_VALIDATE(array_count > 0, VALIDATE_SHADERDESC_UB_ARRAY_COUNT);
                            const uint32_t u_align = _sg_uniform_alignment(u_desc->type, array_count, ub_desc->layout);
                            const uint32_t u_size  = _sg_uniform_size(u_desc->type, array_count, ub_desc->layout);
                            uniform_offset = _sg_align_u32(uniform_offset, u_align);
                            uniform_offset += u_size;
                            num_uniforms++;
                            // with std140, arrays are only allowed for FLOAT4, INT4, MAT4
                            if (ub_desc->layout == SG_UNIFORMLAYOUT_STD140) {
                                if (array_count > 1) {
                                    _SG_VALIDATE((u_desc->type == SG_UNIFORMTYPE_FLOAT4) || (u_desc->type == SG_UNIFORMTYPE_INT4) || (u_desc->type == SG_UNIFORMTYPE_MAT4), VALIDATE_SHADERDESC_UB_STD140_ARRAY_TYPE);
                                }
                            }
                        } else {
                            uniforms_continuous = false;
                        }
                    }
                    if (ub_desc->layout == SG_UNIFORMLAYOUT_STD140) {
                        uniform_offset = _sg_align_u32(uniform_offset, 16);
                    }
                    _SG_VALIDATE((size_t)uniform_offset == ub_desc->size, VALIDATE_SHADERDESC_UB_SIZE_MISMATCH);
                    _SG_VALIDATE(num_uniforms > 0, VALIDATE_SHADERDESC_NO_UB_MEMBERS);
                    #endif
                } else {
                    uniform_blocks_continuous = false;
                }
            }
            bool storage_buffers_continuous = true;
            for (int sbuf_index = 0; sbuf_index < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; sbuf_index++) {
                const sg_shader_storage_buffer_desc* sbuf_desc = &stage_desc->storage_buffers[sbuf_index];
                if (sbuf_desc->used) {
                    _SG_VALIDATE(storage_buffers_continuous, VALIDATE_SHADERDESC_NO_CONT_STORAGEBUFFERS);
                    _SG_VALIDATE(sbuf_desc->readonly, VALIDATE_SHADERDESC_STORAGEBUFFER_READONLY);
                } else {
                    storage_buffers_continuous = false;
                }
            }
            bool images_continuous = true;
            int num_images = 0;
            for (int img_index = 0; img_index < SG_MAX_SHADERSTAGE_IMAGES; img_index++) {
                const sg_shader_image_desc* img_desc = &stage_desc->images[img_index];
                if (img_desc->used) {
                    _SG_VALIDATE(images_continuous, VALIDATE_SHADERDESC_NO_CONT_IMAGES);
                    num_images++;
                } else {
                    images_continuous = false;
                }
            }
            bool samplers_continuous = true;
            int num_samplers = 0;
            for (int smp_index = 0; smp_index < SG_MAX_SHADERSTAGE_SAMPLERS; smp_index++) {
                const sg_shader_sampler_desc* smp_desc = &stage_desc->samplers[smp_index];
                if (smp_desc->used) {
                    _SG_VALIDATE(samplers_continuous, VALIDATE_SHADERDESC_NO_CONT_SAMPLERS);
                    num_samplers++;
                } else {
                    samplers_continuous = false;
                }
            }
            bool image_samplers_continuous = true;
            int num_image_samplers = 0;
            for (int img_smp_index = 0; img_smp_index < SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS; img_smp_index++) {
                const sg_shader_image_sampler_pair_desc* img_smp_desc = &stage_desc->image_sampler_pairs[img_smp_index];
                if (img_smp_desc->used) {
                    _SG_VALIDATE(image_samplers_continuous, VALIDATE_SHADERDESC_NO_CONT_IMAGE_SAMPLER_PAIRS);
                    num_image_samplers++;
                    const bool img_slot_in_range = (img_smp_desc->image_slot >= 0) && (img_smp_desc->image_slot < SG_MAX_SHADERSTAGE_IMAGES);
                    const bool smp_slot_in_range = (img_smp_desc->sampler_slot >= 0) && (img_smp_desc->sampler_slot < SG_MAX_SHADERSTAGE_SAMPLERS);
                    _SG_VALIDATE(img_slot_in_range && (img_smp_desc->image_slot < num_images), VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_IMAGE_SLOT_OUT_OF_RANGE);
                    _SG_VALIDATE(smp_slot_in_range && (img_smp_desc->sampler_slot < num_samplers), VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_IMAGE_SLOT_OUT_OF_RANGE);
                    #if defined(_SOKOL_ANY_GL)
                    _SG_VALIDATE(img_smp_desc->glsl_name != 0, VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_NAME_REQUIRED_FOR_GL);
                    #endif
                    if (img_slot_in_range && smp_slot_in_range) {
                        const sg_shader_image_desc* img_desc = &stage_desc->images[img_smp_desc->image_slot];
                        const sg_shader_sampler_desc* smp_desc = &stage_desc->samplers[img_smp_desc->sampler_slot];
                        const bool needs_nonfiltering = (img_desc->sample_type == SG_IMAGESAMPLETYPE_UINT)
                                                     || (img_desc->sample_type == SG_IMAGESAMPLETYPE_SINT)
                                                     || (img_desc->sample_type == SG_IMAGESAMPLETYPE_UNFILTERABLE_FLOAT);
                        const bool needs_comparison = img_desc->sample_type == SG_IMAGESAMPLETYPE_DEPTH;
                        if (needs_nonfiltering) {
                            _SG_VALIDATE(needs_nonfiltering && (smp_desc->sampler_type == SG_SAMPLERTYPE_NONFILTERING), VALIDATE_SHADERDESC_NONFILTERING_SAMPLER_REQUIRED);
                        }
                        if (needs_comparison) {
                            _SG_VALIDATE(needs_comparison && (smp_desc->sampler_type == SG_SAMPLERTYPE_COMPARISON), VALIDATE_SHADERDESC_COMPARISON_SAMPLER_REQUIRED);
                        }
                    }
                } else {
                    _SG_VALIDATE(img_smp_desc->glsl_name == 0, VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_NAME_BUT_NOT_USED);
                    _SG_VALIDATE(img_smp_desc->image_slot == 0, VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_IMAGE_BUT_NOT_USED);
                    _SG_VALIDATE(img_smp_desc->sampler_slot == 0, VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_SAMPLER_BUT_NOT_USED);
                    image_samplers_continuous = false;
                }
            }
            // each image and sampler must be referenced by an image sampler
            const uint32_t expected_img_slot_mask = (uint32_t)((1 << num_images) - 1);
            const uint32_t expected_smp_slot_mask = (uint32_t)((1 << num_samplers) - 1);
            uint32_t actual_img_slot_mask = 0;
            uint32_t actual_smp_slot_mask = 0;
            for (int img_smp_index = 0; img_smp_index < num_image_samplers; img_smp_index++) {
                const sg_shader_image_sampler_pair_desc* img_smp_desc = &stage_desc->image_sampler_pairs[img_smp_index];
                actual_img_slot_mask |= (1 << ((uint32_t)img_smp_desc->image_slot & 31));
                actual_smp_slot_mask |= (1 << ((uint32_t)img_smp_desc->sampler_slot & 31));
            }
            _SG_VALIDATE(expected_img_slot_mask == actual_img_slot_mask, VALIDATE_SHADERDESC_IMAGE_NOT_REFERENCED_BY_IMAGE_SAMPLER_PAIRS);
            _SG_VALIDATE(expected_smp_slot_mask == actual_smp_slot_mask, VALIDATE_SHADERDESC_SAMPLER_NOT_REFERENCED_BY_IMAGE_SAMPLER_PAIRS);
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_pipeline_desc(const sg_pipeline_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_PIPELINEDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_PIPELINEDESC_CANARY);
        _SG_VALIDATE(desc->shader.id != SG_INVALID_ID, VALIDATE_PIPELINEDESC_SHADER);
        for (int buf_index = 0; buf_index < SG_MAX_VERTEX_BUFFERS; buf_index++) {
            const sg_vertex_buffer_layout_state* l_state = &desc->layout.buffers[buf_index];
            if (l_state->stride == 0) {
                continue;
            }
            _SG_VALIDATE(_sg_multiple_u64((uint64_t)l_state->stride, 4), VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4);
        }
        const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, desc->shader.id);
        _SG_VALIDATE(0 != shd, VALIDATE_PIPELINEDESC_SHADER);
        if (shd) {
            _SG_VALIDATE(shd->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_PIPELINEDESC_SHADER);
            bool attrs_cont = true;
            for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
                const sg_vertex_attr_state* a_state = &desc->layout.attrs[attr_index];
                if (a_state->format == SG_VERTEXFORMAT_INVALID) {
                    attrs_cont = false;
                    continue;
                }
                _SG_VALIDATE(attrs_cont, VALIDATE_PIPELINEDESC_NO_CONT_ATTRS);
                SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
                #if defined(SOKOL_D3D11)
                // on D3D11, semantic names (and semantic indices) must be provided
                _SG_VALIDATE(!_sg_strempty(&shd->d3d11.attrs[attr_index].sem_name), VALIDATE_PIPELINEDESC_ATTR_SEMANTICS);
                #endif
            }
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_attachments_desc(const sg_attachments_desc* desc) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(desc);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(desc);
        _sg_validate_begin();
        _SG_VALIDATE(desc->_start_canary == 0, VALIDATE_ATTACHMENTSDESC_CANARY);
        _SG_VALIDATE(desc->_end_canary == 0, VALIDATE_ATTACHMENTSDESC_CANARY);
        bool atts_cont = true;
        int color_width = -1, color_height = -1, color_sample_count = -1;
        bool has_color_atts = false;
        for (int att_index = 0; att_index < SG_MAX_COLOR_ATTACHMENTS; att_index++) {
            const sg_attachment_desc* att = &desc->colors[att_index];
            if (att->image.id == SG_INVALID_ID) {
                atts_cont = false;
                continue;
            }
            _SG_VALIDATE(atts_cont, VALIDATE_ATTACHMENTSDESC_NO_CONT_COLOR_ATTS);
            has_color_atts = true;
            const _sg_image_t* img = _sg_lookup_image(&_sg.pools, att->image.id);
            _SG_VALIDATE(img, VALIDATE_ATTACHMENTSDESC_IMAGE);
            if (0 != img) {
                _SG_VALIDATE(img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_ATTACHMENTSDESC_IMAGE);
                _SG_VALIDATE(img->cmn.render_target, VALIDATE_ATTACHMENTSDESC_IMAGE_NO_RT);
                _SG_VALIDATE(att->mip_level < img->cmn.num_mipmaps, VALIDATE_ATTACHMENTSDESC_MIPLEVEL);
                if (img->cmn.type == SG_IMAGETYPE_CUBE) {
                    _SG_VALIDATE(att->slice < 6, VALIDATE_ATTACHMENTSDESC_FACE);
                } else if (img->cmn.type == SG_IMAGETYPE_ARRAY) {
                    _SG_VALIDATE(att->slice < img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_LAYER);
                } else if (img->cmn.type == SG_IMAGETYPE_3D) {
                    _SG_VALIDATE(att->slice < img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_SLICE);
                }
                if (att_index == 0) {
                    color_width = _sg_miplevel_dim(img->cmn.width, att->mip_level);
                    color_height = _sg_miplevel_dim(img->cmn.height, att->mip_level);
                    color_sample_count = img->cmn.sample_count;
                } else {
                    _SG_VALIDATE(color_width == _sg_miplevel_dim(img->cmn.width, att->mip_level), VALIDATE_ATTACHMENTSDESC_IMAGE_SIZES);
                    _SG_VALIDATE(color_height == _sg_miplevel_dim(img->cmn.height, att->mip_level), VALIDATE_ATTACHMENTSDESC_IMAGE_SIZES);
                    _SG_VALIDATE(color_sample_count == img->cmn.sample_count, VALIDATE_ATTACHMENTSDESC_IMAGE_SAMPLE_COUNTS);
                }
                _SG_VALIDATE(_sg_is_valid_rendertarget_color_format(img->cmn.pixel_format), VALIDATE_ATTACHMENTSDESC_COLOR_INV_PIXELFORMAT);

                // check resolve attachment
                const sg_attachment_desc* res_att = &desc->resolves[att_index];
                if (res_att->image.id != SG_INVALID_ID) {
                    // associated color attachment must be MSAA
                    _SG_VALIDATE(img->cmn.sample_count > 1, VALIDATE_ATTACHMENTSDESC_RESOLVE_COLOR_IMAGE_MSAA);
                    const _sg_image_t* res_img = _sg_lookup_image(&_sg.pools, res_att->image.id);
                    _SG_VALIDATE(res_img, VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE);
                    if (res_img != 0) {
                        _SG_VALIDATE(res_img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE);
                        _SG_VALIDATE(res_img->cmn.render_target, VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_NO_RT);
                        _SG_VALIDATE(res_img->cmn.sample_count == 1, VALIDATE_ATTACHMENTSDESC_RESOLVE_SAMPLE_COUNT);
                        _SG_VALIDATE(res_att->mip_level < res_img->cmn.num_mipmaps, VALIDATE_ATTACHMENTSDESC_RESOLVE_MIPLEVEL);
                        if (res_img->cmn.type == SG_IMAGETYPE_CUBE) {
                            _SG_VALIDATE(res_att->slice < 6, VALIDATE_ATTACHMENTSDESC_RESOLVE_FACE);
                        } else if (res_img->cmn.type == SG_IMAGETYPE_ARRAY) {
                            _SG_VALIDATE(res_att->slice < res_img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_RESOLVE_LAYER);
                        } else if (res_img->cmn.type == SG_IMAGETYPE_3D) {
                            _SG_VALIDATE(res_att->slice < res_img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_RESOLVE_SLICE);
                        }
                        _SG_VALIDATE(img->cmn.pixel_format == res_img->cmn.pixel_format, VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_FORMAT);
                        _SG_VALIDATE(color_width == _sg_miplevel_dim(res_img->cmn.width, res_att->mip_level), VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_SIZES);
                        _SG_VALIDATE(color_height == _sg_miplevel_dim(res_img->cmn.height, res_att->mip_level), VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_SIZES);
                    }
                }
            }
        }
        bool has_depth_stencil_att = false;
        if (desc->depth_stencil.image.id != SG_INVALID_ID) {
            const sg_attachment_desc* att = &desc->depth_stencil;
            const _sg_image_t* img = _sg_lookup_image(&_sg.pools, att->image.id);
            _SG_VALIDATE(img, VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE);
            has_depth_stencil_att = true;
            if (img) {
                _SG_VALIDATE(img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE);
                _SG_VALIDATE(att->mip_level < img->cmn.num_mipmaps, VALIDATE_ATTACHMENTSDESC_DEPTH_MIPLEVEL);
                if (img->cmn.type == SG_IMAGETYPE_CUBE) {
                    _SG_VALIDATE(att->slice < 6, VALIDATE_ATTACHMENTSDESC_DEPTH_FACE);
                } else if (img->cmn.type == SG_IMAGETYPE_ARRAY) {
                    _SG_VALIDATE(att->slice < img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_DEPTH_LAYER);
                } else if (img->cmn.type == SG_IMAGETYPE_3D) {
                    // NOTE: this can't actually happen because of VALIDATE_IMAGEDESC_DEPTH_3D_IMAGE
                    _SG_VALIDATE(att->slice < img->cmn.num_slices, VALIDATE_ATTACHMENTSDESC_DEPTH_SLICE);
                }
                _SG_VALIDATE(img->cmn.render_target, VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_NO_RT);
                _SG_VALIDATE((color_width == -1) || (color_width == _sg_miplevel_dim(img->cmn.width, att->mip_level)), VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_SIZES);
                _SG_VALIDATE((color_height == -1) || (color_height == _sg_miplevel_dim(img->cmn.height, att->mip_level)), VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_SIZES);
                _SG_VALIDATE((color_sample_count == -1) || (color_sample_count == img->cmn.sample_count), VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_SAMPLE_COUNT);
                _SG_VALIDATE(_sg_is_valid_rendertarget_depth_format(img->cmn.pixel_format), VALIDATE_ATTACHMENTSDESC_DEPTH_INV_PIXELFORMAT);
            }
        }
        _SG_VALIDATE(has_color_atts || has_depth_stencil_att, VALIDATE_ATTACHMENTSDESC_NO_ATTACHMENTS);
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_begin_pass(const sg_pass* pass) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(pass);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        _sg_validate_begin();
        _SG_VALIDATE(pass->_start_canary == 0, VALIDATE_BEGINPASS_CANARY);
        _SG_VALIDATE(pass->_end_canary == 0, VALIDATE_BEGINPASS_CANARY);
        if (pass->attachments.id == SG_INVALID_ID) {
            // this is a swapchain pass
            _SG_VALIDATE(pass->swapchain.width > 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_WIDTH);
            _SG_VALIDATE(pass->swapchain.height > 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_HEIGHT);
            _SG_VALIDATE(pass->swapchain.sample_count > 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_SAMPLECOUNT);
            _SG_VALIDATE(pass->swapchain.color_format > SG_PIXELFORMAT_NONE, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_COLORFORMAT);
            // NOTE: depth buffer is optional, so depth_format is allowed to be invalid
            // NOTE: the GL framebuffer handle may actually be 0
            #if defined(SOKOL_METAL)
                _SG_VALIDATE(pass->swapchain.metal.current_drawable != 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_CURRENTDRAWABLE);
                if (pass->swapchain.depth_format == SG_PIXELFORMAT_NONE) {
                    _SG_VALIDATE(pass->swapchain.metal.depth_stencil_texture == 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_DEPTHSTENCILTEXTURE_NOTSET);
                } else {
                    _SG_VALIDATE(pass->swapchain.metal.depth_stencil_texture != 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_DEPTHSTENCILTEXTURE);
                }
                if (pass->swapchain.sample_count > 1) {
                    _SG_VALIDATE(pass->swapchain.metal.msaa_color_texture != 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_MSAACOLORTEXTURE);
                } else {
                    _SG_VALIDATE(pass->swapchain.metal.msaa_color_texture == 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_MSAACOLORTEXTURE_NOTSET);
                }
            #elif defined(SOKOL_D3D11)
                _SG_VALIDATE(pass->swapchain.d3d11.render_view != 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RENDERVIEW);
                if (pass->swapchain.depth_format == SG_PIXELFORMAT_NONE) {
                    _SG_VALIDATE(pass->swapchain.d3d11.depth_stencil_view == 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_DEPTHSTENCILVIEW_NOTSET);
                } else {
                    _SG_VALIDATE(pass->swapchain.d3d11.depth_stencil_view != 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_DEPTHSTENCILVIEW);
                }
                if (pass->swapchain.sample_count > 1) {
                    _SG_VALIDATE(pass->swapchain.d3d11.resolve_view != 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RESOLVEVIEW);
                } else {
                    _SG_VALIDATE(pass->swapchain.d3d11.resolve_view == 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RESOLVEVIEW_NOTSET);
                }
            #endif
        } else {
            // this is an 'offscreen pass'
            const _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, pass->attachments.id);
            if (atts) {
                _SG_VALIDATE(atts->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_BEGINPASS_ATTACHMENTS_VALID);
                for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
                    const _sg_attachment_common_t* color_att = &atts->cmn.colors[i];
                    const _sg_image_t* color_img = _sg_attachments_color_image(atts, i);
                    if (color_img) {
                        _SG_VALIDATE(color_img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_BEGINPASS_COLOR_ATTACHMENT_IMAGE);
                        _SG_VALIDATE(color_img->slot.id == color_att->image_id.id, VALIDATE_BEGINPASS_COLOR_ATTACHMENT_IMAGE);
                    }
                    const _sg_attachment_common_t* resolve_att = &atts->cmn.resolves[i];
                    const _sg_image_t* resolve_img = _sg_attachments_resolve_image(atts, i);
                    if (resolve_img) {
                        _SG_VALIDATE(resolve_img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_BEGINPASS_RESOLVE_ATTACHMENT_IMAGE);
                        _SG_VALIDATE(resolve_img->slot.id == resolve_att->image_id.id, VALIDATE_BEGINPASS_RESOLVE_ATTACHMENT_IMAGE);
                    }
                }
                const _sg_image_t* ds_img = _sg_attachments_ds_image(atts);
                if (ds_img) {
                    const _sg_attachment_common_t* att = &atts->cmn.depth_stencil;
                    _SG_VALIDATE(ds_img->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_BEGINPASS_DEPTHSTENCIL_ATTACHMENT_IMAGE);
                    _SG_VALIDATE(ds_img->slot.id == att->image_id.id, VALIDATE_BEGINPASS_DEPTHSTENCIL_ATTACHMENT_IMAGE);
                }
            } else {
                _SG_VALIDATE(atts != 0, VALIDATE_BEGINPASS_ATTACHMENTS_EXISTS);
            }
            // swapchain params must be all zero!
            _SG_VALIDATE(pass->swapchain.width == 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_WIDTH_NOTSET);
            _SG_VALIDATE(pass->swapchain.height == 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_HEIGHT_NOTSET);
            _SG_VALIDATE(pass->swapchain.sample_count == 0, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_SAMPLECOUNT_NOTSET);
            _SG_VALIDATE(pass->swapchain.color_format == _SG_PIXELFORMAT_DEFAULT, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_COLORFORMAT_NOTSET);
            _SG_VALIDATE(pass->swapchain.depth_format == _SG_PIXELFORMAT_DEFAULT, VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_DEPTHFORMAT_NOTSET);
            #if defined(SOKOL_METAL)
                _SG_VALIDATE(pass->swapchain.metal.current_drawable == 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_CURRENTDRAWABLE_NOTSET);
                _SG_VALIDATE(pass->swapchain.metal.depth_stencil_texture == 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_DEPTHSTENCILTEXTURE_NOTSET);
                _SG_VALIDATE(pass->swapchain.metal.msaa_color_texture == 0, VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_MSAACOLORTEXTURE_NOTSET);
            #elif defined(SOKOL_D3D11)
                _SG_VALIDATE(pass->swapchain.d3d11.render_view == 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RENDERVIEW_NOTSET);
                _SG_VALIDATE(pass->swapchain.d3d11.depth_stencil_view == 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_DEPTHSTENCILVIEW_NOTSET);
                _SG_VALIDATE(pass->swapchain.d3d11.resolve_view == 0, VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RESOLVEVIEW_NOTSET);
            #elif defined(_SOKOL_ANY_GL)
                _SG_VALIDATE(pass->swapchain.gl.framebuffer == 0, VALIDATE_BEGINPASS_SWAPCHAIN_GL_EXPECT_FRAMEBUFFER_NOTSET);
            #endif
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_apply_pipeline(sg_pipeline pip_id) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(pip_id);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        _sg_validate_begin();
        // the pipeline object must be alive and valid
        _SG_VALIDATE(pip_id.id != SG_INVALID_ID, VALIDATE_APIP_PIPELINE_VALID_ID);
        const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
        _SG_VALIDATE(pip != 0, VALIDATE_APIP_PIPELINE_EXISTS);
        if (!pip) {
            return _sg_validate_end();
        }
        _SG_VALIDATE(pip->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_APIP_PIPELINE_VALID);
        // the pipeline's shader must be alive and valid
        SOKOL_ASSERT(pip->shader);
        _SG_VALIDATE(pip->shader->slot.id == pip->cmn.shader_id.id, VALIDATE_APIP_SHADER_EXISTS);
        _SG_VALIDATE(pip->shader->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_APIP_SHADER_VALID);
        // check that pipeline attributes match current pass attributes
        if (_sg.cur_pass.atts_id.id != SG_INVALID_ID) {
            // an offscreen pass
            const _sg_attachments_t* atts = _sg.cur_pass.atts;
            SOKOL_ASSERT(atts);
            _SG_VALIDATE(atts->slot.id == _sg.cur_pass.atts_id.id, VALIDATE_APIP_CURPASS_ATTACHMENTS_EXISTS);
            _SG_VALIDATE(atts->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_APIP_CURPASS_ATTACHMENTS_VALID);

            _SG_VALIDATE(pip->cmn.color_count == atts->cmn.num_colors, VALIDATE_APIP_ATT_COUNT);
            for (int i = 0; i < pip->cmn.color_count; i++) {
                const _sg_image_t* att_img = _sg_attachments_color_image(atts, i);
                _SG_VALIDATE(pip->cmn.colors[i].pixel_format == att_img->cmn.pixel_format, VALIDATE_APIP_COLOR_FORMAT);
                _SG_VALIDATE(pip->cmn.sample_count == att_img->cmn.sample_count, VALIDATE_APIP_SAMPLE_COUNT);
            }
            const _sg_image_t* att_dsimg = _sg_attachments_ds_image(atts);
            if (att_dsimg) {
                _SG_VALIDATE(pip->cmn.depth.pixel_format == att_dsimg->cmn.pixel_format, VALIDATE_APIP_DEPTH_FORMAT);
            } else {
                _SG_VALIDATE(pip->cmn.depth.pixel_format == SG_PIXELFORMAT_NONE, VALIDATE_APIP_DEPTH_FORMAT);
            }
        } else {
            // default pass
            _SG_VALIDATE(pip->cmn.color_count == 1, VALIDATE_APIP_ATT_COUNT);
            _SG_VALIDATE(pip->cmn.colors[0].pixel_format == _sg.cur_pass.swapchain.color_fmt, VALIDATE_APIP_COLOR_FORMAT);
            _SG_VALIDATE(pip->cmn.depth.pixel_format == _sg.cur_pass.swapchain.depth_fmt, VALIDATE_APIP_DEPTH_FORMAT);
            _SG_VALIDATE(pip->cmn.sample_count == _sg.cur_pass.swapchain.sample_count, VALIDATE_APIP_SAMPLE_COUNT);
        }
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_apply_bindings(const sg_bindings* bindings) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(bindings);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        _sg_validate_begin();

        // a pipeline object must have been applied
        _SG_VALIDATE(_sg.cur_pipeline.id != SG_INVALID_ID, VALIDATE_ABND_PIPELINE);
        const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, _sg.cur_pipeline.id);
        _SG_VALIDATE(pip != 0, VALIDATE_ABND_PIPELINE_EXISTS);
        if (!pip) {
            return _sg_validate_end();
        }
        _SG_VALIDATE(pip->slot.state == SG_RESOURCESTATE_VALID, VALIDATE_ABND_PIPELINE_VALID);
        SOKOL_ASSERT(pip->shader && (pip->cmn.shader_id.id == pip->shader->slot.id));

        // has expected vertex buffers, and vertex buffers still exist
        for (int i = 0; i < SG_MAX_VERTEX_BUFFERS; i++) {
            if (bindings->vertex_buffers[i].id != SG_INVALID_ID) {
                _SG_VALIDATE(pip->cmn.vertex_buffer_layout_active[i], VALIDATE_ABND_VBS);
                // buffers in vertex-buffer-slots must be of type SG_BUFFERTYPE_VERTEXBUFFER
                const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, bindings->vertex_buffers[i].id);
                _SG_VALIDATE(buf != 0, VALIDATE_ABND_VB_EXISTS);
                if (buf && buf->slot.state == SG_RESOURCESTATE_VALID) {
                    _SG_VALIDATE(SG_BUFFERTYPE_VERTEXBUFFER == buf->cmn.type, VALIDATE_ABND_VB_TYPE);
                    _SG_VALIDATE(!buf->cmn.append_overflow, VALIDATE_ABND_VB_OVERFLOW);
                }
            } else {
                // vertex buffer provided in a slot which has no vertex layout in pipeline
                _SG_VALIDATE(!pip->cmn.vertex_buffer_layout_active[i], VALIDATE_ABND_VBS);
            }
        }

        // index buffer expected or not, and index buffer still exists
        if (pip->cmn.index_type == SG_INDEXTYPE_NONE) {
            // pipeline defines non-indexed rendering, but index buffer provided
            _SG_VALIDATE(bindings->index_buffer.id == SG_INVALID_ID, VALIDATE_ABND_IB);
        } else {
            // pipeline defines indexed rendering, but no index buffer provided
            _SG_VALIDATE(bindings->index_buffer.id != SG_INVALID_ID, VALIDATE_ABND_NO_IB);
        }
        if (bindings->index_buffer.id != SG_INVALID_ID) {
            // buffer in index-buffer-slot must be of type SG_BUFFERTYPE_INDEXBUFFER
            const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, bindings->index_buffer.id);
            _SG_VALIDATE(buf != 0, VALIDATE_ABND_IB_EXISTS);
            if (buf && buf->slot.state == SG_RESOURCESTATE_VALID) {
                _SG_VALIDATE(SG_BUFFERTYPE_INDEXBUFFER == buf->cmn.type, VALIDATE_ABND_IB_TYPE);
                _SG_VALIDATE(!buf->cmn.append_overflow, VALIDATE_ABND_IB_OVERFLOW);
            }
        }

        // has expected vertex shader images
        for (int i = 0; i < SG_MAX_SHADERSTAGE_IMAGES; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_VS];
            if (stage->images[i].image_type != _SG_IMAGETYPE_DEFAULT) {
                _SG_VALIDATE(bindings->vs.images[i].id != SG_INVALID_ID, VALIDATE_ABND_VS_EXPECTED_IMAGE_BINDING);
                if (bindings->vs.images[i].id != SG_INVALID_ID) {
                    const _sg_image_t* img = _sg_lookup_image(&_sg.pools, bindings->vs.images[i].id);
                    _SG_VALIDATE(img != 0, VALIDATE_ABND_VS_IMG_EXISTS);
                    if (img && img->slot.state == SG_RESOURCESTATE_VALID) {
                        _SG_VALIDATE(img->cmn.type == stage->images[i].image_type, VALIDATE_ABND_VS_IMAGE_TYPE_MISMATCH);
                        _SG_VALIDATE(img->cmn.sample_count == 1, VALIDATE_ABND_VS_IMAGE_MSAA);
                        const _sg_pixelformat_info_t* info = &_sg.formats[img->cmn.pixel_format];
                        switch (stage->images[i].sample_type) {
                            case SG_IMAGESAMPLETYPE_FLOAT:
                                _SG_VALIDATE(info->filter, VALIDATE_ABND_VS_EXPECTED_FILTERABLE_IMAGE);
                                break;
                            case SG_IMAGESAMPLETYPE_DEPTH:
                                _SG_VALIDATE(info->depth, VALIDATE_ABND_VS_EXPECTED_DEPTH_IMAGE);
                                break;
                            default:
                                break;
                        }
                    }
                }
            } else {
                _SG_VALIDATE(bindings->vs.images[i].id == SG_INVALID_ID, VALIDATE_ABND_VS_UNEXPECTED_IMAGE_BINDING);
            }
        }

        // has expected vertex shader image samplers
        for (int i = 0; i < SG_MAX_SHADERSTAGE_SAMPLERS; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_VS];
            if (stage->samplers[i].sampler_type != _SG_SAMPLERTYPE_DEFAULT) {
                _SG_VALIDATE(bindings->vs.samplers[i].id != SG_INVALID_ID, VALIDATE_ABND_VS_EXPECTED_SAMPLER_BINDING);
                if (bindings->vs.samplers[i].id != SG_INVALID_ID) {
                    const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, bindings->vs.samplers[i].id);
                    _SG_VALIDATE(smp != 0, VALIDATE_ABND_VS_SMP_EXISTS);
                    if (smp) {
                        if (stage->samplers[i].sampler_type == SG_SAMPLERTYPE_COMPARISON) {
                            _SG_VALIDATE(smp->cmn.compare != SG_COMPAREFUNC_NEVER, VALIDATE_ABND_VS_UNEXPECTED_SAMPLER_COMPARE_NEVER);
                        } else {
                            _SG_VALIDATE(smp->cmn.compare == SG_COMPAREFUNC_NEVER, VALIDATE_ABND_VS_EXPECTED_SAMPLER_COMPARE_NEVER);
                        }
                        if (stage->samplers[i].sampler_type == SG_SAMPLERTYPE_NONFILTERING) {
                            const bool nonfiltering = (smp->cmn.min_filter != SG_FILTER_LINEAR)
                                                   && (smp->cmn.mag_filter != SG_FILTER_LINEAR)
                                                   && (smp->cmn.mipmap_filter != SG_FILTER_LINEAR);
                            _SG_VALIDATE(nonfiltering, VALIDATE_ABND_VS_EXPECTED_NONFILTERING_SAMPLER);
                        }
                    }
                }
            } else {
                _SG_VALIDATE(bindings->vs.samplers[i].id == SG_INVALID_ID, VALIDATE_ABND_VS_UNEXPECTED_SAMPLER_BINDING);
            }
        }

        // has expected vertex shader storage buffers
        for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_VS];
            if (stage->storage_buffers[i].used) {
                _SG_VALIDATE(bindings->vs.storage_buffers[i].id != SG_INVALID_ID, VALIDATE_ABND_VS_EXPECTED_STORAGEBUFFER_BINDING);
                if (bindings->vs.storage_buffers[i].id != SG_INVALID_ID) {
                    const _sg_buffer_t* sbuf = _sg_lookup_buffer(&_sg.pools, bindings->vs.storage_buffers[i].id);
                    _SG_VALIDATE(sbuf != 0, VALIDATE_ABND_VS_STORAGEBUFFER_EXISTS);
                    if (sbuf) {
                        _SG_VALIDATE(sbuf->cmn.type == SG_BUFFERTYPE_STORAGEBUFFER, VALIDATE_ABND_VS_STORAGEBUFFER_BINDING_BUFFERTYPE);
                    }
                }
            } else {
                _SG_VALIDATE(bindings->vs.storage_buffers[i].id == SG_INVALID_ID, VALIDATE_ABND_VS_UNEXPECTED_STORAGEBUFFER_BINDING);
            }
        }

        // has expected fragment shader images
        for (int i = 0; i < SG_MAX_SHADERSTAGE_IMAGES; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_FS];
            if (stage->images[i].image_type != _SG_IMAGETYPE_DEFAULT) {
                _SG_VALIDATE(bindings->fs.images[i].id != SG_INVALID_ID, VALIDATE_ABND_FS_EXPECTED_IMAGE_BINDING);
                if (bindings->fs.images[i].id != SG_INVALID_ID) {
                    const _sg_image_t* img = _sg_lookup_image(&_sg.pools, bindings->fs.images[i].id);
                    _SG_VALIDATE(img != 0, VALIDATE_ABND_FS_IMG_EXISTS);
                    if (img && img->slot.state == SG_RESOURCESTATE_VALID) {
                        _SG_VALIDATE(img->cmn.type == stage->images[i].image_type, VALIDATE_ABND_FS_IMAGE_TYPE_MISMATCH);
                        _SG_VALIDATE(img->cmn.sample_count == 1, VALIDATE_ABND_FS_IMAGE_MSAA);
                        const _sg_pixelformat_info_t* info = &_sg.formats[img->cmn.pixel_format];
                        switch (stage->images[i].sample_type) {
                            case SG_IMAGESAMPLETYPE_FLOAT:
                                _SG_VALIDATE(info->filter, VALIDATE_ABND_FS_EXPECTED_FILTERABLE_IMAGE);
                                break;
                            case SG_IMAGESAMPLETYPE_DEPTH:
                                _SG_VALIDATE(info->depth, VALIDATE_ABND_FS_EXPECTED_DEPTH_IMAGE);
                                break;
                            default:
                                break;
                        }
                    }
                }
            } else {
                _SG_VALIDATE(bindings->fs.images[i].id == SG_INVALID_ID, VALIDATE_ABND_FS_UNEXPECTED_IMAGE_BINDING);
            }
        }

        // has expected fragment shader samplers
        for (int i = 0; i < SG_MAX_SHADERSTAGE_SAMPLERS; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_FS];
            if (stage->samplers[i].sampler_type != _SG_SAMPLERTYPE_DEFAULT) {
                _SG_VALIDATE(bindings->fs.samplers[i].id != SG_INVALID_ID, VALIDATE_ABND_FS_EXPECTED_SAMPLER_BINDING);
                if (bindings->fs.samplers[i].id != SG_INVALID_ID) {
                    const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, bindings->fs.samplers[i].id);
                    _SG_VALIDATE(smp != 0, VALIDATE_ABND_FS_SMP_EXISTS);
                    if (smp) {
                        if (stage->samplers[i].sampler_type == SG_SAMPLERTYPE_COMPARISON) {
                            _SG_VALIDATE(smp->cmn.compare != SG_COMPAREFUNC_NEVER, VALIDATE_ABND_FS_UNEXPECTED_SAMPLER_COMPARE_NEVER);
                        } else {
                            _SG_VALIDATE(smp->cmn.compare == SG_COMPAREFUNC_NEVER, VALIDATE_ABND_FS_EXPECTED_SAMPLER_COMPARE_NEVER);
                        }
                        if (stage->samplers[i].sampler_type == SG_SAMPLERTYPE_NONFILTERING) {
                            const bool nonfiltering = (smp->cmn.min_filter != SG_FILTER_LINEAR)
                                                   && (smp->cmn.mag_filter != SG_FILTER_LINEAR)
                                                   && (smp->cmn.mipmap_filter != SG_FILTER_LINEAR);
                            _SG_VALIDATE(nonfiltering, VALIDATE_ABND_FS_EXPECTED_NONFILTERING_SAMPLER);
                        }
                    }
                }
            } else {
                _SG_VALIDATE(bindings->fs.samplers[i].id == SG_INVALID_ID, VALIDATE_ABND_FS_UNEXPECTED_SAMPLER_BINDING);
            }
        }

        // has expected fragment shader storage buffers
        for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++) {
            const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[SG_SHADERSTAGE_FS];
            if (stage->storage_buffers[i].used) {
                _SG_VALIDATE(bindings->fs.storage_buffers[i].id != SG_INVALID_ID, VALIDATE_ABND_FS_EXPECTED_STORAGEBUFFER_BINDING);
                if (bindings->fs.storage_buffers[i].id != SG_INVALID_ID) {
                    const _sg_buffer_t* sbuf = _sg_lookup_buffer(&_sg.pools, bindings->fs.storage_buffers[i].id);
                    _SG_VALIDATE(sbuf != 0, VALIDATE_ABND_FS_STORAGEBUFFER_EXISTS);
                    if (sbuf) {
                        _SG_VALIDATE(sbuf->cmn.type == SG_BUFFERTYPE_STORAGEBUFFER, VALIDATE_ABND_FS_STORAGEBUFFER_BINDING_BUFFERTYPE);
                    }
                }
            } else {
                _SG_VALIDATE(bindings->fs.storage_buffers[i].id == SG_INVALID_ID, VALIDATE_ABND_FS_UNEXPECTED_STORAGEBUFFER_BINDING);
            }
        }

        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(stage_index);
        _SOKOL_UNUSED(ub_index);
        _SOKOL_UNUSED(data);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT((stage_index == SG_SHADERSTAGE_VS) || (stage_index == SG_SHADERSTAGE_FS));
        SOKOL_ASSERT((ub_index >= 0) && (ub_index < SG_MAX_SHADERSTAGE_UBS));
        _sg_validate_begin();
        _SG_VALIDATE(_sg.cur_pipeline.id != SG_INVALID_ID, VALIDATE_AUB_NO_PIPELINE);
        const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, _sg.cur_pipeline.id);
        SOKOL_ASSERT(pip && (pip->slot.id == _sg.cur_pipeline.id));
        SOKOL_ASSERT(pip->shader && (pip->shader->slot.id == pip->cmn.shader_id.id));

        // check that there is a uniform block at 'stage' and 'ub_index'
        const _sg_shader_stage_t* stage = &pip->shader->cmn.stage[stage_index];
        _SG_VALIDATE(ub_index < stage->num_uniform_blocks, VALIDATE_AUB_NO_UB_AT_SLOT);

        // check that the provided data size matches the uniform block size
        _SG_VALIDATE(data->size == stage->uniform_blocks[ub_index].size, VALIDATE_AUB_SIZE);

        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_update_buffer(const _sg_buffer_t* buf, const sg_range* data) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(buf);
        _SOKOL_UNUSED(data);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(buf && data && data->ptr);
        _sg_validate_begin();
        _SG_VALIDATE(buf->cmn.usage != SG_USAGE_IMMUTABLE, VALIDATE_UPDATEBUF_USAGE);
        _SG_VALIDATE(buf->cmn.size >= (int)data->size, VALIDATE_UPDATEBUF_SIZE);
        _SG_VALIDATE(buf->cmn.update_frame_index != _sg.frame_index, VALIDATE_UPDATEBUF_ONCE);
        _SG_VALIDATE(buf->cmn.append_frame_index != _sg.frame_index, VALIDATE_UPDATEBUF_APPEND);
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_append_buffer(const _sg_buffer_t* buf, const sg_range* data) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(buf);
        _SOKOL_UNUSED(data);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(buf && data && data->ptr);
        _sg_validate_begin();
        _SG_VALIDATE(buf->cmn.usage != SG_USAGE_IMMUTABLE, VALIDATE_APPENDBUF_USAGE);
        _SG_VALIDATE(buf->cmn.size >= (buf->cmn.append_pos + (int)data->size), VALIDATE_APPENDBUF_SIZE);
        _SG_VALIDATE(buf->cmn.update_frame_index != _sg.frame_index, VALIDATE_APPENDBUF_UPDATE);
        return _sg_validate_end();
    #endif
}

_SOKOL_PRIVATE bool _sg_validate_update_image(const _sg_image_t* img, const sg_image_data* data) {
    #if !defined(SOKOL_DEBUG)
        _SOKOL_UNUSED(img);
        _SOKOL_UNUSED(data);
        return true;
    #else
        if (_sg.desc.disable_validation) {
            return true;
        }
        SOKOL_ASSERT(img && data);
        _sg_validate_begin();
        _SG_VALIDATE(img->cmn.usage != SG_USAGE_IMMUTABLE, VALIDATE_UPDIMG_USAGE);
        _SG_VALIDATE(img->cmn.upd_frame_index != _sg.frame_index, VALIDATE_UPDIMG_ONCE);
        _sg_validate_image_data(data,
            img->cmn.pixel_format,
            img->cmn.width,
            img->cmn.height,
            (img->cmn.type == SG_IMAGETYPE_CUBE) ? 6 : 1,
            img->cmn.num_mipmaps,
            img->cmn.num_slices);
        return _sg_validate_end();
    #endif
}

// ██████  ███████ ███████  ██████  ██    ██ ██████   ██████ ███████ ███████
// ██   ██ ██      ██      ██    ██ ██    ██ ██   ██ ██      ██      ██
// ██████  █████   ███████ ██    ██ ██    ██ ██████  ██      █████   ███████
// ██   ██ ██           ██ ██    ██ ██    ██ ██   ██ ██      ██           ██
// ██   ██ ███████ ███████  ██████   ██████  ██   ██  ██████ ███████ ███████
//
// >>resources
_SOKOL_PRIVATE sg_buffer_desc _sg_buffer_desc_defaults(const sg_buffer_desc* desc) {
    sg_buffer_desc def = *desc;
    def.type = _sg_def(def.type, SG_BUFFERTYPE_VERTEXBUFFER);
    def.usage = _sg_def(def.usage, SG_USAGE_IMMUTABLE);
    if (def.size == 0) {
        def.size = def.data.size;
    } else if (def.data.size == 0) {
        def.data.size = def.size;
    }
    return def;
}

_SOKOL_PRIVATE sg_image_desc _sg_image_desc_defaults(const sg_image_desc* desc) {
    sg_image_desc def = *desc;
    def.type = _sg_def(def.type, SG_IMAGETYPE_2D);
    def.num_slices = _sg_def(def.num_slices, 1);
    def.num_mipmaps = _sg_def(def.num_mipmaps, 1);
    def.usage = _sg_def(def.usage, SG_USAGE_IMMUTABLE);
    if (desc->render_target) {
        def.pixel_format = _sg_def(def.pixel_format, _sg.desc.environment.defaults.color_format);
        def.sample_count = _sg_def(def.sample_count, _sg.desc.environment.defaults.sample_count);
    } else {
        def.pixel_format = _sg_def(def.pixel_format, SG_PIXELFORMAT_RGBA8);
        def.sample_count = _sg_def(def.sample_count, 1);
    }
    return def;
}

_SOKOL_PRIVATE sg_sampler_desc _sg_sampler_desc_defaults(const sg_sampler_desc* desc) {
    sg_sampler_desc def = *desc;
    def.min_filter = _sg_def(def.min_filter, SG_FILTER_NEAREST);
    def.mag_filter = _sg_def(def.mag_filter, SG_FILTER_NEAREST);
    def.mipmap_filter = _sg_def(def.mipmap_filter, SG_FILTER_NONE);
    def.wrap_u = _sg_def(def.wrap_u, SG_WRAP_REPEAT);
    def.wrap_v = _sg_def(def.wrap_v, SG_WRAP_REPEAT);
    def.wrap_w = _sg_def(def.wrap_w, SG_WRAP_REPEAT);
    def.max_lod = _sg_def_flt(def.max_lod, FLT_MAX);
    def.border_color = _sg_def(def.border_color, SG_BORDERCOLOR_OPAQUE_BLACK);
    def.compare = _sg_def(def.compare, SG_COMPAREFUNC_NEVER);
    def.max_anisotropy = _sg_def(def.max_anisotropy, 1);
    return def;
}

_SOKOL_PRIVATE sg_shader_desc _sg_shader_desc_defaults(const sg_shader_desc* desc) {
    sg_shader_desc def = *desc;
    #if defined(SOKOL_METAL)
        def.vs.entry = _sg_def(def.vs.entry, "_main");
        def.fs.entry = _sg_def(def.fs.entry, "_main");
    #else
        def.vs.entry = _sg_def(def.vs.entry, "main");
        def.fs.entry = _sg_def(def.fs.entry, "main");
    #endif
    #if defined(SOKOL_D3D11)
        if (def.vs.source) {
            def.vs.d3d11_target = _sg_def(def.vs.d3d11_target, "vs_4_0");
        }
        if (def.fs.source) {
            def.fs.d3d11_target = _sg_def(def.fs.d3d11_target, "ps_4_0");
        }
    #endif
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        sg_shader_stage_desc* stage_desc = (stage_index == SG_SHADERSTAGE_VS)? &def.vs : &def.fs;
        for (int ub_index = 0; ub_index < SG_MAX_SHADERSTAGE_UBS; ub_index++) {
            sg_shader_uniform_block_desc* ub_desc = &stage_desc->uniform_blocks[ub_index];
            if (0 == ub_desc->size) {
                break;
            }
            ub_desc->layout = _sg_def(ub_desc->layout, SG_UNIFORMLAYOUT_NATIVE);
            for (int u_index = 0; u_index < SG_MAX_UB_MEMBERS; u_index++) {
                sg_shader_uniform_desc* u_desc = &ub_desc->uniforms[u_index];
                if (u_desc->type == SG_UNIFORMTYPE_INVALID) {
                    break;
                }
                u_desc->array_count = _sg_def(u_desc->array_count, 1);
            }
        }
        for (int img_index = 0; img_index < SG_MAX_SHADERSTAGE_IMAGES; img_index++) {
            sg_shader_image_desc* img_desc = &stage_desc->images[img_index];
            if (!img_desc->used) {
                break;
            }
            img_desc->image_type = _sg_def(img_desc->image_type, SG_IMAGETYPE_2D);
            img_desc->sample_type = _sg_def(img_desc->sample_type, SG_IMAGESAMPLETYPE_FLOAT);
        }
        for (int smp_index = 0; smp_index < SG_MAX_SHADERSTAGE_SAMPLERS; smp_index++) {
            sg_shader_sampler_desc* smp_desc = &stage_desc->samplers[smp_index];
            if (!smp_desc->used) {
                break;
            }
            smp_desc->sampler_type = _sg_def(smp_desc->sampler_type, SG_SAMPLERTYPE_FILTERING);
        }
    }
    return def;
}

_SOKOL_PRIVATE sg_pipeline_desc _sg_pipeline_desc_defaults(const sg_pipeline_desc* desc) {
    sg_pipeline_desc def = *desc;

    def.primitive_type = _sg_def(def.primitive_type, SG_PRIMITIVETYPE_TRIANGLES);
    def.index_type = _sg_def(def.index_type, SG_INDEXTYPE_NONE);
    def.cull_mode = _sg_def(def.cull_mode, SG_CULLMODE_NONE);
    def.face_winding = _sg_def(def.face_winding, SG_FACEWINDING_CW);
    def.sample_count = _sg_def(def.sample_count, _sg.desc.environment.defaults.sample_count);

    def.stencil.front.compare = _sg_def(def.stencil.front.compare, SG_COMPAREFUNC_ALWAYS);
    def.stencil.front.fail_op = _sg_def(def.stencil.front.fail_op, SG_STENCILOP_KEEP);
    def.stencil.front.depth_fail_op = _sg_def(def.stencil.front.depth_fail_op, SG_STENCILOP_KEEP);
    def.stencil.front.pass_op = _sg_def(def.stencil.front.pass_op, SG_STENCILOP_KEEP);
    def.stencil.back.compare = _sg_def(def.stencil.back.compare, SG_COMPAREFUNC_ALWAYS);
    def.stencil.back.fail_op = _sg_def(def.stencil.back.fail_op, SG_STENCILOP_KEEP);
    def.stencil.back.depth_fail_op = _sg_def(def.stencil.back.depth_fail_op, SG_STENCILOP_KEEP);
    def.stencil.back.pass_op = _sg_def(def.stencil.back.pass_op, SG_STENCILOP_KEEP);

    def.depth.compare = _sg_def(def.depth.compare, SG_COMPAREFUNC_ALWAYS);
    def.depth.pixel_format = _sg_def(def.depth.pixel_format, _sg.desc.environment.defaults.depth_format);
    if (def.colors[0].pixel_format == SG_PIXELFORMAT_NONE) {
        // special case depth-only rendering, enforce a color count of 0
        def.color_count = 0;
    } else {
        def.color_count = _sg_def(def.color_count, 1);
    }
    if (def.color_count > SG_MAX_COLOR_ATTACHMENTS) {
        def.color_count = SG_MAX_COLOR_ATTACHMENTS;
    }
    for (int i = 0; i < def.color_count; i++) {
        sg_color_target_state* cs = &def.colors[i];
        cs->pixel_format = _sg_def(cs->pixel_format, _sg.desc.environment.defaults.color_format);
        cs->write_mask = _sg_def(cs->write_mask, SG_COLORMASK_RGBA);
        sg_blend_state* bs = &def.colors[i].blend;
        bs->src_factor_rgb = _sg_def(bs->src_factor_rgb, SG_BLENDFACTOR_ONE);
        bs->dst_factor_rgb = _sg_def(bs->dst_factor_rgb, SG_BLENDFACTOR_ZERO);
        bs->op_rgb = _sg_def(bs->op_rgb, SG_BLENDOP_ADD);
        bs->src_factor_alpha = _sg_def(bs->src_factor_alpha, SG_BLENDFACTOR_ONE);
        bs->dst_factor_alpha = _sg_def(bs->dst_factor_alpha, SG_BLENDFACTOR_ZERO);
        bs->op_alpha = _sg_def(bs->op_alpha, SG_BLENDOP_ADD);
    }

    for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        sg_vertex_attr_state* a_state = &def.layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        sg_vertex_buffer_layout_state* l_state = &def.layout.buffers[a_state->buffer_index];
        l_state->step_func = _sg_def(l_state->step_func, SG_VERTEXSTEP_PER_VERTEX);
        l_state->step_rate = _sg_def(l_state->step_rate, 1);
    }

    // resolve vertex layout strides and offsets
    int auto_offset[SG_MAX_VERTEX_BUFFERS];
    _sg_clear(auto_offset, sizeof(auto_offset));
    bool use_auto_offset = true;
    for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        // to use computed offsets, *all* attr offsets must be 0
        if (def.layout.attrs[attr_index].offset != 0) {
            use_auto_offset = false;
        }
    }
    for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        sg_vertex_attr_state* a_state = &def.layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        if (use_auto_offset) {
            a_state->offset = auto_offset[a_state->buffer_index];
        }
        auto_offset[a_state->buffer_index] += _sg_vertexformat_bytesize(a_state->format);
    }
    // compute vertex strides if needed
    for (int buf_index = 0; buf_index < SG_MAX_VERTEX_BUFFERS; buf_index++) {
        sg_vertex_buffer_layout_state* l_state = &def.layout.buffers[buf_index];
        if (l_state->stride == 0) {
            l_state->stride = auto_offset[buf_index];
        }
    }

    return def;
}

_SOKOL_PRIVATE sg_attachments_desc _sg_attachments_desc_defaults(const sg_attachments_desc* desc) {
    sg_attachments_desc def = *desc;
    return def;
}

_SOKOL_PRIVATE sg_buffer _sg_alloc_buffer(void) {
    sg_buffer res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.buffer_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id = _sg_slot_alloc(&_sg.pools.buffer_pool, &_sg.pools.buffers[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(BUFFER_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE sg_image _sg_alloc_image(void) {
    sg_image res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.image_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id = _sg_slot_alloc(&_sg.pools.image_pool, &_sg.pools.images[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(IMAGE_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE sg_sampler _sg_alloc_sampler(void) {
    sg_sampler res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.sampler_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id = _sg_slot_alloc(&_sg.pools.sampler_pool, &_sg.pools.samplers[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(SAMPLER_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE sg_shader _sg_alloc_shader(void) {
    sg_shader res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.shader_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id = _sg_slot_alloc(&_sg.pools.shader_pool, &_sg.pools.shaders[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(SHADER_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE sg_pipeline _sg_alloc_pipeline(void) {
    sg_pipeline res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.pipeline_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id =_sg_slot_alloc(&_sg.pools.pipeline_pool, &_sg.pools.pipelines[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(PIPELINE_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE sg_attachments _sg_alloc_attachments(void) {
    sg_attachments res;
    int slot_index = _sg_pool_alloc_index(&_sg.pools.attachments_pool);
    if (_SG_INVALID_SLOT_INDEX != slot_index) {
        res.id = _sg_slot_alloc(&_sg.pools.attachments_pool, &_sg.pools.attachments[slot_index].slot, slot_index);
    } else {
        res.id = SG_INVALID_ID;
        _SG_ERROR(PASS_POOL_EXHAUSTED);
    }
    return res;
}

_SOKOL_PRIVATE void _sg_dealloc_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf && (buf->slot.state == SG_RESOURCESTATE_ALLOC) && (buf->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.buffer_pool, _sg_slot_index(buf->slot.id));
    _sg_reset_slot(&buf->slot);
}

_SOKOL_PRIVATE void _sg_dealloc_image(_sg_image_t* img) {
    SOKOL_ASSERT(img && (img->slot.state == SG_RESOURCESTATE_ALLOC) && (img->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.image_pool, _sg_slot_index(img->slot.id));
    _sg_reset_slot(&img->slot);
}

_SOKOL_PRIVATE void _sg_dealloc_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp && (smp->slot.state == SG_RESOURCESTATE_ALLOC) && (smp->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.sampler_pool, _sg_slot_index(smp->slot.id));
    _sg_reset_slot(&smp->slot);
}

_SOKOL_PRIVATE void _sg_dealloc_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd && (shd->slot.state == SG_RESOURCESTATE_ALLOC) && (shd->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.shader_pool, _sg_slot_index(shd->slot.id));
    _sg_reset_slot(&shd->slot);
}

_SOKOL_PRIVATE void _sg_dealloc_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip && (pip->slot.state == SG_RESOURCESTATE_ALLOC) && (pip->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.pipeline_pool, _sg_slot_index(pip->slot.id));
    _sg_reset_slot(&pip->slot);
}

_SOKOL_PRIVATE void _sg_dealloc_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts && (atts->slot.state == SG_RESOURCESTATE_ALLOC) && (atts->slot.id != SG_INVALID_ID));
    _sg_pool_free_index(&_sg.pools.attachments_pool, _sg_slot_index(atts->slot.id));
    _sg_reset_slot(&atts->slot);
}

_SOKOL_PRIVATE void _sg_init_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(buf && (buf->slot.state == SG_RESOURCESTATE_ALLOC));
    SOKOL_ASSERT(desc);
    if (_sg_validate_buffer_desc(desc)) {
        _sg_buffer_common_init(&buf->cmn, desc);
        buf->slot.state = _sg_create_buffer(buf, desc);
    } else {
        buf->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((buf->slot.state == SG_RESOURCESTATE_VALID)||(buf->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_init_image(_sg_image_t* img, const sg_image_desc* desc) {
    SOKOL_ASSERT(img && (img->slot.state == SG_RESOURCESTATE_ALLOC));
    SOKOL_ASSERT(desc);
    if (_sg_validate_image_desc(desc)) {
        _sg_image_common_init(&img->cmn, desc);
        img->slot.state = _sg_create_image(img, desc);
    } else {
        img->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((img->slot.state == SG_RESOURCESTATE_VALID)||(img->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_init_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(smp && (smp->slot.state == SG_RESOURCESTATE_ALLOC));
    SOKOL_ASSERT(desc);
    if (_sg_validate_sampler_desc(desc)) {
        _sg_sampler_common_init(&smp->cmn, desc);
        smp->slot.state = _sg_create_sampler(smp, desc);
    } else {
        smp->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((smp->slot.state == SG_RESOURCESTATE_VALID)||(smp->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_init_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    SOKOL_ASSERT(shd && (shd->slot.state == SG_RESOURCESTATE_ALLOC));
    SOKOL_ASSERT(desc);
    if (_sg_validate_shader_desc(desc)) {
        _sg_shader_common_init(&shd->cmn, desc);
        shd->slot.state = _sg_create_shader(shd, desc);
    } else {
        shd->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((shd->slot.state == SG_RESOURCESTATE_VALID)||(shd->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_init_pipeline(_sg_pipeline_t* pip, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(pip && (pip->slot.state == SG_RESOURCESTATE_ALLOC));
    SOKOL_ASSERT(desc);
    if (_sg_validate_pipeline_desc(desc)) {
        _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, desc->shader.id);
        if (shd && (shd->slot.state == SG_RESOURCESTATE_VALID)) {
            _sg_pipeline_common_init(&pip->cmn, desc);
            pip->slot.state = _sg_create_pipeline(pip, shd, desc);
        } else {
            pip->slot.state = SG_RESOURCESTATE_FAILED;
        }
    } else {
        pip->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((pip->slot.state == SG_RESOURCESTATE_VALID)||(pip->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_init_attachments(_sg_attachments_t* atts, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(atts && atts->slot.state == SG_RESOURCESTATE_ALLOC);
    SOKOL_ASSERT(desc);
    if (_sg_validate_attachments_desc(desc)) {
        // lookup pass attachment image pointers
        _sg_image_t* color_images[SG_MAX_COLOR_ATTACHMENTS] = { 0 };
        _sg_image_t* resolve_images[SG_MAX_COLOR_ATTACHMENTS] = { 0 };
        _sg_image_t* ds_image = 0;
        // NOTE: validation already checked that all surfaces are same width/height
        int width = 0;
        int height = 0;
        for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
            if (desc->colors[i].image.id) {
                color_images[i] = _sg_lookup_image(&_sg.pools, desc->colors[i].image.id);
                if (!(color_images[i] && color_images[i]->slot.state == SG_RESOURCESTATE_VALID)) {
                    atts->slot.state = SG_RESOURCESTATE_FAILED;
                    return;
                }
                const int mip_level = desc->colors[i].mip_level;
                width = _sg_miplevel_dim(color_images[i]->cmn.width, mip_level);
                height = _sg_miplevel_dim(color_images[i]->cmn.height, mip_level);
            }
            if (desc->resolves[i].image.id) {
                resolve_images[i] = _sg_lookup_image(&_sg.pools, desc->resolves[i].image.id);
                if (!(resolve_images[i] && resolve_images[i]->slot.state == SG_RESOURCESTATE_VALID)) {
                    atts->slot.state = SG_RESOURCESTATE_FAILED;
                    return;
                }
            }
        }
        if (desc->depth_stencil.image.id) {
            ds_image = _sg_lookup_image(&_sg.pools, desc->depth_stencil.image.id);
            if (!(ds_image && ds_image->slot.state == SG_RESOURCESTATE_VALID)) {
                atts->slot.state = SG_RESOURCESTATE_FAILED;
                return;
            }
            const int mip_level = desc->depth_stencil.mip_level;
            width = _sg_miplevel_dim(ds_image->cmn.width, mip_level);
            height = _sg_miplevel_dim(ds_image->cmn.height, mip_level);
        }
        _sg_attachments_common_init(&atts->cmn, desc, width, height);
        atts->slot.state = _sg_create_attachments(atts, color_images, resolve_images, ds_image, desc);
    } else {
        atts->slot.state = SG_RESOURCESTATE_FAILED;
    }
    SOKOL_ASSERT((atts->slot.state == SG_RESOURCESTATE_VALID)||(atts->slot.state == SG_RESOURCESTATE_FAILED));
}

_SOKOL_PRIVATE void _sg_uninit_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf && ((buf->slot.state == SG_RESOURCESTATE_VALID) || (buf->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_buffer(buf);
    _sg_reset_buffer_to_alloc_state(buf);
}

_SOKOL_PRIVATE void _sg_uninit_image(_sg_image_t* img) {
    SOKOL_ASSERT(img && ((img->slot.state == SG_RESOURCESTATE_VALID) || (img->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_image(img);
    _sg_reset_image_to_alloc_state(img);
}

_SOKOL_PRIVATE void _sg_uninit_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp && ((smp->slot.state == SG_RESOURCESTATE_VALID) || (smp->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_sampler(smp);
    _sg_reset_sampler_to_alloc_state(smp);
}

_SOKOL_PRIVATE void _sg_uninit_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd && ((shd->slot.state == SG_RESOURCESTATE_VALID) || (shd->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_shader(shd);
    _sg_reset_shader_to_alloc_state(shd);
}

_SOKOL_PRIVATE void _sg_uninit_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip && ((pip->slot.state == SG_RESOURCESTATE_VALID) || (pip->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_pipeline(pip);
    _sg_reset_pipeline_to_alloc_state(pip);
}

_SOKOL_PRIVATE void _sg_uninit_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts && ((atts->slot.state == SG_RESOURCESTATE_VALID) || (atts->slot.state == SG_RESOURCESTATE_FAILED)));
    _sg_discard_attachments(atts);
    _sg_reset_attachments_to_alloc_state(atts);
}

_SOKOL_PRIVATE void _sg_setup_commit_listeners(const sg_desc* desc) {
    SOKOL_ASSERT(desc->max_commit_listeners > 0);
    SOKOL_ASSERT(0 == _sg.commit_listeners.items);
    SOKOL_ASSERT(0 == _sg.commit_listeners.num);
    SOKOL_ASSERT(0 == _sg.commit_listeners.upper);
    _sg.commit_listeners.num = desc->max_commit_listeners;
    const size_t size = (size_t)_sg.commit_listeners.num * sizeof(sg_commit_listener);
    _sg.commit_listeners.items = (sg_commit_listener*)_sg_malloc_clear(size);
}

_SOKOL_PRIVATE void _sg_discard_commit_listeners(void) {
    SOKOL_ASSERT(0 != _sg.commit_listeners.items);
    _sg_free(_sg.commit_listeners.items);
    _sg.commit_listeners.items = 0;
}

_SOKOL_PRIVATE void _sg_notify_commit_listeners(void) {
    SOKOL_ASSERT(_sg.commit_listeners.items);
    for (int i = 0; i < _sg.commit_listeners.upper; i++) {
        const sg_commit_listener* listener = &_sg.commit_listeners.items[i];
        if (listener->func) {
            listener->func(listener->user_data);
        }
    }
}

_SOKOL_PRIVATE bool _sg_add_commit_listener(const sg_commit_listener* new_listener) {
    SOKOL_ASSERT(new_listener && new_listener->func);
    SOKOL_ASSERT(_sg.commit_listeners.items);
    // first check if the listener hadn't been added already
    for (int i = 0; i < _sg.commit_listeners.upper; i++) {
        const sg_commit_listener* slot = &_sg.commit_listeners.items[i];
        if ((slot->func == new_listener->func) && (slot->user_data == new_listener->user_data)) {
            _SG_ERROR(IDENTICAL_COMMIT_LISTENER);
            return false;
        }
    }
    // first try to plug a hole
    sg_commit_listener* slot = 0;
    for (int i = 0; i < _sg.commit_listeners.upper; i++) {
        if (_sg.commit_listeners.items[i].func == 0) {
            slot = &_sg.commit_listeners.items[i];
            break;
        }
    }
    if (!slot) {
        // append to end
        if (_sg.commit_listeners.upper < _sg.commit_listeners.num) {
            slot = &_sg.commit_listeners.items[_sg.commit_listeners.upper++];
        }
    }
    if (!slot) {
        _SG_ERROR(COMMIT_LISTENER_ARRAY_FULL);
        return false;
    }
    *slot = *new_listener;
    return true;
}

_SOKOL_PRIVATE bool _sg_remove_commit_listener(const sg_commit_listener* listener) {
    SOKOL_ASSERT(listener && listener->func);
    SOKOL_ASSERT(_sg.commit_listeners.items);
    for (int i = 0; i < _sg.commit_listeners.upper; i++) {
        sg_commit_listener* slot = &_sg.commit_listeners.items[i];
        // both the function pointer and user data must match!
        if ((slot->func == listener->func) && (slot->user_data == listener->user_data)) {
            slot->func = 0;
            slot->user_data = 0;
            // NOTE: since _sg_add_commit_listener() already catches duplicates,
            // we don't need to worry about them here
            return true;
        }
    }
    return false;
}

_SOKOL_PRIVATE sg_desc _sg_desc_defaults(const sg_desc* desc) {
    /*
        NOTE: on WebGPU, the default color pixel format MUST be provided,
        cannot be a default compile-time constant.
    */
    sg_desc res = *desc;
    #if defined(SOKOL_METAL) || defined(SOKOL_D3D11)
        res.environment.defaults.color_format = _sg_def(res.environment.defaults.color_format, SG_PIXELFORMAT_BGRA8);
    #else
        res.environment.defaults.color_format = _sg_def(res.environment.defaults.color_format, SG_PIXELFORMAT_RGBA8);
    #endif
    res.environment.defaults.depth_format = _sg_def(res.environment.defaults.depth_format, SG_PIXELFORMAT_DEPTH_STENCIL);
    res.environment.defaults.sample_count = _sg_def(res.environment.defaults.sample_count, 1);
    res.buffer_pool_size = _sg_def(res.buffer_pool_size, _SG_DEFAULT_BUFFER_POOL_SIZE);
    res.image_pool_size = _sg_def(res.image_pool_size, _SG_DEFAULT_IMAGE_POOL_SIZE);
    res.sampler_pool_size = _sg_def(res.sampler_pool_size, _SG_DEFAULT_SAMPLER_POOL_SIZE);
    res.shader_pool_size = _sg_def(res.shader_pool_size, _SG_DEFAULT_SHADER_POOL_SIZE);
    res.pipeline_pool_size = _sg_def(res.pipeline_pool_size, _SG_DEFAULT_PIPELINE_POOL_SIZE);
    res.attachments_pool_size = _sg_def(res.attachments_pool_size, _SG_DEFAULT_ATTACHMENTS_POOL_SIZE);
    res.uniform_buffer_size = _sg_def(res.uniform_buffer_size, _SG_DEFAULT_UB_SIZE);
    res.max_commit_listeners = _sg_def(res.max_commit_listeners, _SG_DEFAULT_MAX_COMMIT_LISTENERS);
    return res;
}

_SOKOL_PRIVATE sg_pass _sg_pass_defaults(const sg_pass* pass) {
    sg_pass res = *pass;
    if (res.attachments.id == SG_INVALID_ID) {
        // this is a swapchain-pass
        res.swapchain.sample_count = _sg_def(res.swapchain.sample_count, _sg.desc.environment.defaults.sample_count);
        res.swapchain.color_format = _sg_def(res.swapchain.color_format, _sg.desc.environment.defaults.color_format);
        res.swapchain.depth_format = _sg_def(res.swapchain.depth_format, _sg.desc.environment.defaults.depth_format);
    }
    res.action = _sg_pass_action_defaults(&res.action);
    return res;
}

// ██████  ██    ██ ██████  ██      ██  ██████
// ██   ██ ██    ██ ██   ██ ██      ██ ██
// ██████  ██    ██ ██████  ██      ██ ██
// ██      ██    ██ ██   ██ ██      ██ ██
// ██       ██████  ██████  ███████ ██  ██████
//
// >>public
SOKOL_API_IMPL void sg_setup(const sg_desc* desc) {
    SOKOL_ASSERT(desc);
    SOKOL_ASSERT((desc->_start_canary == 0) && (desc->_end_canary == 0));
    SOKOL_ASSERT((desc->allocator.alloc_fn && desc->allocator.free_fn) || (!desc->allocator.alloc_fn && !desc->allocator.free_fn));
    _SG_CLEAR_ARC_STRUCT(_sg_state_t, _sg);
    _sg.desc = _sg_desc_defaults(desc);
    _sg_setup_pools(&_sg.pools, &_sg.desc);
    _sg_setup_commit_listeners(&_sg.desc);
    _sg.frame_index = 1;
    _sg.stats_enabled = true;
    _sg_setup_backend(&_sg.desc);
    _sg.valid = true;
}

SOKOL_API_IMPL void sg_shutdown(void) {
    _sg_discard_all_resources(&_sg.pools);
    _sg_discard_backend();
    _sg_discard_commit_listeners();
    _sg_discard_pools(&_sg.pools);
    _SG_CLEAR_ARC_STRUCT(_sg_state_t, _sg);
}

SOKOL_API_IMPL bool sg_isvalid(void) {
    return _sg.valid;
}

SOKOL_API_IMPL sg_desc sg_query_desc(void) {
    SOKOL_ASSERT(_sg.valid);
    return _sg.desc;
}

SOKOL_API_IMPL sg_backend sg_query_backend(void) {
    SOKOL_ASSERT(_sg.valid);
    return _sg.backend;
}

SOKOL_API_IMPL sg_features sg_query_features(void) {
    SOKOL_ASSERT(_sg.valid);
    return _sg.features;
}

SOKOL_API_IMPL sg_limits sg_query_limits(void) {
    SOKOL_ASSERT(_sg.valid);
    return _sg.limits;
}

SOKOL_API_IMPL sg_pixelformat_info sg_query_pixelformat(sg_pixel_format fmt) {
    SOKOL_ASSERT(_sg.valid);
    int fmt_index = (int) fmt;
    SOKOL_ASSERT((fmt_index > SG_PIXELFORMAT_NONE) && (fmt_index < _SG_PIXELFORMAT_NUM));
    const _sg_pixelformat_info_t* src = &_sg.formats[fmt_index];
    sg_pixelformat_info res;
    _sg_clear(&res, sizeof(res));
    res.sample = src->sample;
    res.filter = src->filter;
    res.render = src->render;
    res.blend = src->blend;
    res.msaa = src->msaa;
    res.depth = src->depth;
    res.compressed = _sg_is_compressed_pixel_format(fmt);
    if (!res.compressed) {
        res.bytes_per_pixel = _sg_pixelformat_bytesize(fmt);
    }
    return res;
}

SOKOL_API_IMPL int sg_query_row_pitch(sg_pixel_format fmt, int width, int row_align_bytes) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(width > 0);
    SOKOL_ASSERT((row_align_bytes > 0) && _sg_ispow2(row_align_bytes));
    SOKOL_ASSERT(((int)fmt > SG_PIXELFORMAT_NONE) && ((int)fmt < _SG_PIXELFORMAT_NUM));
    return _sg_row_pitch(fmt, width, row_align_bytes);
}

SOKOL_API_IMPL int sg_query_surface_pitch(sg_pixel_format fmt, int width, int height, int row_align_bytes) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT((width > 0) && (height > 0));
    SOKOL_ASSERT((row_align_bytes > 0) && _sg_ispow2(row_align_bytes));
    SOKOL_ASSERT(((int)fmt > SG_PIXELFORMAT_NONE) && ((int)fmt < _SG_PIXELFORMAT_NUM));
    return _sg_surface_pitch(fmt, width, height, row_align_bytes);
}

SOKOL_API_IMPL sg_frame_stats sg_query_frame_stats(void) {
    SOKOL_ASSERT(_sg.valid);
    return _sg.prev_stats;
}

SOKOL_API_IMPL sg_trace_hooks sg_install_trace_hooks(const sg_trace_hooks* trace_hooks) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(trace_hooks);
    _SOKOL_UNUSED(trace_hooks);
    #if defined(SOKOL_TRACE_HOOKS)
        sg_trace_hooks old_hooks = _sg.hooks;
        _sg.hooks = *trace_hooks;
    #else
        static sg_trace_hooks old_hooks;
        _SG_WARN(TRACE_HOOKS_NOT_ENABLED);
    #endif
    return old_hooks;
}

SOKOL_API_IMPL sg_buffer sg_alloc_buffer(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_buffer res = _sg_alloc_buffer();
    _SG_TRACE_ARGS(alloc_buffer, res);
    return res;
}

SOKOL_API_IMPL sg_image sg_alloc_image(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_image res = _sg_alloc_image();
    _SG_TRACE_ARGS(alloc_image, res);
    return res;
}

SOKOL_API_IMPL sg_sampler sg_alloc_sampler(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_sampler res = _sg_alloc_sampler();
    _SG_TRACE_ARGS(alloc_sampler, res);
    return res;
}

SOKOL_API_IMPL sg_shader sg_alloc_shader(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_shader res = _sg_alloc_shader();
    _SG_TRACE_ARGS(alloc_shader, res);
    return res;
}

SOKOL_API_IMPL sg_pipeline sg_alloc_pipeline(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_pipeline res = _sg_alloc_pipeline();
    _SG_TRACE_ARGS(alloc_pipeline, res);
    return res;
}

SOKOL_API_IMPL sg_attachments sg_alloc_attachments(void) {
    SOKOL_ASSERT(_sg.valid);
    sg_attachments res = _sg_alloc_attachments();
    _SG_TRACE_ARGS(alloc_attachments, res);
    return res;
}

SOKOL_API_IMPL void sg_dealloc_buffer(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        if (buf->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_buffer(buf);
        } else {
            _SG_ERROR(DEALLOC_BUFFER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_buffer, buf_id);
}

SOKOL_API_IMPL void sg_dealloc_image(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        if (img->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_image(img);
        } else {
            _SG_ERROR(DEALLOC_IMAGE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_image, img_id);
}

SOKOL_API_IMPL void sg_dealloc_sampler(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        if (smp->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_sampler(smp);
        } else {
            _SG_ERROR(DEALLOC_SAMPLER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_sampler, smp_id);
}

SOKOL_API_IMPL void sg_dealloc_shader(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        if (shd->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_shader(shd);
        } else {
            _SG_ERROR(DEALLOC_SHADER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_shader, shd_id);
}

SOKOL_API_IMPL void sg_dealloc_pipeline(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        if (pip->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_pipeline(pip);
        } else {
            _SG_ERROR(DEALLOC_PIPELINE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_pipeline, pip_id);
}

SOKOL_API_IMPL void sg_dealloc_attachments(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        if (atts->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_attachments(atts);
        } else {
            _SG_ERROR(DEALLOC_ATTACHMENTS_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(dealloc_attachments, atts_id);
}

SOKOL_API_IMPL void sg_init_buffer(sg_buffer buf_id, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_buffer_desc desc_def = _sg_buffer_desc_defaults(desc);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        if (buf->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_buffer(buf, &desc_def);
            SOKOL_ASSERT((buf->slot.state == SG_RESOURCESTATE_VALID) || (buf->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_BUFFER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_buffer, buf_id, &desc_def);
}

SOKOL_API_IMPL void sg_init_image(sg_image img_id, const sg_image_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_image_desc desc_def = _sg_image_desc_defaults(desc);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        if (img->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_image(img, &desc_def);
            SOKOL_ASSERT((img->slot.state == SG_RESOURCESTATE_VALID) || (img->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_IMAGE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_image, img_id, &desc_def);
}

SOKOL_API_IMPL void sg_init_sampler(sg_sampler smp_id, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_sampler_desc desc_def = _sg_sampler_desc_defaults(desc);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        if (smp->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_sampler(smp, &desc_def);
            SOKOL_ASSERT((smp->slot.state == SG_RESOURCESTATE_VALID) || (smp->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_SAMPLER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_sampler, smp_id, &desc_def);
}

SOKOL_API_IMPL void sg_init_shader(sg_shader shd_id, const sg_shader_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_shader_desc desc_def = _sg_shader_desc_defaults(desc);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        if (shd->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_shader(shd, &desc_def);
            SOKOL_ASSERT((shd->slot.state == SG_RESOURCESTATE_VALID) || (shd->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_SHADER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_shader, shd_id, &desc_def);
}

SOKOL_API_IMPL void sg_init_pipeline(sg_pipeline pip_id, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_pipeline_desc desc_def = _sg_pipeline_desc_defaults(desc);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        if (pip->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_pipeline(pip, &desc_def);
            SOKOL_ASSERT((pip->slot.state == SG_RESOURCESTATE_VALID) || (pip->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_PIPELINE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_pipeline, pip_id, &desc_def);
}

SOKOL_API_IMPL void sg_init_attachments(sg_attachments atts_id, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    sg_attachments_desc desc_def = _sg_attachments_desc_defaults(desc);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        if (atts->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_init_attachments(atts, &desc_def);
            SOKOL_ASSERT((atts->slot.state == SG_RESOURCESTATE_VALID) || (atts->slot.state == SG_RESOURCESTATE_FAILED));
        } else {
            _SG_ERROR(INIT_ATTACHMENTS_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(init_attachments, atts_id, &desc_def);
}

SOKOL_API_IMPL void sg_uninit_buffer(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        if ((buf->slot.state == SG_RESOURCESTATE_VALID) || (buf->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_buffer(buf);
            SOKOL_ASSERT(buf->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_BUFFER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_buffer, buf_id);
}

SOKOL_API_IMPL void sg_uninit_image(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        if ((img->slot.state == SG_RESOURCESTATE_VALID) || (img->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_image(img);
            SOKOL_ASSERT(img->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_IMAGE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_image, img_id);
}

SOKOL_API_IMPL void sg_uninit_sampler(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        if ((smp->slot.state == SG_RESOURCESTATE_VALID) || (smp->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_sampler(smp);
            SOKOL_ASSERT(smp->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_SAMPLER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_sampler, smp_id);
}

SOKOL_API_IMPL void sg_uninit_shader(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        if ((shd->slot.state == SG_RESOURCESTATE_VALID) || (shd->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_shader(shd);
            SOKOL_ASSERT(shd->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_SHADER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_shader, shd_id);
}

SOKOL_API_IMPL void sg_uninit_pipeline(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        if ((pip->slot.state == SG_RESOURCESTATE_VALID) || (pip->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_pipeline(pip);
            SOKOL_ASSERT(pip->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_PIPELINE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_pipeline, pip_id);
}

SOKOL_API_IMPL void sg_uninit_attachments(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        if ((atts->slot.state == SG_RESOURCESTATE_VALID) || (atts->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_attachments(atts);
            SOKOL_ASSERT(atts->slot.state == SG_RESOURCESTATE_ALLOC);
        } else {
            _SG_ERROR(UNINIT_ATTACHMENTS_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(uninit_attachments, atts_id);
}

SOKOL_API_IMPL void sg_fail_buffer(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        if (buf->slot.state == SG_RESOURCESTATE_ALLOC) {
            buf->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_BUFFER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_buffer, buf_id);
}

SOKOL_API_IMPL void sg_fail_image(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        if (img->slot.state == SG_RESOURCESTATE_ALLOC) {
            img->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_IMAGE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_image, img_id);
}

SOKOL_API_IMPL void sg_fail_sampler(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        if (smp->slot.state == SG_RESOURCESTATE_ALLOC) {
            smp->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_SAMPLER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_sampler, smp_id);
}

SOKOL_API_IMPL void sg_fail_shader(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        if (shd->slot.state == SG_RESOURCESTATE_ALLOC) {
            shd->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_SHADER_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_shader, shd_id);
}

SOKOL_API_IMPL void sg_fail_pipeline(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        if (pip->slot.state == SG_RESOURCESTATE_ALLOC) {
            pip->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_PIPELINE_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_pipeline, pip_id);
}

SOKOL_API_IMPL void sg_fail_attachments(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        if (atts->slot.state == SG_RESOURCESTATE_ALLOC) {
            atts->slot.state = SG_RESOURCESTATE_FAILED;
        } else {
            _SG_ERROR(FAIL_ATTACHMENTS_INVALID_STATE);
        }
    }
    _SG_TRACE_ARGS(fail_attachments, atts_id);
}

SOKOL_API_IMPL sg_resource_state sg_query_buffer_state(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    sg_resource_state res = buf ? buf->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_resource_state sg_query_image_state(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    sg_resource_state res = img ? img->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_resource_state sg_query_sampler_state(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    sg_resource_state res = smp ? smp->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_resource_state sg_query_shader_state(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    sg_resource_state res = shd ? shd->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_resource_state sg_query_pipeline_state(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    sg_resource_state res = pip ? pip->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_resource_state sg_query_attachments_state(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    sg_resource_state res = atts ? atts->slot.state : SG_RESOURCESTATE_INVALID;
    return res;
}

SOKOL_API_IMPL sg_buffer sg_make_buffer(const sg_buffer_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_buffer_desc desc_def = _sg_buffer_desc_defaults(desc);
    sg_buffer buf_id = _sg_alloc_buffer();
    if (buf_id.id != SG_INVALID_ID) {
        _sg_buffer_t* buf = _sg_buffer_at(&_sg.pools, buf_id.id);
        SOKOL_ASSERT(buf && (buf->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_buffer(buf, &desc_def);
        SOKOL_ASSERT((buf->slot.state == SG_RESOURCESTATE_VALID) || (buf->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_buffer, &desc_def, buf_id);
    return buf_id;
}

SOKOL_API_IMPL sg_image sg_make_image(const sg_image_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_image_desc desc_def = _sg_image_desc_defaults(desc);
    sg_image img_id = _sg_alloc_image();
    if (img_id.id != SG_INVALID_ID) {
        _sg_image_t* img = _sg_image_at(&_sg.pools, img_id.id);
        SOKOL_ASSERT(img && (img->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_image(img, &desc_def);
        SOKOL_ASSERT((img->slot.state == SG_RESOURCESTATE_VALID) || (img->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_image, &desc_def, img_id);
    return img_id;
}

SOKOL_API_IMPL sg_sampler sg_make_sampler(const sg_sampler_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_sampler_desc desc_def = _sg_sampler_desc_defaults(desc);
    sg_sampler smp_id = _sg_alloc_sampler();
    if (smp_id.id != SG_INVALID_ID) {
        _sg_sampler_t* smp = _sg_sampler_at(&_sg.pools, smp_id.id);
        SOKOL_ASSERT(smp && (smp->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_sampler(smp, &desc_def);
        SOKOL_ASSERT((smp->slot.state == SG_RESOURCESTATE_VALID) || (smp->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_sampler, &desc_def, smp_id);
    return smp_id;
}

SOKOL_API_IMPL sg_shader sg_make_shader(const sg_shader_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_shader_desc desc_def = _sg_shader_desc_defaults(desc);
    sg_shader shd_id = _sg_alloc_shader();
    if (shd_id.id != SG_INVALID_ID) {
        _sg_shader_t* shd = _sg_shader_at(&_sg.pools, shd_id.id);
        SOKOL_ASSERT(shd && (shd->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_shader(shd, &desc_def);
        SOKOL_ASSERT((shd->slot.state == SG_RESOURCESTATE_VALID) || (shd->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_shader, &desc_def, shd_id);
    return shd_id;
}

SOKOL_API_IMPL sg_pipeline sg_make_pipeline(const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_pipeline_desc desc_def = _sg_pipeline_desc_defaults(desc);
    sg_pipeline pip_id = _sg_alloc_pipeline();
    if (pip_id.id != SG_INVALID_ID) {
        _sg_pipeline_t* pip = _sg_pipeline_at(&_sg.pools, pip_id.id);
        SOKOL_ASSERT(pip && (pip->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_pipeline(pip, &desc_def);
        SOKOL_ASSERT((pip->slot.state == SG_RESOURCESTATE_VALID) || (pip->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_pipeline, &desc_def, pip_id);
    return pip_id;
}

SOKOL_API_IMPL sg_attachments sg_make_attachments(const sg_attachments_desc* desc) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(desc);
    sg_attachments_desc desc_def = _sg_attachments_desc_defaults(desc);
    sg_attachments atts_id = _sg_alloc_attachments();
    if (atts_id.id != SG_INVALID_ID) {
        _sg_attachments_t* atts = _sg_attachments_at(&_sg.pools, atts_id.id);
        SOKOL_ASSERT(atts && (atts->slot.state == SG_RESOURCESTATE_ALLOC));
        _sg_init_attachments(atts, &desc_def);
        SOKOL_ASSERT((atts->slot.state == SG_RESOURCESTATE_VALID) || (atts->slot.state == SG_RESOURCESTATE_FAILED));
    }
    _SG_TRACE_ARGS(make_attachments, &desc_def, atts_id);
    return atts_id;
}

SOKOL_API_IMPL void sg_destroy_buffer(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_buffer, buf_id);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        if ((buf->slot.state == SG_RESOURCESTATE_VALID) || (buf->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_buffer(buf);
            SOKOL_ASSERT(buf->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (buf->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_buffer(buf);
            SOKOL_ASSERT(buf->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_destroy_image(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_image, img_id);
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        if ((img->slot.state == SG_RESOURCESTATE_VALID) || (img->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_image(img);
            SOKOL_ASSERT(img->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (img->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_image(img);
            SOKOL_ASSERT(img->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_destroy_sampler(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_sampler, smp_id);
    _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        if ((smp->slot.state == SG_RESOURCESTATE_VALID) || (smp->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_sampler(smp);
            SOKOL_ASSERT(smp->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (smp->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_sampler(smp);
            SOKOL_ASSERT(smp->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_destroy_shader(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_shader, shd_id);
    _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        if ((shd->slot.state == SG_RESOURCESTATE_VALID) || (shd->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_shader(shd);
            SOKOL_ASSERT(shd->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (shd->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_shader(shd);
            SOKOL_ASSERT(shd->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_destroy_pipeline(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_pipeline, pip_id);
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        if ((pip->slot.state == SG_RESOURCESTATE_VALID) || (pip->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_pipeline(pip);
            SOKOL_ASSERT(pip->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (pip->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_pipeline(pip);
            SOKOL_ASSERT(pip->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_destroy_attachments(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    _SG_TRACE_ARGS(destroy_attachments, atts_id);
    _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        if ((atts->slot.state == SG_RESOURCESTATE_VALID) || (atts->slot.state == SG_RESOURCESTATE_FAILED)) {
            _sg_uninit_attachments(atts);
            SOKOL_ASSERT(atts->slot.state == SG_RESOURCESTATE_ALLOC);
        }
        if (atts->slot.state == SG_RESOURCESTATE_ALLOC) {
            _sg_dealloc_attachments(atts);
            SOKOL_ASSERT(atts->slot.state == SG_RESOURCESTATE_INITIAL);
        }
    }
}

SOKOL_API_IMPL void sg_begin_pass(const sg_pass* pass) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(!_sg.cur_pass.valid);
    SOKOL_ASSERT(!_sg.cur_pass.in_pass);
    SOKOL_ASSERT(pass);
    SOKOL_ASSERT((pass->_start_canary == 0) && (pass->_end_canary == 0));
    const sg_pass pass_def = _sg_pass_defaults(pass);
    if (!_sg_validate_begin_pass(&pass_def)) {
        return;
    }
    if (pass_def.attachments.id != SG_INVALID_ID) {
        // an offscreen pass
        SOKOL_ASSERT(_sg.cur_pass.atts == 0);
        _sg.cur_pass.atts = _sg_lookup_attachments(&_sg.pools, pass_def.attachments.id);
        if (0 == _sg.cur_pass.atts) {
            _SG_ERROR(BEGINPASS_ATTACHMENT_INVALID);
            return;
        }
        _sg.cur_pass.atts_id = pass_def.attachments;
        _sg.cur_pass.width = _sg.cur_pass.atts->cmn.width;
        _sg.cur_pass.height = _sg.cur_pass.atts->cmn.height;
    } else {
        // a swapchain pass
        SOKOL_ASSERT(pass_def.swapchain.width > 0);
        SOKOL_ASSERT(pass_def.swapchain.height > 0);
        SOKOL_ASSERT(pass_def.swapchain.color_format > SG_PIXELFORMAT_NONE);
        SOKOL_ASSERT(pass_def.swapchain.sample_count > 0);
        _sg.cur_pass.width = pass_def.swapchain.width;
        _sg.cur_pass.height = pass_def.swapchain.height;
        _sg.cur_pass.swapchain.color_fmt = pass_def.swapchain.color_format;
        _sg.cur_pass.swapchain.depth_fmt = pass_def.swapchain.depth_format;
        _sg.cur_pass.swapchain.sample_count = pass_def.swapchain.sample_count;
    }
    _sg.cur_pass.valid = true;  // may be overruled by backend begin-pass functions
    _sg.cur_pass.in_pass = true;
    _sg_begin_pass(&pass_def);
    _SG_TRACE_ARGS(begin_pass, &pass_def);
}

SOKOL_API_IMPL void sg_apply_viewport(int x, int y, int width, int height, bool origin_top_left) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    _sg_stats_add(num_apply_viewport, 1);
    if (!_sg.cur_pass.valid) {
        return;
    }
    _sg_apply_viewport(x, y, width, height, origin_top_left);
    _SG_TRACE_ARGS(apply_viewport, x, y, width, height, origin_top_left);
}

SOKOL_API_IMPL void sg_apply_viewportf(float x, float y, float width, float height, bool origin_top_left) {
    sg_apply_viewport((int)x, (int)y, (int)width, (int)height, origin_top_left);
}

SOKOL_API_IMPL void sg_apply_scissor_rect(int x, int y, int width, int height, bool origin_top_left) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    _sg_stats_add(num_apply_scissor_rect, 1);
    if (!_sg.cur_pass.valid) {
        return;
    }
    _sg_apply_scissor_rect(x, y, width, height, origin_top_left);
    _SG_TRACE_ARGS(apply_scissor_rect, x, y, width, height, origin_top_left);
}

SOKOL_API_IMPL void sg_apply_scissor_rectf(float x, float y, float width, float height, bool origin_top_left) {
    sg_apply_scissor_rect((int)x, (int)y, (int)width, (int)height, origin_top_left);
}

SOKOL_API_IMPL void sg_apply_pipeline(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    _sg_stats_add(num_apply_pipeline, 1);
    if (!_sg_validate_apply_pipeline(pip_id)) {
        _sg.next_draw_valid = false;
        return;
    }
    if (!_sg.cur_pass.valid) {
        return;
    }
    _sg.cur_pipeline = pip_id;
    _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    SOKOL_ASSERT(pip);
    _sg.next_draw_valid = (SG_RESOURCESTATE_VALID == pip->slot.state);
    SOKOL_ASSERT(pip->shader && (pip->shader->slot.id == pip->cmn.shader_id.id));
    _sg_apply_pipeline(pip);
    _SG_TRACE_ARGS(apply_pipeline, pip_id);
}

SOKOL_API_IMPL void sg_apply_bindings(const sg_bindings* bindings) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    SOKOL_ASSERT(bindings);
    SOKOL_ASSERT((bindings->_start_canary == 0) && (bindings->_end_canary==0));
    _sg_stats_add(num_apply_bindings, 1);
    if (!_sg_validate_apply_bindings(bindings)) {
        _sg.next_draw_valid = false;
        return;
    }
    if (!_sg.cur_pass.valid) {
        return;
    }

    _sg_bindings_t bnd;
    _sg_clear(&bnd, sizeof(bnd));
    bnd.pip = _sg_lookup_pipeline(&_sg.pools, _sg.cur_pipeline.id);
    if (0 == bnd.pip) {
        _sg.next_draw_valid = false;
    }

    for (int i = 0; i < SG_MAX_VERTEX_BUFFERS; i++, bnd.num_vbs++) {
        if (bindings->vertex_buffers[i].id) {
            bnd.vbs[i] = _sg_lookup_buffer(&_sg.pools, bindings->vertex_buffers[i].id);
            bnd.vb_offsets[i] = bindings->vertex_buffer_offsets[i];
            if (bnd.vbs[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.vbs[i]->slot.state);
                _sg.next_draw_valid &= !bnd.vbs[i]->cmn.append_overflow;
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    if (bindings->index_buffer.id) {
        bnd.ib = _sg_lookup_buffer(&_sg.pools, bindings->index_buffer.id);
        bnd.ib_offset = bindings->index_buffer_offset;
        if (bnd.ib) {
            _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.ib->slot.state);
            _sg.next_draw_valid &= !bnd.ib->cmn.append_overflow;
        } else {
            _sg.next_draw_valid = false;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_IMAGES; i++, bnd.num_vs_imgs++) {
        if (bindings->vs.images[i].id) {
            bnd.vs_imgs[i] = _sg_lookup_image(&_sg.pools, bindings->vs.images[i].id);
            if (bnd.vs_imgs[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.vs_imgs[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_SAMPLERS; i++, bnd.num_vs_smps++) {
        if (bindings->vs.samplers[i].id) {
            bnd.vs_smps[i] = _sg_lookup_sampler(&_sg.pools, bindings->vs.samplers[i].id);
            if (bnd.vs_smps[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.vs_smps[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++, bnd.num_vs_sbufs++) {
        if (bindings->vs.storage_buffers[i].id) {
            bnd.vs_sbufs[i] = _sg_lookup_buffer(&_sg.pools, bindings->vs.storage_buffers[i].id);
            if (bnd.vs_sbufs[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.vs_sbufs[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_IMAGES; i++, bnd.num_fs_imgs++) {
        if (bindings->fs.images[i].id) {
            bnd.fs_imgs[i] = _sg_lookup_image(&_sg.pools, bindings->fs.images[i].id);
            if (bnd.fs_imgs[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.fs_imgs[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_SAMPLERS; i++, bnd.num_fs_smps++) {
        if (bindings->fs.samplers[i].id) {
            bnd.fs_smps[i] = _sg_lookup_sampler(&_sg.pools, bindings->fs.samplers[i].id);
            if (bnd.fs_smps[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.fs_smps[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }

    for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++, bnd.num_fs_sbufs++) {
        if (bindings->fs.storage_buffers[i].id) {
            bnd.fs_sbufs[i] = _sg_lookup_buffer(&_sg.pools, bindings->fs.storage_buffers[i].id);
            if (bnd.fs_sbufs[i]) {
                _sg.next_draw_valid &= (SG_RESOURCESTATE_VALID == bnd.fs_sbufs[i]->slot.state);
            } else {
                _sg.next_draw_valid = false;
            }
        } else {
            break;
        }
    }
    if (_sg.next_draw_valid) {
        _sg.next_draw_valid &= _sg_apply_bindings(&bnd);
        _SG_TRACE_ARGS(apply_bindings, bindings);
    }
}

SOKOL_API_IMPL void sg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    SOKOL_ASSERT((stage == SG_SHADERSTAGE_VS) || (stage == SG_SHADERSTAGE_FS));
    SOKOL_ASSERT((ub_index >= 0) && (ub_index < SG_MAX_SHADERSTAGE_UBS));
    SOKOL_ASSERT(data && data->ptr && (data->size > 0));
    _sg_stats_add(num_apply_uniforms, 1);
    _sg_stats_add(size_apply_uniforms, (uint32_t)data->size);
    if (!_sg_validate_apply_uniforms(stage, ub_index, data)) {
        _sg.next_draw_valid = false;
        return;
    }
    if (!_sg.cur_pass.valid) {
        return;
    }
    if (!_sg.next_draw_valid) {
        return;
    }
    _sg_apply_uniforms(stage, ub_index, data);
    _SG_TRACE_ARGS(apply_uniforms, stage, ub_index, data);
}

SOKOL_API_IMPL void sg_draw(int base_element, int num_elements, int num_instances) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    SOKOL_ASSERT(base_element >= 0);
    SOKOL_ASSERT(num_elements >= 0);
    SOKOL_ASSERT(num_instances >= 0);
    _sg_stats_add(num_draw, 1);
    if (!_sg.cur_pass.valid) {
        return;
    }
    if (!_sg.next_draw_valid) {
        return;
    }
    /* attempting to draw with zero elements or instances is not technically an
       error, but might be handled as an error in the backend API (e.g. on Metal)
    */
    if ((0 == num_elements) || (0 == num_instances)) {
        return;
    }
    _sg_draw(base_element, num_elements, num_instances);
    _SG_TRACE_ARGS(draw, base_element, num_elements, num_instances);
}

SOKOL_API_IMPL void sg_end_pass(void) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(_sg.cur_pass.in_pass);
    _sg_stats_add(num_passes, 1);
    // NOTE: don't exit early if !_sg.cur_pass.valid
    _sg_end_pass();
    _sg.cur_pipeline.id = SG_INVALID_ID;
    _sg_clear(&_sg.cur_pass, sizeof(_sg.cur_pass));
    _SG_TRACE_NOARGS(end_pass);
}

SOKOL_API_IMPL void sg_commit(void) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(!_sg.cur_pass.valid);
    SOKOL_ASSERT(!_sg.cur_pass.in_pass);
    _sg_commit();
    _sg.stats.frame_index = _sg.frame_index;
    _sg.prev_stats = _sg.stats;
    _sg_clear(&_sg.stats, sizeof(_sg.stats));
    _sg_notify_commit_listeners();
    _SG_TRACE_NOARGS(commit);
    _sg.frame_index++;
}

SOKOL_API_IMPL void sg_reset_state_cache(void) {
    SOKOL_ASSERT(_sg.valid);
    _sg_reset_state_cache();
    _SG_TRACE_NOARGS(reset_state_cache);
}

SOKOL_API_IMPL void sg_update_buffer(sg_buffer buf_id, const sg_range* data) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(data && data->ptr && (data->size > 0));
    _sg_stats_add(num_update_buffer, 1);
    _sg_stats_add(size_update_buffer, (uint32_t)data->size);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if ((data->size > 0) && buf && (buf->slot.state == SG_RESOURCESTATE_VALID)) {
        if (_sg_validate_update_buffer(buf, data)) {
            SOKOL_ASSERT(data->size <= (size_t)buf->cmn.size);
            // only one update allowed per buffer and frame
            SOKOL_ASSERT(buf->cmn.update_frame_index != _sg.frame_index);
            // update and append on same buffer in same frame not allowed
            SOKOL_ASSERT(buf->cmn.append_frame_index != _sg.frame_index);
            _sg_update_buffer(buf, data);
            buf->cmn.update_frame_index = _sg.frame_index;
        }
    }
    _SG_TRACE_ARGS(update_buffer, buf_id, data);
}

SOKOL_API_IMPL int sg_append_buffer(sg_buffer buf_id, const sg_range* data) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(data && data->ptr);
    _sg_stats_add(num_append_buffer, 1);
    _sg_stats_add(size_append_buffer, (uint32_t)data->size);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    int result;
    if (buf) {
        // rewind append cursor in a new frame
        if (buf->cmn.append_frame_index != _sg.frame_index) {
            buf->cmn.append_pos = 0;
            buf->cmn.append_overflow = false;
        }
        if (((size_t)buf->cmn.append_pos + data->size) > (size_t)buf->cmn.size) {
            buf->cmn.append_overflow = true;
        }
        const int start_pos = buf->cmn.append_pos;
        // NOTE: the multiple-of-4 requirement for the buffer offset is coming
        // from WebGPU, but we want identical behaviour between backends
        SOKOL_ASSERT(_sg_multiple_u64((uint64_t)start_pos, 4));
        if (buf->slot.state == SG_RESOURCESTATE_VALID) {
            if (_sg_validate_append_buffer(buf, data)) {
                if (!buf->cmn.append_overflow && (data->size > 0)) {
                    // update and append on same buffer in same frame not allowed
                    SOKOL_ASSERT(buf->cmn.update_frame_index != _sg.frame_index);
                    _sg_append_buffer(buf, data, buf->cmn.append_frame_index != _sg.frame_index);
                    buf->cmn.append_pos += (int) _sg_roundup_u64(data->size, 4);
                    buf->cmn.append_frame_index = _sg.frame_index;
                }
            }
        }
        result = start_pos;
    } else {
        // FIXME: should we return -1 here?
        result = 0;
    }
    _SG_TRACE_ARGS(append_buffer, buf_id, data, result);
    return result;
}

SOKOL_API_IMPL bool sg_query_buffer_overflow(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    bool result = buf ? buf->cmn.append_overflow : false;
    return result;
}

SOKOL_API_IMPL bool sg_query_buffer_will_overflow(sg_buffer buf_id, size_t size) {
    SOKOL_ASSERT(_sg.valid);
    _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    bool result = false;
    if (buf) {
        int append_pos = buf->cmn.append_pos;
        // rewind append cursor in a new frame
        if (buf->cmn.append_frame_index != _sg.frame_index) {
            append_pos = 0;
        }
        if ((append_pos + _sg_roundup((int)size, 4)) > buf->cmn.size) {
            result = true;
        }
    }
    return result;
}

SOKOL_API_IMPL void sg_update_image(sg_image img_id, const sg_image_data* data) {
    SOKOL_ASSERT(_sg.valid);
    _sg_stats_add(num_update_image, 1);
    for (int face_index = 0; face_index < SG_CUBEFACE_NUM; face_index++) {
        for (int mip_index = 0; mip_index < SG_MAX_MIPMAPS; mip_index++) {
            if (data->subimage[face_index][mip_index].size == 0) {
                break;
            }
            _sg_stats_add(size_update_image, (uint32_t)data->subimage[face_index][mip_index].size);
        }
    }
    _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img && img->slot.state == SG_RESOURCESTATE_VALID) {
        if (_sg_validate_update_image(img, data)) {
            SOKOL_ASSERT(img->cmn.upd_frame_index != _sg.frame_index);
            _sg_update_image(img, data);
            img->cmn.upd_frame_index = _sg.frame_index;
        }
    }
    _SG_TRACE_ARGS(update_image, img_id, data);
}

SOKOL_API_IMPL void sg_push_debug_group(const char* name) {
    SOKOL_ASSERT(_sg.valid);
    SOKOL_ASSERT(name);
    _sg_push_debug_group(name);
    _SG_TRACE_ARGS(push_debug_group, name);
}

SOKOL_API_IMPL void sg_pop_debug_group(void) {
    SOKOL_ASSERT(_sg.valid);
    _sg_pop_debug_group();
    _SG_TRACE_NOARGS(pop_debug_group);
}

SOKOL_API_IMPL bool sg_add_commit_listener(sg_commit_listener listener) {
    SOKOL_ASSERT(_sg.valid);
    return _sg_add_commit_listener(&listener);
}

SOKOL_API_IMPL bool sg_remove_commit_listener(sg_commit_listener listener) {
    SOKOL_ASSERT(_sg.valid);
    return _sg_remove_commit_listener(&listener);
}

SOKOL_API_IMPL void sg_enable_frame_stats(void) {
    SOKOL_ASSERT(_sg.valid);
    _sg.stats_enabled = true;
}

SOKOL_API_IMPL void sg_disable_frame_stats(void) {
    SOKOL_ASSERT(_sg.valid);
    _sg.stats_enabled = false;
}

SOKOL_API_IMPL bool sg_frame_stats_enabled(void) {
    return _sg.stats_enabled;
}

SOKOL_API_IMPL sg_buffer_info sg_query_buffer_info(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_buffer_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        info.slot.state = buf->slot.state;
        info.slot.res_id = buf->slot.id;
        info.update_frame_index = buf->cmn.update_frame_index;
        info.append_frame_index = buf->cmn.append_frame_index;
        info.append_pos = buf->cmn.append_pos;
        info.append_overflow = buf->cmn.append_overflow;
        #if defined(SOKOL_D3D11)
        info.num_slots = 1;
        info.active_slot = 0;
        #else
        info.num_slots = buf->cmn.num_slots;
        info.active_slot = buf->cmn.active_slot;
        #endif
    }
    return info;
}

SOKOL_API_IMPL sg_image_info sg_query_image_info(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_image_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        info.slot.state = img->slot.state;
        info.slot.res_id = img->slot.id;
        info.upd_frame_index = img->cmn.upd_frame_index;
        #if defined(SOKOL_D3D11)
        info.num_slots = 1;
        info.active_slot = 0;
        #else
        info.num_slots = img->cmn.num_slots;
        info.active_slot = img->cmn.active_slot;
        #endif
    }
    return info;
}

SOKOL_API_IMPL sg_sampler_info sg_query_sampler_info(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_sampler_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        info.slot.state = smp->slot.state;
        info.slot.res_id = smp->slot.id;
    }
    return info;
}

SOKOL_API_IMPL sg_shader_info sg_query_shader_info(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_shader_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        info.slot.state = shd->slot.state;
        info.slot.res_id = shd->slot.id;
    }
    return info;
}

SOKOL_API_IMPL sg_pipeline_info sg_query_pipeline_info(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_pipeline_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        info.slot.state = pip->slot.state;
        info.slot.res_id = pip->slot.id;
    }
    return info;
}

SOKOL_API_IMPL sg_attachments_info sg_query_attachments_info(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_attachments_info info;
    _sg_clear(&info, sizeof(info));
    const _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        info.slot.state = atts->slot.state;
        info.slot.res_id = atts->slot.id;
    }
    return info;
}

SOKOL_API_IMPL sg_buffer_desc sg_query_buffer_desc(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_buffer_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
    if (buf) {
        desc.size = (size_t)buf->cmn.size;
        desc.type = buf->cmn.type;
        desc.usage = buf->cmn.usage;
    }
    return desc;
}

SOKOL_API_IMPL sg_image_desc sg_query_image_desc(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_image_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
    if (img) {
        desc.type = img->cmn.type;
        desc.render_target = img->cmn.render_target;
        desc.width = img->cmn.width;
        desc.height = img->cmn.height;
        desc.num_slices = img->cmn.num_slices;
        desc.num_mipmaps = img->cmn.num_mipmaps;
        desc.usage = img->cmn.usage;
        desc.pixel_format = img->cmn.pixel_format;
        desc.sample_count = img->cmn.sample_count;
    }
    return desc;
}

SOKOL_API_IMPL sg_sampler_desc sg_query_sampler_desc(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_sampler_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
    if (smp) {
        desc.min_filter = smp->cmn.min_filter;
        desc.mag_filter = smp->cmn.mag_filter;
        desc.mipmap_filter = smp->cmn.mipmap_filter;
        desc.wrap_u = smp->cmn.wrap_u;
        desc.wrap_v = smp->cmn.wrap_v;
        desc.wrap_w = smp->cmn.wrap_w;
        desc.min_lod = smp->cmn.min_lod;
        desc.max_lod = smp->cmn.max_lod;
        desc.border_color = smp->cmn.border_color;
        desc.compare = smp->cmn.compare;
        desc.max_anisotropy = smp->cmn.max_anisotropy;
    }
    return desc;
}

SOKOL_API_IMPL sg_shader_desc sg_query_shader_desc(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_shader_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
    if (shd) {
        for (int stage_idx = 0; stage_idx < SG_NUM_SHADER_STAGES; stage_idx++) {
            sg_shader_stage_desc* stage_desc = (stage_idx == 0) ? &desc.vs : &desc.fs;
            const _sg_shader_stage_t* stage = &shd->cmn.stage[stage_idx];
            for (int ub_idx = 0; ub_idx < stage->num_uniform_blocks; ub_idx++) {
                sg_shader_uniform_block_desc* ub_desc = &stage_desc->uniform_blocks[ub_idx];
                const _sg_shader_uniform_block_t* ub = &stage->uniform_blocks[ub_idx];
                ub_desc->size = ub->size;
            }
            for (int img_idx = 0; img_idx < stage->num_images; img_idx++) {
                sg_shader_image_desc* img_desc = &stage_desc->images[img_idx];
                const _sg_shader_image_t* img = &stage->images[img_idx];
                img_desc->used = true;
                img_desc->image_type = img->image_type;
                img_desc->sample_type = img->sample_type;
                img_desc->multisampled = img->multisampled;
            }
            for (int smp_idx = 0; smp_idx < stage->num_samplers; smp_idx++) {
                sg_shader_sampler_desc* smp_desc = &stage_desc->samplers[smp_idx];
                const _sg_shader_sampler_t* smp = &stage->samplers[smp_idx];
                smp_desc->used = true;
                smp_desc->sampler_type = smp->sampler_type;
            }
            for (int img_smp_idx = 0; img_smp_idx < stage->num_image_samplers; img_smp_idx++) {
                sg_shader_image_sampler_pair_desc* img_smp_desc = &stage_desc->image_sampler_pairs[img_smp_idx];
                const _sg_shader_image_sampler_t* img_smp = &stage->image_samplers[img_smp_idx];
                img_smp_desc->used = true;
                img_smp_desc->image_slot = img_smp->image_slot;
                img_smp_desc->sampler_slot = img_smp->sampler_slot;
                img_smp_desc->glsl_name = 0;
            }
        }
    }
    return desc;
}

SOKOL_API_IMPL sg_pipeline_desc sg_query_pipeline_desc(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_pipeline_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
    if (pip) {
        desc.shader = pip->cmn.shader_id;
        desc.layout = pip->cmn.layout;
        desc.depth = pip->cmn.depth;
        desc.stencil = pip->cmn.stencil;
        desc.color_count = pip->cmn.color_count;
        for (int i = 0; i < pip->cmn.color_count; i++) {
            desc.colors[i] = pip->cmn.colors[i];
        }
        desc.primitive_type = pip->cmn.primitive_type;
        desc.index_type = pip->cmn.index_type;
        desc.cull_mode = pip->cmn.cull_mode;
        desc.face_winding = pip->cmn.face_winding;
        desc.sample_count = pip->cmn.sample_count;
        desc.blend_color = pip->cmn.blend_color;
        desc.alpha_to_coverage_enabled = pip->cmn.alpha_to_coverage_enabled;
    }
    return desc;
}

SOKOL_API_IMPL sg_attachments_desc sg_query_attachments_desc(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_attachments_desc desc;
    _sg_clear(&desc, sizeof(desc));
    const _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
    if (atts) {
        for (int i = 0; i < atts->cmn.num_colors; i++) {
            desc.colors[i].image = atts->cmn.colors[i].image_id;
            desc.colors[i].mip_level = atts->cmn.colors[i].mip_level;
            desc.colors[i].slice = atts->cmn.colors[i].slice;
        }
        desc.depth_stencil.image = atts->cmn.depth_stencil.image_id;
        desc.depth_stencil.mip_level = atts->cmn.depth_stencil.mip_level;
        desc.depth_stencil.slice = atts->cmn.depth_stencil.slice;
    }
    return desc;
}

SOKOL_API_IMPL sg_buffer_desc sg_query_buffer_defaults(const sg_buffer_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_buffer_desc_defaults(desc);
}

SOKOL_API_IMPL sg_image_desc sg_query_image_defaults(const sg_image_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_image_desc_defaults(desc);
}

SOKOL_API_IMPL sg_sampler_desc sg_query_sampler_defaults(const sg_sampler_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_sampler_desc_defaults(desc);
}

SOKOL_API_IMPL sg_shader_desc sg_query_shader_defaults(const sg_shader_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_shader_desc_defaults(desc);
}

SOKOL_API_IMPL sg_pipeline_desc sg_query_pipeline_defaults(const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_pipeline_desc_defaults(desc);
}

SOKOL_API_IMPL sg_attachments_desc sg_query_attachments_defaults(const sg_attachments_desc* desc) {
    SOKOL_ASSERT(_sg.valid && desc);
    return _sg_attachments_desc_defaults(desc);
}

SOKOL_API_IMPL const void* sg_d3d11_device(void) {
    #if defined(SOKOL_D3D11)
        return (const void*) _sg.d3d11.dev;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sg_d3d11_device_context(void) {
    #if defined(SOKOL_D3D11)
        return (const void*) _sg.d3d11.ctx;
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL sg_d3d11_buffer_info sg_d3d11_query_buffer_info(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_buffer_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
        if (buf) {
            res.buf = (const void*) buf->d3d11.buf;
        }
    #else
        _SOKOL_UNUSED(buf_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_d3d11_image_info sg_d3d11_query_image_info(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_image_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
        if (img) {
            res.tex2d = (const void*) img->d3d11.tex2d;
            res.tex3d = (const void*) img->d3d11.tex3d;
            res.res = (const void*) img->d3d11.res;
            res.srv = (const void*) img->d3d11.srv;
        }
    #else
        _SOKOL_UNUSED(img_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_d3d11_sampler_info sg_d3d11_query_sampler_info(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_sampler_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
        if (smp) {
            res.smp = (const void*) smp->d3d11.smp;
        }
    #else
        _SOKOL_UNUSED(smp_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_d3d11_shader_info sg_d3d11_query_shader_info(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_shader_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
        if (shd) {
            for (int i = 0; i < SG_MAX_SHADERSTAGE_UBS; i++) {
                res.vs_cbufs[i] = (const void*) shd->d3d11.stage[SG_SHADERSTAGE_VS].cbufs[i];
                res.fs_cbufs[i] = (const void*) shd->d3d11.stage[SG_SHADERSTAGE_FS].cbufs[i];
            }
            res.vs = (const void*) shd->d3d11.vs;
            res.fs = (const void*) shd->d3d11.fs;
        }
    #else
        _SOKOL_UNUSED(shd_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_d3d11_pipeline_info sg_d3d11_query_pipeline_info(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_pipeline_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
        if (pip) {
            res.il = (const void*) pip->d3d11.il;
            res.rs = (const void*) pip->d3d11.rs;
            res.dss = (const void*) pip->d3d11.dss;
            res.bs = (const void*) pip->d3d11.bs;
        }
    #else
        _SOKOL_UNUSED(pip_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_d3d11_attachments_info sg_d3d11_query_attachments_info(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_d3d11_attachments_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_D3D11)
        const _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
        if (atts) {
            for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
                res.color_rtv[i] = (const void*) atts->d3d11.colors[i].view.rtv;
                res.resolve_rtv[i] = (const void*) atts->d3d11.resolves[i].view.rtv;
            }
            res.dsv = (const void*) atts->d3d11.depth_stencil.view.dsv;
        }
    #else
        _SOKOL_UNUSED(atts_id);
    #endif
    return res;
}

SOKOL_API_IMPL const void* sg_mtl_device(void) {
    #if defined(SOKOL_METAL)
        if (nil != _sg.mtl.device) {
            return (__bridge const void*) _sg.mtl.device;
        } else {
            return 0;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL const void* sg_mtl_render_command_encoder(void) {
    #if defined(SOKOL_METAL)
        if (nil != _sg.mtl.cmd_encoder) {
            return (__bridge const void*) _sg.mtl.cmd_encoder;
        } else {
            return 0;
        }
    #else
        return 0;
    #endif
}

SOKOL_API_IMPL sg_mtl_buffer_info sg_mtl_query_buffer_info(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_mtl_buffer_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_METAL)
        const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
        if (buf) {
            for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
                if (buf->mtl.buf[i] != 0) {
                    res.buf[i] = (__bridge void*) _sg_mtl_id(buf->mtl.buf[i]);
                }
            }
            res.active_slot = buf->cmn.active_slot;
        }
    #else
        _SOKOL_UNUSED(buf_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_mtl_image_info sg_mtl_query_image_info(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_mtl_image_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_METAL)
        const _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
        if (img) {
            for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
                if (img->mtl.tex[i] != 0) {
                    res.tex[i] = (__bridge void*) _sg_mtl_id(img->mtl.tex[i]);
                }
            }
            res.active_slot = img->cmn.active_slot;
        }
    #else
        _SOKOL_UNUSED(img_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_mtl_sampler_info sg_mtl_query_sampler_info(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_mtl_sampler_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_METAL)
        const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
        if (smp) {
            if (smp->mtl.sampler_state != 0) {
                res.smp = (__bridge void*) _sg_mtl_id(smp->mtl.sampler_state);
            }
        }
    #else
        _SOKOL_UNUSED(smp_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_mtl_shader_info sg_mtl_query_shader_info(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_mtl_shader_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_METAL)
        const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
        if (shd) {
            const int vs_lib  = shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_lib;
            const int vs_func = shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_func;
            const int fs_lib  = shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_lib;
            const int fs_func = shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_func;
            if (vs_lib != 0) {
                res.vs_lib = (__bridge void*) _sg_mtl_id(vs_lib);
            }
            if (fs_lib != 0) {
                res.fs_lib = (__bridge void*) _sg_mtl_id(fs_lib);
            }
            if (vs_func != 0) {
                res.vs_func = (__bridge void*) _sg_mtl_id(vs_func);
            }
            if (fs_func != 0) {
                res.fs_func = (__bridge void*) _sg_mtl_id(fs_func);
            }
        }
    #else
        _SOKOL_UNUSED(shd_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_mtl_pipeline_info sg_mtl_query_pipeline_info(sg_pipeline pip_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_mtl_pipeline_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(SOKOL_METAL)
        const _sg_pipeline_t* pip = _sg_lookup_pipeline(&_sg.pools, pip_id.id);
        if (pip) {
            if (pip->mtl.rps != 0) {
                res.rps = (__bridge void*) _sg_mtl_id(pip->mtl.rps);
            }
            if (pip->mtl.dss != 0) {
                res.dss = (__bridge void*) _sg_mtl_id(pip->mtl.dss);
            }
        }
    #else
        _SOKOL_UNUSED(pip_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_gl_buffer_info sg_gl_query_buffer_info(sg_buffer buf_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_gl_buffer_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(_SOKOL_ANY_GL)
        const _sg_buffer_t* buf = _sg_lookup_buffer(&_sg.pools, buf_id.id);
        if (buf) {
            for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
                res.buf[i] = buf->gl.buf[i];
            }
            res.active_slot = buf->cmn.active_slot;
        }
    #else
        _SOKOL_UNUSED(buf_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_gl_image_info sg_gl_query_image_info(sg_image img_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_gl_image_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(_SOKOL_ANY_GL)
        const _sg_image_t* img = _sg_lookup_image(&_sg.pools, img_id.id);
        if (img) {
            for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
                res.tex[i] = img->gl.tex[i];
            }
            res.tex_target = img->gl.target;
            res.msaa_render_buffer = img->gl.msaa_render_buffer;
            res.active_slot = img->cmn.active_slot;
        }
    #else
        _SOKOL_UNUSED(img_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_gl_sampler_info sg_gl_query_sampler_info(sg_sampler smp_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_gl_sampler_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(_SOKOL_ANY_GL)
        const _sg_sampler_t* smp = _sg_lookup_sampler(&_sg.pools, smp_id.id);
        if (smp) {
            res.smp = smp->gl.smp;
        }
    #else
        _SOKOL_UNUSED(smp_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_gl_shader_info sg_gl_query_shader_info(sg_shader shd_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_gl_shader_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(_SOKOL_ANY_GL)
        const _sg_shader_t* shd = _sg_lookup_shader(&_sg.pools, shd_id.id);
        if (shd) {
            res.prog = shd->gl.prog;
        }
    #else
        _SOKOL_UNUSED(shd_id);
    #endif
    return res;
}

SOKOL_API_IMPL sg_gl_attachments_info sg_gl_query_attachments_info(sg_attachments atts_id) {
    SOKOL_ASSERT(_sg.valid);
    sg_gl_attachments_info res;
    _sg_clear(&res, sizeof(res));
    #if defined(_SOKOL_ANY_GL)
        const _sg_attachments_t* atts = _sg_lookup_attachments(&_sg.pools, atts_id.id);
        if (atts) {
            res.framebuffer = atts->gl.fb;
            for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
                res.msaa_resolve_framebuffer[i] = atts->gl.msaa_resolve_framebuffer[i];
            }
        }
    #else
        _SOKOL_UNUSED(atts_id);
    #endif
    return res;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
