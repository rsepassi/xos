#ifndef SOKOL_GFX_INCLUDED
#define SOKOL_GFX_INCLUDED (1)
#include <stddef.h>     // size_t
#include <stdint.h>
#include <stdbool.h>

#define SOKOL_GFX_API_DECL extern

/*
    Resource id typedefs:

    sg_buffer:      vertex- and index-buffers
    sg_image:       images used as textures and render targets
    sg_sampler      sampler object describing how a texture is sampled in a shader
    sg_shader:      vertex- and fragment-shaders and shader interface information
    sg_pipeline:    associated shader and vertex-layouts, and render states
    sg_attachments: a baked collection of render pass attachment images

    Instead of pointers, resource creation functions return a 32-bit
    number which uniquely identifies the resource object.

    The 32-bit resource id is split into a 16-bit pool index in the lower bits,
    and a 16-bit 'generation counter' in the upper bits. The index allows fast
    pool lookups, and combined with the generation-counter it allows to detect
    'dangling accesses' (trying to use an object which no longer exists, and
    its pool slot has been reused for a new object)

    The resource ids are wrapped into a strongly-typed struct so that
    trying to pass an incompatible resource id is a compile error.
*/
typedef struct sg_buffer        { uint32_t id; } sg_buffer;
typedef struct sg_image         { uint32_t id; } sg_image;
typedef struct sg_sampler       { uint32_t id; } sg_sampler;
typedef struct sg_shader        { uint32_t id; } sg_shader;
typedef struct sg_pipeline      { uint32_t id; } sg_pipeline;
typedef struct sg_attachments   { uint32_t id; } sg_attachments;

/*
    sg_range is a pointer-size-pair struct used to pass memory blobs into
    sokol-gfx. When initialized from a value type (array or struct), you can
    use the SG_RANGE() macro to build an sg_range struct. For functions which
    take either a sg_range pointer, or a (C++) sg_range reference, use the
    SG_RANGE_REF macro as a solution which compiles both in C and C++.
*/
typedef struct sg_range {
    const void* ptr;
    size_t size;
} sg_range;

// disabling this for every includer isn't great, but the warnings are also quite pointless
#if defined(_MSC_VER)
#pragma warning(disable:4221)   // /W4 only: nonstandard extension used: 'x': cannot be initialized using address of automatic variable 'y'
#pragma warning(disable:4204)   // VS2015: nonstandard extension used: non-constant aggregate initializer
#endif
#if defined(__cplusplus)
#define SG_RANGE(x) sg_range{ &x, sizeof(x) }
#define SG_RANGE_REF(x) sg_range{ &x, sizeof(x) }
#else
#define SG_RANGE(x) (sg_range){ &x, sizeof(x) }
#define SG_RANGE_REF(x) &(sg_range){ &x, sizeof(x) }
#endif

//  various compile-time constants
enum {
    SG_INVALID_ID = 0,
    SG_NUM_SHADER_STAGES = 2,
    SG_NUM_INFLIGHT_FRAMES = 2,
    SG_MAX_COLOR_ATTACHMENTS = 4,
    SG_MAX_VERTEX_BUFFERS = 8,
    SG_MAX_SHADERSTAGE_IMAGES = 12,
    SG_MAX_SHADERSTAGE_SAMPLERS = 8,
    SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS = 12,
    SG_MAX_SHADERSTAGE_STORAGEBUFFERS = 8,
    SG_MAX_SHADERSTAGE_UBS = 4,
    SG_MAX_UB_MEMBERS = 16,
    SG_MAX_VERTEX_ATTRIBUTES = 16,
    SG_MAX_MIPMAPS = 16,
    SG_MAX_TEXTUREARRAY_LAYERS = 128
};

/*
    sg_color

    An RGBA color value.
*/
typedef struct sg_color { float r, g, b, a; } sg_color;

/*
    sg_backend

    The active 3D-API backend, use the function sg_query_backend()
    to get the currently active backend.
*/
typedef enum sg_backend {
    SG_BACKEND_GLCORE,
    SG_BACKEND_GLES3,
    SG_BACKEND_D3D11,
    SG_BACKEND_METAL_IOS,
    SG_BACKEND_METAL_MACOS,
    SG_BACKEND_METAL_SIMULATOR,
    SG_BACKEND_WGPU,
    SG_BACKEND_DUMMY,
} sg_backend;

/*
    sg_pixel_format

    sokol_gfx.h basically uses the same pixel formats as WebGPU, since these
    are supported on most newer GPUs.

    A pixelformat name consist of three parts:

        - components (R, RG, RGB or RGBA)
        - bit width per component (8, 16 or 32)
        - component data type:
            - unsigned normalized (no postfix)
            - signed normalized (SN postfix)
            - unsigned integer (UI postfix)
            - signed integer (SI postfix)
            - float (F postfix)

    Not all pixel formats can be used for everything, call sg_query_pixelformat()
    to inspect the capabilities of a given pixelformat. The function returns
    an sg_pixelformat_info struct with the following members:

        - sample: the pixelformat can be sampled as texture at least with
                  nearest filtering
        - filter: the pixelformat can be samples as texture with linear
                  filtering
        - render: the pixelformat can be used for render targets
        - blend:  blending is supported when using the pixelformat for
                  render targets
        - msaa:   multisample-antialiasing is supported when using the
                  pixelformat for render targets
        - depth:  the pixelformat can be used for depth-stencil attachments
        - compressed: this is a block-compressed format
        - bytes_per_pixel: the numbers of bytes in a pixel (0 for compressed formats)

    The default pixel format for texture images is SG_PIXELFORMAT_RGBA8.

    The default pixel format for render target images is platform-dependent
    and taken from the sg_environment struct passed into sg_setup(). Typically
    the default formats are:

        - for the Metal, D3D11 and WebGPU backends: SG_PIXELFORMAT_BGRA8
        - for GL backends: SG_PIXELFORMAT_RGBA8
*/
typedef enum sg_pixel_format {
    _SG_PIXELFORMAT_DEFAULT,    // value 0 reserved for default-init
    SG_PIXELFORMAT_NONE,

    SG_PIXELFORMAT_R8,
    SG_PIXELFORMAT_R8SN,
    SG_PIXELFORMAT_R8UI,
    SG_PIXELFORMAT_R8SI,

    SG_PIXELFORMAT_R16,
    SG_PIXELFORMAT_R16SN,
    SG_PIXELFORMAT_R16UI,
    SG_PIXELFORMAT_R16SI,
    SG_PIXELFORMAT_R16F,
    SG_PIXELFORMAT_RG8,
    SG_PIXELFORMAT_RG8SN,
    SG_PIXELFORMAT_RG8UI,
    SG_PIXELFORMAT_RG8SI,

    SG_PIXELFORMAT_R32UI,
    SG_PIXELFORMAT_R32SI,
    SG_PIXELFORMAT_R32F,
    SG_PIXELFORMAT_RG16,
    SG_PIXELFORMAT_RG16SN,
    SG_PIXELFORMAT_RG16UI,
    SG_PIXELFORMAT_RG16SI,
    SG_PIXELFORMAT_RG16F,
    SG_PIXELFORMAT_RGBA8,
    SG_PIXELFORMAT_SRGB8A8,
    SG_PIXELFORMAT_RGBA8SN,
    SG_PIXELFORMAT_RGBA8UI,
    SG_PIXELFORMAT_RGBA8SI,
    SG_PIXELFORMAT_BGRA8,
    SG_PIXELFORMAT_RGB10A2,
    SG_PIXELFORMAT_RG11B10F,
    SG_PIXELFORMAT_RGB9E5,

    SG_PIXELFORMAT_RG32UI,
    SG_PIXELFORMAT_RG32SI,
    SG_PIXELFORMAT_RG32F,
    SG_PIXELFORMAT_RGBA16,
    SG_PIXELFORMAT_RGBA16SN,
    SG_PIXELFORMAT_RGBA16UI,
    SG_PIXELFORMAT_RGBA16SI,
    SG_PIXELFORMAT_RGBA16F,

    SG_PIXELFORMAT_RGBA32UI,
    SG_PIXELFORMAT_RGBA32SI,
    SG_PIXELFORMAT_RGBA32F,

    // NOTE: when adding/removing pixel formats before DEPTH, also update sokol_app.h/_SAPP_PIXELFORMAT_*
    SG_PIXELFORMAT_DEPTH,
    SG_PIXELFORMAT_DEPTH_STENCIL,

    // NOTE: don't put any new compressed format in front of here
    SG_PIXELFORMAT_BC1_RGBA,
    SG_PIXELFORMAT_BC2_RGBA,
    SG_PIXELFORMAT_BC3_RGBA,
    SG_PIXELFORMAT_BC3_SRGBA,
    SG_PIXELFORMAT_BC4_R,
    SG_PIXELFORMAT_BC4_RSN,
    SG_PIXELFORMAT_BC5_RG,
    SG_PIXELFORMAT_BC5_RGSN,
    SG_PIXELFORMAT_BC6H_RGBF,
    SG_PIXELFORMAT_BC6H_RGBUF,
    SG_PIXELFORMAT_BC7_RGBA,
    SG_PIXELFORMAT_BC7_SRGBA,
    SG_PIXELFORMAT_PVRTC_RGB_2BPP,      // FIXME: deprecated
    SG_PIXELFORMAT_PVRTC_RGB_4BPP,      // FIXME: deprecated
    SG_PIXELFORMAT_PVRTC_RGBA_2BPP,     // FIXME: deprecated
    SG_PIXELFORMAT_PVRTC_RGBA_4BPP,     // FIXME: deprecated
    SG_PIXELFORMAT_ETC2_RGB8,
    SG_PIXELFORMAT_ETC2_SRGB8,
    SG_PIXELFORMAT_ETC2_RGB8A1,
    SG_PIXELFORMAT_ETC2_RGBA8,
    SG_PIXELFORMAT_ETC2_SRGB8A8,
    SG_PIXELFORMAT_EAC_R11,
    SG_PIXELFORMAT_EAC_R11SN,
    SG_PIXELFORMAT_EAC_RG11,
    SG_PIXELFORMAT_EAC_RG11SN,

    SG_PIXELFORMAT_ASTC_4x4_RGBA,
    SG_PIXELFORMAT_ASTC_4x4_SRGBA,

    _SG_PIXELFORMAT_NUM,
    _SG_PIXELFORMAT_FORCE_U32 = 0x7FFFFFFF
} sg_pixel_format;

/*
    Runtime information about a pixel format, returned
    by sg_query_pixelformat().
*/
typedef struct sg_pixelformat_info {
    bool sample;            // pixel format can be sampled in shaders at least with nearest filtering
    bool filter;            // pixel format can be sampled with linear filtering
    bool render;            // pixel format can be used as render target
    bool blend;             // alpha-blending is supported
    bool msaa;              // pixel format can be used as MSAA render target
    bool depth;             // pixel format is a depth format
    bool compressed;        // true if this is a hardware-compressed format
    int bytes_per_pixel;    // NOTE: this is 0 for compressed formats, use sg_query_row_pitch() / sg_query_surface_pitch() as alternative
} sg_pixelformat_info;

/*
    Runtime information about available optional features,
    returned by sg_query_features()
*/
typedef struct sg_features {
    bool origin_top_left;               // framebuffer and texture origin is in top left corner
    bool image_clamp_to_border;         // border color and clamp-to-border UV-wrap mode is supported
    bool mrt_independent_blend_state;   // multiple-render-target rendering can use per-render-target blend state
    bool mrt_independent_write_mask;    // multiple-render-target rendering can use per-render-target color write masks
    bool storage_buffer;                // storage buffers are supported
} sg_features;

/*
    Runtime information about resource limits, returned by sg_query_limit()
*/
typedef struct sg_limits {
    int max_image_size_2d;          // max width/height of SG_IMAGETYPE_2D images
    int max_image_size_cube;        // max width/height of SG_IMAGETYPE_CUBE images
    int max_image_size_3d;          // max width/height/depth of SG_IMAGETYPE_3D images
    int max_image_size_array;       // max width/height of SG_IMAGETYPE_ARRAY images
    int max_image_array_layers;     // max number of layers in SG_IMAGETYPE_ARRAY images
    int max_vertex_attrs;           // max number of vertex attributes, clamped to SG_MAX_VERTEX_ATTRIBUTES
    int gl_max_vertex_uniform_components;    // <= GL_MAX_VERTEX_UNIFORM_COMPONENTS (only on GL backends)
    int gl_max_combined_texture_image_units; // <= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS (only on GL backends)
} sg_limits;

/*
    sg_resource_state

    The current state of a resource in its resource pool.
    Resources start in the INITIAL state, which means the
    pool slot is unoccupied and can be allocated. When a resource is
    created, first an id is allocated, and the resource pool slot
    is set to state ALLOC. After allocation, the resource is
    initialized, which may result in the VALID or FAILED state. The
    reason why allocation and initialization are separate is because
    some resource types (e.g. buffers and images) might be asynchronously
    initialized by the user application. If a resource which is not
    in the VALID state is attempted to be used for rendering, rendering
    operations will silently be dropped.

    The special INVALID state is returned in sg_query_xxx_state() if no
    resource object exists for the provided resource id.
*/
typedef enum sg_resource_state {
    SG_RESOURCESTATE_INITIAL,
    SG_RESOURCESTATE_ALLOC,
    SG_RESOURCESTATE_VALID,
    SG_RESOURCESTATE_FAILED,
    SG_RESOURCESTATE_INVALID,
    _SG_RESOURCESTATE_FORCE_U32 = 0x7FFFFFFF
} sg_resource_state;

/*
    sg_usage

    A resource usage hint describing the update strategy of
    buffers and images. This is used in the sg_buffer_desc.usage
    and sg_image_desc.usage members when creating buffers
    and images:

    SG_USAGE_IMMUTABLE:     the resource will never be updated with
                            new data, instead the content of the
                            resource must be provided on creation
    SG_USAGE_DYNAMIC:       the resource will be updated infrequently
                            with new data (this could range from "once
                            after creation", to "quite often but not
                            every frame")
    SG_USAGE_STREAM:        the resource will be updated each frame
                            with new content

    The rendering backends use this hint to prevent that the
    CPU needs to wait for the GPU when attempting to update
    a resource that might be currently accessed by the GPU.

    Resource content is updated with the functions sg_update_buffer() or
    sg_append_buffer() for buffer objects, and sg_update_image() for image
    objects. For the sg_update_*() functions, only one update is allowed per
    frame and resource object, while sg_append_buffer() can be called
    multiple times per frame on the same buffer. The application must update
    all data required for rendering (this means that the update data can be
    smaller than the resource size, if only a part of the overall resource
    size is used for rendering, you only need to make sure that the data that
    *is* used is valid).

    The default usage is SG_USAGE_IMMUTABLE.
*/
typedef enum sg_usage {
    _SG_USAGE_DEFAULT,      // value 0 reserved for default-init
    SG_USAGE_IMMUTABLE,
    SG_USAGE_DYNAMIC,
    SG_USAGE_STREAM,
    _SG_USAGE_NUM,
    _SG_USAGE_FORCE_U32 = 0x7FFFFFFF
} sg_usage;

/*
    sg_buffer_type

    Indicates whether a buffer will be bound as vertex-,
    index- or storage-buffer.

    Used in the sg_buffer_desc.type member when creating a buffer.

    The default value is SG_BUFFERTYPE_VERTEXBUFFER.
*/
typedef enum sg_buffer_type {
    _SG_BUFFERTYPE_DEFAULT,         // value 0 reserved for default-init
    SG_BUFFERTYPE_VERTEXBUFFER,
    SG_BUFFERTYPE_INDEXBUFFER,
    SG_BUFFERTYPE_STORAGEBUFFER,
    _SG_BUFFERTYPE_NUM,
    _SG_BUFFERTYPE_FORCE_U32 = 0x7FFFFFFF
} sg_buffer_type;

/*
    sg_index_type

    Indicates whether indexed rendering (fetching vertex-indices from an
    index buffer) is used, and if yes, the index data type (16- or 32-bits).
    This is used in the sg_pipeline_desc.index_type member when creating a
    pipeline object.

    The default index type is SG_INDEXTYPE_NONE.
*/
typedef enum sg_index_type {
    _SG_INDEXTYPE_DEFAULT,   // value 0 reserved for default-init
    SG_INDEXTYPE_NONE,
    SG_INDEXTYPE_UINT16,
    SG_INDEXTYPE_UINT32,
    _SG_INDEXTYPE_NUM,
    _SG_INDEXTYPE_FORCE_U32 = 0x7FFFFFFF
} sg_index_type;

/*
    sg_image_type

    Indicates the basic type of an image object (2D-texture, cubemap,
    3D-texture or 2D-array-texture). Used in the sg_image_desc.type member when
    creating an image, and in sg_shader_image_desc to describe a sampled texture
    in the shader (both must match and will be checked in the validation layer
    when calling sg_apply_bindings).

    The default image type when creating an image is SG_IMAGETYPE_2D.
*/
typedef enum sg_image_type {
    _SG_IMAGETYPE_DEFAULT,  // value 0 reserved for default-init
    SG_IMAGETYPE_2D,
    SG_IMAGETYPE_CUBE,
    SG_IMAGETYPE_3D,
    SG_IMAGETYPE_ARRAY,
    _SG_IMAGETYPE_NUM,
    _SG_IMAGETYPE_FORCE_U32 = 0x7FFFFFFF
} sg_image_type;

/*
    sg_image_sample_type

    The basic data type of a texture sample as expected by a shader.
    Must be provided in sg_shader_image_desc and used by the validation
    layer in sg_apply_bindings() to check if the provided image object
    is compatible with what the shader expects. Apart from the sokol-gfx
    validation layer, WebGPU is the only backend API which actually requires
    matching texture and sampler type to be provided upfront for validation
    (other 3D APIs treat texture/sampler type mismatches as undefined behaviour).

    NOTE that the following texture pixel formats require the use
    of SG_IMAGESAMPLETYPE_UNFILTERABLE_FLOAT, combined with a sampler
    of type SG_SAMPLERTYPE_NONFILTERING:

    - SG_PIXELFORMAT_R32F
    - SG_PIXELFORMAT_RG32F
    - SG_PIXELFORMAT_RGBA32F

    (when using sokol-shdc, also check out the meta tags `@image_sample_type`
    and `@sampler_type`)
*/
typedef enum sg_image_sample_type {
    _SG_IMAGESAMPLETYPE_DEFAULT,  // value 0 reserved for default-init
    SG_IMAGESAMPLETYPE_FLOAT,
    SG_IMAGESAMPLETYPE_DEPTH,
    SG_IMAGESAMPLETYPE_SINT,
    SG_IMAGESAMPLETYPE_UINT,
    SG_IMAGESAMPLETYPE_UNFILTERABLE_FLOAT,
    _SG_IMAGESAMPLETYPE_NUM,
    _SG_IMAGESAMPLETYPE_FORCE_U32 = 0x7FFFFFFF
} sg_image_sample_type;

/*
    sg_sampler_type

    The basic type of a texture sampler (sampling vs comparison) as
    defined in a shader. Must be provided in sg_shader_sampler_desc.

    sg_image_sample_type and sg_sampler_type for a texture/sampler
    pair must be compatible with each other, specifically only
    the following pairs are allowed:

    - SG_IMAGESAMPLETYPE_FLOAT => (SG_SAMPLERTYPE_FILTERING or SG_SAMPLERTYPE_NONFILTERING)
    - SG_IMAGESAMPLETYPE_UNFILTERABLE_FLOAT => SG_SAMPLERTYPE_NONFILTERING
    - SG_IMAGESAMPLETYPE_SINT => SG_SAMPLERTYPE_NONFILTERING
    - SG_IMAGESAMPLETYPE_UINT => SG_SAMPLERTYPE_NONFILTERING
    - SG_IMAGESAMPLETYPE_DEPTH => SG_SAMPLERTYPE_COMPARISON
*/
typedef enum sg_sampler_type {
    _SG_SAMPLERTYPE_DEFAULT,
    SG_SAMPLERTYPE_FILTERING,
    SG_SAMPLERTYPE_NONFILTERING,
    SG_SAMPLERTYPE_COMPARISON,
    _SG_SAMPLERTYPE_NUM,
    _SG_SAMPLERTYPE_FORCE_U32,
} sg_sampler_type;

/*
    sg_cube_face

    The cubemap faces. Use these as indices in the sg_image_desc.content
    array.
*/
typedef enum sg_cube_face {
    SG_CUBEFACE_POS_X,
    SG_CUBEFACE_NEG_X,
    SG_CUBEFACE_POS_Y,
    SG_CUBEFACE_NEG_Y,
    SG_CUBEFACE_POS_Z,
    SG_CUBEFACE_NEG_Z,
    SG_CUBEFACE_NUM,
    _SG_CUBEFACE_FORCE_U32 = 0x7FFFFFFF
} sg_cube_face;

/*
    sg_shader_stage

    There are 2 shader stages: vertex- and fragment-shader-stage.
    Each shader stage

    - SG_MAX_SHADERSTAGE_UBS slots for applying uniform data
    - SG_MAX_SHADERSTAGE_IMAGES slots for images used as textures
    - SG_MAX_SHADERSTAGE_SAMPLERS slots for texture samplers
    - SG_MAX_SHADERSTAGE_STORAGEBUFFERS slots for storage buffer bindings
*/
typedef enum sg_shader_stage {
    SG_SHADERSTAGE_VS,
    SG_SHADERSTAGE_FS,
    _SG_SHADERSTAGE_FORCE_U32 = 0x7FFFFFFF
} sg_shader_stage;

/*
    sg_primitive_type

    This is the common subset of 3D primitive types supported across all 3D
    APIs. This is used in the sg_pipeline_desc.primitive_type member when
    creating a pipeline object.

    The default primitive type is SG_PRIMITIVETYPE_TRIANGLES.
*/
typedef enum sg_primitive_type {
    _SG_PRIMITIVETYPE_DEFAULT,  // value 0 reserved for default-init
    SG_PRIMITIVETYPE_POINTS,
    SG_PRIMITIVETYPE_LINES,
    SG_PRIMITIVETYPE_LINE_STRIP,
    SG_PRIMITIVETYPE_TRIANGLES,
    SG_PRIMITIVETYPE_TRIANGLE_STRIP,
    _SG_PRIMITIVETYPE_NUM,
    _SG_PRIMITIVETYPE_FORCE_U32 = 0x7FFFFFFF
} sg_primitive_type;

/*
    sg_filter

    The filtering mode when sampling a texture image. This is
    used in the sg_sampler_desc.min_filter, sg_sampler_desc.mag_filter
    and sg_sampler_desc.mipmap_filter members when creating a sampler object.

    For min_filter and mag_filter the default is SG_FILTER_NEAREST.

    For mipmap_filter the default is SG_FILTER_NONE.
*/
typedef enum sg_filter {
    _SG_FILTER_DEFAULT, // value 0 reserved for default-init
    SG_FILTER_NONE,     // FIXME: deprecated
    SG_FILTER_NEAREST,
    SG_FILTER_LINEAR,
    _SG_FILTER_NUM,
    _SG_FILTER_FORCE_U32 = 0x7FFFFFFF
} sg_filter;

/*
    sg_wrap

    The texture coordinates wrapping mode when sampling a texture
    image. This is used in the sg_image_desc.wrap_u, .wrap_v
    and .wrap_w members when creating an image.

    The default wrap mode is SG_WRAP_REPEAT.

    NOTE: SG_WRAP_CLAMP_TO_BORDER is not supported on all backends
    and platforms. To check for support, call sg_query_features()
    and check the "clamp_to_border" boolean in the returned
    sg_features struct.

    Platforms which don't support SG_WRAP_CLAMP_TO_BORDER will silently fall back
    to SG_WRAP_CLAMP_TO_EDGE without a validation error.
*/
typedef enum sg_wrap {
    _SG_WRAP_DEFAULT,   // value 0 reserved for default-init
    SG_WRAP_REPEAT,
    SG_WRAP_CLAMP_TO_EDGE,
    SG_WRAP_CLAMP_TO_BORDER,
    SG_WRAP_MIRRORED_REPEAT,
    _SG_WRAP_NUM,
    _SG_WRAP_FORCE_U32 = 0x7FFFFFFF
} sg_wrap;

/*
    sg_border_color

    The border color to use when sampling a texture, and the UV wrap
    mode is SG_WRAP_CLAMP_TO_BORDER.

    The default border color is SG_BORDERCOLOR_OPAQUE_BLACK
*/
typedef enum sg_border_color {
    _SG_BORDERCOLOR_DEFAULT,    // value 0 reserved for default-init
    SG_BORDERCOLOR_TRANSPARENT_BLACK,
    SG_BORDERCOLOR_OPAQUE_BLACK,
    SG_BORDERCOLOR_OPAQUE_WHITE,
    _SG_BORDERCOLOR_NUM,
    _SG_BORDERCOLOR_FORCE_U32 = 0x7FFFFFFF
} sg_border_color;

/*
    sg_vertex_format

    The data type of a vertex component. This is used to describe
    the layout of vertex data when creating a pipeline object.
*/
typedef enum sg_vertex_format {
    SG_VERTEXFORMAT_INVALID,
    SG_VERTEXFORMAT_FLOAT,
    SG_VERTEXFORMAT_FLOAT2,
    SG_VERTEXFORMAT_FLOAT3,
    SG_VERTEXFORMAT_FLOAT4,
    SG_VERTEXFORMAT_BYTE4,
    SG_VERTEXFORMAT_BYTE4N,
    SG_VERTEXFORMAT_UBYTE4,
    SG_VERTEXFORMAT_UBYTE4N,
    SG_VERTEXFORMAT_SHORT2,
    SG_VERTEXFORMAT_SHORT2N,
    SG_VERTEXFORMAT_USHORT2N,
    SG_VERTEXFORMAT_SHORT4,
    SG_VERTEXFORMAT_SHORT4N,
    SG_VERTEXFORMAT_USHORT4N,
    SG_VERTEXFORMAT_UINT10_N2,
    SG_VERTEXFORMAT_HALF2,
    SG_VERTEXFORMAT_HALF4,
    _SG_VERTEXFORMAT_NUM,
    _SG_VERTEXFORMAT_FORCE_U32 = 0x7FFFFFFF
} sg_vertex_format;

/*
    sg_vertex_step

    Defines whether the input pointer of a vertex input stream is advanced
    'per vertex' or 'per instance'. The default step-func is
    SG_VERTEXSTEP_PER_VERTEX. SG_VERTEXSTEP_PER_INSTANCE is used with
    instanced-rendering.

    The vertex-step is part of the vertex-layout definition
    when creating pipeline objects.
*/
typedef enum sg_vertex_step {
    _SG_VERTEXSTEP_DEFAULT,     // value 0 reserved for default-init
    SG_VERTEXSTEP_PER_VERTEX,
    SG_VERTEXSTEP_PER_INSTANCE,
    _SG_VERTEXSTEP_NUM,
    _SG_VERTEXSTEP_FORCE_U32 = 0x7FFFFFFF
} sg_vertex_step;

/*
    sg_uniform_type

    The data type of a uniform block member. This is used to
    describe the internal layout of uniform blocks when creating
    a shader object.
*/
typedef enum sg_uniform_type {
    SG_UNIFORMTYPE_INVALID,
    SG_UNIFORMTYPE_FLOAT,
    SG_UNIFORMTYPE_FLOAT2,
    SG_UNIFORMTYPE_FLOAT3,
    SG_UNIFORMTYPE_FLOAT4,
    SG_UNIFORMTYPE_INT,
    SG_UNIFORMTYPE_INT2,
    SG_UNIFORMTYPE_INT3,
    SG_UNIFORMTYPE_INT4,
    SG_UNIFORMTYPE_MAT4,
    _SG_UNIFORMTYPE_NUM,
    _SG_UNIFORMTYPE_FORCE_U32 = 0x7FFFFFFF
} sg_uniform_type;

/*
    sg_uniform_layout

    A hint for the interior memory layout of uniform blocks. This is
    only really relevant for the GL backend where the internal layout
    of uniform blocks must be known to sokol-gfx. For all other backends the
    internal memory layout of uniform blocks doesn't matter, sokol-gfx
    will just pass uniform data as a single memory blob to the
    3D backend.

    SG_UNIFORMLAYOUT_NATIVE (default)
        Native layout means that a 'backend-native' memory layout
        is used. For the GL backend this means that uniforms
        are packed tightly in memory (e.g. there are no padding
        bytes).

    SG_UNIFORMLAYOUT_STD140
        The memory layout is a subset of std140. Arrays are only
        allowed for the FLOAT4, INT4 and MAT4. Alignment is as
        is as follows:

            FLOAT, INT:         4 byte alignment
            FLOAT2, INT2:       8 byte alignment
            FLOAT3, INT3:       16 byte alignment(!)
            FLOAT4, INT4:       16 byte alignment
            MAT4:               16 byte alignment
            FLOAT4[], INT4[]:   16 byte alignment

        The overall size of the uniform block must be a multiple
        of 16.

    For more information search for 'UNIFORM DATA LAYOUT' in the documentation block
    at the start of the header.
*/
typedef enum sg_uniform_layout {
    _SG_UNIFORMLAYOUT_DEFAULT,     // value 0 reserved for default-init
    SG_UNIFORMLAYOUT_NATIVE,       // default: layout depends on currently active backend
    SG_UNIFORMLAYOUT_STD140,       // std140: memory layout according to std140
    _SG_UNIFORMLAYOUT_NUM,
    _SG_UNIFORMLAYOUT_FORCE_U32 = 0x7FFFFFFF
} sg_uniform_layout;

/*
    sg_cull_mode

    The face-culling mode, this is used in the
    sg_pipeline_desc.cull_mode member when creating a
    pipeline object.

    The default cull mode is SG_CULLMODE_NONE
*/
typedef enum sg_cull_mode {
    _SG_CULLMODE_DEFAULT,   // value 0 reserved for default-init
    SG_CULLMODE_NONE,
    SG_CULLMODE_FRONT,
    SG_CULLMODE_BACK,
    _SG_CULLMODE_NUM,
    _SG_CULLMODE_FORCE_U32 = 0x7FFFFFFF
} sg_cull_mode;

/*
    sg_face_winding

    The vertex-winding rule that determines a front-facing primitive. This
    is used in the member sg_pipeline_desc.face_winding
    when creating a pipeline object.

    The default winding is SG_FACEWINDING_CW (clockwise)
*/
typedef enum sg_face_winding {
    _SG_FACEWINDING_DEFAULT,    // value 0 reserved for default-init
    SG_FACEWINDING_CCW,
    SG_FACEWINDING_CW,
    _SG_FACEWINDING_NUM,
    _SG_FACEWINDING_FORCE_U32 = 0x7FFFFFFF
} sg_face_winding;

/*
    sg_compare_func

    The compare-function for configuring depth- and stencil-ref tests
    in pipeline objects, and for texture samplers which perform a comparison
    instead of regular sampling operation.

    sg_pipeline_desc
        .depth
            .compare
        .stencil
            .front.compare
            .back.compar

    sg_sampler_desc
        .compare

    The default compare func for depth- and stencil-tests is
    SG_COMPAREFUNC_ALWAYS.

    The default compare func for sampler is SG_COMPAREFUNC_NEVER.
*/
typedef enum sg_compare_func {
    _SG_COMPAREFUNC_DEFAULT,    // value 0 reserved for default-init
    SG_COMPAREFUNC_NEVER,
    SG_COMPAREFUNC_LESS,
    SG_COMPAREFUNC_EQUAL,
    SG_COMPAREFUNC_LESS_EQUAL,
    SG_COMPAREFUNC_GREATER,
    SG_COMPAREFUNC_NOT_EQUAL,
    SG_COMPAREFUNC_GREATER_EQUAL,
    SG_COMPAREFUNC_ALWAYS,
    _SG_COMPAREFUNC_NUM,
    _SG_COMPAREFUNC_FORCE_U32 = 0x7FFFFFFF
} sg_compare_func;

/*
    sg_stencil_op

    The operation performed on a currently stored stencil-value when a
    comparison test passes or fails. This is used when creating a pipeline
    object in the members:

    sg_pipeline_desc
        .stencil
            .front
                .fail_op
                .depth_fail_op
                .pass_op
            .back
                .fail_op
                .depth_fail_op
                .pass_op

    The default value is SG_STENCILOP_KEEP.
*/
typedef enum sg_stencil_op {
    _SG_STENCILOP_DEFAULT,      // value 0 reserved for default-init
    SG_STENCILOP_KEEP,
    SG_STENCILOP_ZERO,
    SG_STENCILOP_REPLACE,
    SG_STENCILOP_INCR_CLAMP,
    SG_STENCILOP_DECR_CLAMP,
    SG_STENCILOP_INVERT,
    SG_STENCILOP_INCR_WRAP,
    SG_STENCILOP_DECR_WRAP,
    _SG_STENCILOP_NUM,
    _SG_STENCILOP_FORCE_U32 = 0x7FFFFFFF
} sg_stencil_op;

/*
    sg_blend_factor

    The source and destination factors in blending operations.
    This is used in the following members when creating a pipeline object:

    sg_pipeline_desc
        .colors[i]
            .blend
                .src_factor_rgb
                .dst_factor_rgb
                .src_factor_alpha
                .dst_factor_alpha

    The default value is SG_BLENDFACTOR_ONE for source
    factors, and SG_BLENDFACTOR_ZERO for destination factors.
*/
typedef enum sg_blend_factor {
    _SG_BLENDFACTOR_DEFAULT,    // value 0 reserved for default-init
    SG_BLENDFACTOR_ZERO,
    SG_BLENDFACTOR_ONE,
    SG_BLENDFACTOR_SRC_COLOR,
    SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR,
    SG_BLENDFACTOR_SRC_ALPHA,
    SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
    SG_BLENDFACTOR_DST_COLOR,
    SG_BLENDFACTOR_ONE_MINUS_DST_COLOR,
    SG_BLENDFACTOR_DST_ALPHA,
    SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
    SG_BLENDFACTOR_SRC_ALPHA_SATURATED,
    SG_BLENDFACTOR_BLEND_COLOR,
    SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR,
    SG_BLENDFACTOR_BLEND_ALPHA,
    SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA,
    _SG_BLENDFACTOR_NUM,
    _SG_BLENDFACTOR_FORCE_U32 = 0x7FFFFFFF
} sg_blend_factor;

/*
    sg_blend_op

    Describes how the source and destination values are combined in the
    fragment blending operation. It is used in the following members when
    creating a pipeline object:

    sg_pipeline_desc
        .colors[i]
            .blend
                .op_rgb
                .op_alpha

    The default value is SG_BLENDOP_ADD.
*/
typedef enum sg_blend_op {
    _SG_BLENDOP_DEFAULT,    // value 0 reserved for default-init
    SG_BLENDOP_ADD,
    SG_BLENDOP_SUBTRACT,
    SG_BLENDOP_REVERSE_SUBTRACT,
    _SG_BLENDOP_NUM,
    _SG_BLENDOP_FORCE_U32 = 0x7FFFFFFF
} sg_blend_op;

/*
    sg_color_mask

    Selects the active color channels when writing a fragment color to the
    framebuffer. This is used in the members
    sg_pipeline_desc.colors[i].write_mask when creating a pipeline object.

    The default colormask is SG_COLORMASK_RGBA (write all colors channels)

    NOTE: since the color mask value 0 is reserved for the default value
    (SG_COLORMASK_RGBA), use SG_COLORMASK_NONE if all color channels
    should be disabled.
*/
typedef enum sg_color_mask {
    _SG_COLORMASK_DEFAULT = 0,    // value 0 reserved for default-init
    SG_COLORMASK_NONE   = 0x10,   // special value for 'all channels disabled
    SG_COLORMASK_R      = 0x1,
    SG_COLORMASK_G      = 0x2,
    SG_COLORMASK_RG     = 0x3,
    SG_COLORMASK_B      = 0x4,
    SG_COLORMASK_RB     = 0x5,
    SG_COLORMASK_GB     = 0x6,
    SG_COLORMASK_RGB    = 0x7,
    SG_COLORMASK_A      = 0x8,
    SG_COLORMASK_RA     = 0x9,
    SG_COLORMASK_GA     = 0xA,
    SG_COLORMASK_RGA    = 0xB,
    SG_COLORMASK_BA     = 0xC,
    SG_COLORMASK_RBA    = 0xD,
    SG_COLORMASK_GBA    = 0xE,
    SG_COLORMASK_RGBA   = 0xF,
    _SG_COLORMASK_FORCE_U32 = 0x7FFFFFFF
} sg_color_mask;

/*
    sg_load_action

    Defines the load action that should be performed at the start of a render pass:

    SG_LOADACTION_CLEAR:        clear the render target
    SG_LOADACTION_LOAD:         load the previous content of the render target
    SG_LOADACTION_DONTCARE:     leave the render target in an undefined state

    This is used in the sg_pass_action structure.

    The default load action for all pass attachments is SG_LOADACTION_CLEAR,
    with the values rgba = { 0.5f, 0.5f, 0.5f, 1.0f }, depth=1.0f and stencil=0.

    If you want to override the default behaviour, it is important to not
    only set the clear color, but the 'action' field as well (as long as this
    is _SG_LOADACTION_DEFAULT, the value fields will be ignored).
*/
typedef enum sg_load_action {
    _SG_LOADACTION_DEFAULT,
    SG_LOADACTION_CLEAR,
    SG_LOADACTION_LOAD,
    SG_LOADACTION_DONTCARE,
    _SG_LOADACTION_FORCE_U32 = 0x7FFFFFFF
} sg_load_action;

/*
    sg_store_action

    Defines the store action that be performed at the end of a render pass:

    SG_STOREACTION_STORE:       store the rendered content to the color attachment image
    SG_STOREACTION_DONTCARE:    allows the GPU to discard the rendered content
*/
typedef enum sg_store_action {
    _SG_STOREACTION_DEFAULT,
    SG_STOREACTION_STORE,
    SG_STOREACTION_DONTCARE,
    _SG_STOREACTION_FORCE_U32 = 0x7FFFFFFF
} sg_store_action;


/*
    sg_pass_action

    The sg_pass_action struct defines the actions to be performed
    at the start and end of a render pass.

    - at the start of the pass: whether the render targets should be cleared,
      loaded with their previous content, or start in an undefined state
    - for clear operations: the clear value (color, depth, or stencil values)
    - at the end of the pass: whether the rendering result should be
      stored back into the render target or discarded
*/
typedef struct sg_color_attachment_action {
    sg_load_action load_action;         // default: SG_LOADACTION_CLEAR
    sg_store_action store_action;       // default: SG_STOREACTION_STORE
    sg_color clear_value;               // default: { 0.5f, 0.5f, 0.5f, 1.0f }
} sg_color_attachment_action;

typedef struct sg_depth_attachment_action {
    sg_load_action load_action;         // default: SG_LOADACTION_CLEAR
    sg_store_action store_action;       // default: SG_STOREACTION_DONTCARE
    float clear_value;                  // default: 1.0
} sg_depth_attachment_action;

typedef struct sg_stencil_attachment_action {
    sg_load_action load_action;         // default: SG_LOADACTION_CLEAR
    sg_store_action store_action;       // default: SG_STOREACTION_DONTCARE
    uint8_t clear_value;                // default: 0
} sg_stencil_attachment_action;

typedef struct sg_pass_action {
    sg_color_attachment_action colors[SG_MAX_COLOR_ATTACHMENTS];
    sg_depth_attachment_action depth;
    sg_stencil_attachment_action stencil;
} sg_pass_action;

/*
    sg_swapchain

    Used in sg_begin_pass() to provide details about an external swapchain
    (pixel formats, sample count and backend-API specific render surface objects).

    The following information must be provided:

    - the width and height of the swapchain surfaces in number of pixels,
    - the pixel format of the render- and optional msaa-resolve-surface
    - the pixel format of the optional depth- or depth-stencil-surface
    - the MSAA sample count for the render and depth-stencil surface

    If the pixel formats and MSAA sample counts are left zero-initialized,
    their defaults are taken from the sg_environment struct provided in the
    sg_setup() call.

    The width and height *must* be > 0.

    Additionally the following backend API specific objects must be passed in
    as 'type erased' void pointers:

    GL: on all GL backends, a GL framebuffer object must be provided. This
    can be zero for the default framebuffer.

    D3D11:
        - an ID3D11RenderTargetView for the rendering surface, without
          MSAA rendering this surface will also be displayed
        - an optional ID3D11DepthStencilView for the depth- or depth/stencil
          buffer surface
        - when MSAA rendering is used, another ID3D11RenderTargetView
          which serves as MSAA resolve target and will be displayed

    WebGPU (same as D3D11, except different types)
        - a WGPUTextureView for the rendering surface, without
          MSAA rendering this surface will also be displayed
        - an optional WGPUTextureView for the depth- or depth/stencil
          buffer surface
        - when MSAA rendering is used, another WGPUTextureView
          which serves as MSAA resolve target and will be displayed

    Metal (NOTE that the rolves of provided surfaces is slightly different
    than on D3D11 or WebGPU in case of MSAA vs non-MSAA rendering):

        - A current CAMetalDrawable (NOT an MTLDrawable!) which will be presented.
          This will either be rendered to directly (if no MSAA is used), or serve
          as MSAA-resolve target.
        - an optional MTLTexture for the depth- or depth-stencil buffer
        - an optional multisampled MTLTexture which serves as intermediate
          rendering surface which will then be resolved into the
          CAMetalDrawable.

    NOTE that for Metal you must use an ObjC __bridge cast to
    properly tunnel the ObjC object handle through a C void*, e.g.:

        swapchain.metal.current_drawable = (__bridge const void*) [mtkView currentDrawable];

    On all other backends you shouldn't need to mess with the reference count.

    It's a good practice to write a helper function which returns an initialized
    sg_swapchain structs, which can then be plugged directly into
    sg_pass.swapchain. Look at the function sglue_swapchain() in the sokol_glue.h
    as an example.
*/
typedef struct sg_metal_swapchain {
    const void* current_drawable;       // CAMetalDrawable (NOT MTLDrawable!!!)
    const void* depth_stencil_texture;  // MTLTexture
    const void* msaa_color_texture;     // MTLTexture
} sg_metal_swapchain;

typedef struct sg_d3d11_swapchain {
    const void* render_view;            // ID3D11RenderTargetView
    const void* resolve_view;           // ID3D11RenderTargetView
    const void* depth_stencil_view;     // ID3D11DepthStencilView
} sg_d3d11_swapchain;


typedef struct sg_gl_swapchain {
    uint32_t framebuffer;               // GL framebuffer object
} sg_gl_swapchain;

typedef struct sg_swapchain {
    int width;
    int height;
    int sample_count;
    sg_pixel_format color_format;
    sg_pixel_format depth_format;
    sg_metal_swapchain metal;
    sg_d3d11_swapchain d3d11;
    sg_gl_swapchain gl;
} sg_swapchain;

/*
    sg_pass

    The sg_pass structure is passed as argument into the sg_begin_pass()
    function.

    For an offscreen rendering pass, an sg_pass_action struct and sg_attachments
    object must be provided, and for swapchain passes, and sg_pass_action and
    an sg_swapchain struct. It is an error to provide both an sg_attachments
    handle and an initialized sg_swapchain struct in the same sg_begin_pass().

    An sg_begin_pass() call for an offscreen pass would look like this (where
    `attachments` is an sg_attachments handle):

        sg_begin_pass(&(sg_pass){
            .action = { ... },
            .attachments = attachments,
        });

    ...and a swapchain render pass would look like this (using the sokol_glue.h
    helper function sglue_swapchain() which gets the swapchain properties from
    sokol_app.h):

        sg_begin_pass(&(sg_pass){
            .action = { ... },
            .swapchain = sglue_swapchain(),
        });

    You can also omit the .action object to get default pass action behaviour
    (clear to color=grey, depth=1 and stencil=0).
*/
typedef struct sg_pass {
    uint32_t _start_canary;
    sg_pass_action action;
    sg_attachments attachments;
    sg_swapchain swapchain;
    const char* label;
    uint32_t _end_canary;
} sg_pass;

/*
    sg_bindings

    The sg_bindings structure defines the resource binding slots
    of the sokol_gfx render pipeline, used as argument to the
    sg_apply_bindings() function.

    A resource binding struct contains:

    - 1..N vertex buffers
    - 0..N vertex buffer offsets
    - 0..1 index buffers
    - 0..1 index buffer offsets
    - 0..N vertex shader stage images
    - 0..N vertex shader stage samplers
    - 0..N vertex shader storage buffers
    - 0..N fragment shader stage images
    - 0..N fragment shader stage samplers
    - 0..N fragment shader storage buffers

    For the max number of bindings, see the constant definitions:

    - SG_MAX_VERTEX_BUFFERS
    - SG_MAX_SHADERSTAGE_IMAGES
    - SG_MAX_SHADERSTAGE_SAMPLERS
    - SG_MAX_SHADERSTAGE_STORAGEBUFFERS

    The optional buffer offsets can be used to put different unrelated
    chunks of vertex- and/or index-data into the same buffer objects.
*/
typedef struct sg_stage_bindings {
    sg_image images[SG_MAX_SHADERSTAGE_IMAGES];
    sg_sampler samplers[SG_MAX_SHADERSTAGE_SAMPLERS];
    sg_buffer storage_buffers[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
} sg_stage_bindings;

typedef struct sg_bindings {
    uint32_t _start_canary;
    sg_buffer vertex_buffers[SG_MAX_VERTEX_BUFFERS];
    int vertex_buffer_offsets[SG_MAX_VERTEX_BUFFERS];
    sg_buffer index_buffer;
    int index_buffer_offset;
    sg_stage_bindings vs;
    sg_stage_bindings fs;
    uint32_t _end_canary;
} sg_bindings;

/*
    sg_buffer_desc

    Creation parameters for sg_buffer objects, used in the
    sg_make_buffer() call.

    The default configuration is:

    .size:      0       (*must* be >0 for buffers without data)
    .type:      SG_BUFFERTYPE_VERTEXBUFFER
    .usage:     SG_USAGE_IMMUTABLE
    .data.ptr   0       (*must* be valid for immutable buffers)
    .data.size  0       (*must* be > 0 for immutable buffers)
    .label      0       (optional string label)

    For immutable buffers which are initialized with initial data,
    keep the .size item zero-initialized, and set the size together with the
    pointer to the initial data in the .data item.

    For mutable buffers without initial data, keep the .data item
    zero-initialized, and set the buffer size in the .size item instead.

    You can also set both size values, but currently both size values must
    be identical (this may change in the future when the dynamic resource
    management may become more flexible).

    ADVANCED TOPIC: Injecting native 3D-API buffers:

    The following struct members allow to inject your own GL, Metal
    or D3D11 buffers into sokol_gfx:

    .gl_buffers[SG_NUM_INFLIGHT_FRAMES]
    .mtl_buffers[SG_NUM_INFLIGHT_FRAMES]
    .d3d11_buffer

    You must still provide all other struct items except the .data item, and
    these must match the creation parameters of the native buffers you
    provide. For SG_USAGE_IMMUTABLE, only provide a single native 3D-API
    buffer, otherwise you need to provide SG_NUM_INFLIGHT_FRAMES buffers
    (only for GL and Metal, not D3D11). Providing multiple buffers for GL and
    Metal is necessary because sokol_gfx will rotate through them when
    calling sg_update_buffer() to prevent lock-stalls.

    Note that it is expected that immutable injected buffer have already been
    initialized with content, and the .content member must be 0!

    Also you need to call sg_reset_state_cache() after calling native 3D-API
    functions, and before calling any sokol_gfx function.
*/
typedef struct sg_buffer_desc {
    uint32_t _start_canary;
    size_t size;
    sg_buffer_type type;
    sg_usage usage;
    sg_range data;
    const char* label;
    // optionally inject backend-specific resources
    uint32_t gl_buffers[SG_NUM_INFLIGHT_FRAMES];
    const void* mtl_buffers[SG_NUM_INFLIGHT_FRAMES];
    const void* d3d11_buffer;
    uint32_t _end_canary;
} sg_buffer_desc;

/*
    sg_image_data

    Defines the content of an image through a 2D array of sg_range structs.
    The first array dimension is the cubemap face, and the second array
    dimension the mipmap level.
*/
typedef struct sg_image_data {
    sg_range subimage[SG_CUBEFACE_NUM][SG_MAX_MIPMAPS];
} sg_image_data;

/*
    sg_image_desc

    Creation parameters for sg_image objects, used in the sg_make_image() call.

    The default configuration is:

    .type:              SG_IMAGETYPE_2D
    .render_target:     false
    .width              0 (must be set to >0)
    .height             0 (must be set to >0)
    .num_slices         1 (3D textures: depth; array textures: number of layers)
    .num_mipmaps:       1
    .usage:             SG_USAGE_IMMUTABLE
    .pixel_format:      SG_PIXELFORMAT_RGBA8 for textures, or sg_desc.environment.defaults.color_format for render targets
    .sample_count:      1 for textures, or sg_desc.environment.defaults.sample_count for render targets
    .data               an sg_image_data struct to define the initial content
    .label              0 (optional string label for trace hooks)

    Q: Why is the default sample_count for render targets identical with the
    "default sample count" from sg_desc.environment.defaults.sample_count?

    A: So that it matches the default sample count in pipeline objects. Even
    though it is a bit strange/confusing that offscreen render targets by default
    get the same sample count as 'default swapchains', but it's better that
    an offscreen render target created with default parameters matches
    a pipeline object created with default parameters.

    NOTE:

    Images with usage SG_USAGE_IMMUTABLE must be fully initialized by
    providing a valid .data member which points to initialization data.

    ADVANCED TOPIC: Injecting native 3D-API textures:

    The following struct members allow to inject your own GL, Metal or D3D11
    textures into sokol_gfx:

    .gl_textures[SG_NUM_INFLIGHT_FRAMES]
    .mtl_textures[SG_NUM_INFLIGHT_FRAMES]
    .d3d11_texture
    .d3d11_shader_resource_view
    .wgpu_texture
    .wgpu_texture_view

    For GL, you can also specify the texture target or leave it empty to use
    the default texture target for the image type (GL_TEXTURE_2D for
    SG_IMAGETYPE_2D etc)

    For D3D11 and WebGPU, either only provide a texture, or both a texture and
    shader-resource-view / texture-view object. If you want to use access the
    injected texture in a shader you *must* provide a shader-resource-view.

    The same rules apply as for injecting native buffers (see sg_buffer_desc
    documentation for more details).
*/
typedef struct sg_image_desc {
    uint32_t _start_canary;
    sg_image_type type;
    bool render_target;
    int width;
    int height;
    int num_slices;
    int num_mipmaps;
    sg_usage usage;
    sg_pixel_format pixel_format;
    int sample_count;
    sg_image_data data;
    const char* label;
    // optionally inject backend-specific resources
    uint32_t gl_textures[SG_NUM_INFLIGHT_FRAMES];
    uint32_t gl_texture_target;
    const void* mtl_textures[SG_NUM_INFLIGHT_FRAMES];
    const void* d3d11_texture;
    const void* d3d11_shader_resource_view;
    uint32_t _end_canary;
} sg_image_desc;

/*
    sg_sampler_desc

    Creation parameters for sg_sampler objects, used in the sg_make_sampler() call

    .min_filter:        SG_FILTER_NEAREST
    .mag_filter:        SG_FILTER_NEAREST
    .mipmap_filter      SG_FILTER_NONE
    .wrap_u:            SG_WRAP_REPEAT
    .wrap_v:            SG_WRAP_REPEAT
    .wrap_w:            SG_WRAP_REPEAT (only SG_IMAGETYPE_3D)
    .min_lod            0.0f
    .max_lod            FLT_MAX
    .border_color       SG_BORDERCOLOR_OPAQUE_BLACK
    .compare            SG_COMPAREFUNC_NEVER
    .max_anisotropy     1 (must be 1..16)

*/
typedef struct sg_sampler_desc {
    uint32_t _start_canary;
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
    const char* label;
    // optionally inject backend-specific resources
    uint32_t gl_sampler;
    const void* mtl_sampler;
    const void* d3d11_sampler;
    uint32_t _end_canary;
} sg_sampler_desc;

/*
    sg_shader_desc

    The structure sg_shader_desc defines all creation parameters for shader
    programs, used as input to the sg_make_shader() function:

    - reflection information for vertex attributes (vertex shader inputs):
        - vertex attribute name (only optionally used by GLES3 and GL)
        - a semantic name and index (required for D3D11)
    - for each shader-stage (vertex and fragment):
        - the shader source or bytecode
        - an optional entry function name
        - an optional compile target (only for D3D11 when source is provided,
          defaults are "vs_4_0" and "ps_4_0")
        - reflection info for each uniform block used by the shader stage:
            - the size of the uniform block in bytes
            - a memory layout hint (native vs std140, only required for GL backends)
            - reflection info for each uniform block member (only required for GL backends):
                - member name
                - member type (SG_UNIFORMTYPE_xxx)
                - if the member is an array, the number of array items
        - reflection info for textures used in the shader stage:
            - the image type (SG_IMAGETYPE_xxx)
            - the image-sample type (SG_IMAGESAMPLETYPE_xxx, default is SG_IMAGESAMPLETYPE_FLOAT)
            - whether the shader expects a multisampled texture
        - reflection info for samplers used in the shader stage:
            - the sampler type (SG_SAMPLERTYPE_xxx)
        - reflection info for each image-sampler-pair used by the shader:
            - the texture slot of the involved texture
            - the sampler slot of the involved sampler
            - for GLSL only: the name of the combined image-sampler object
        - reflection info for each storage-buffer used by the shader:
            - whether the storage buffer is readonly (currently this
              must be true)

    For all GL backends, shader source-code must be provided. For D3D11 and Metal,
    either shader source-code or byte-code can be provided.

    For D3D11, if source code is provided, the d3dcompiler_47.dll will be loaded
    on demand. If this fails, shader creation will fail. When compiling HLSL
    source code, you can provide an optional target string via
    sg_shader_stage_desc.d3d11_target, the default target is "vs_4_0" for the
    vertex shader stage and "ps_4_0" for the pixel shader stage.
*/
typedef struct sg_shader_attr_desc {
    const char* name;           // GLSL vertex attribute name (optional)
    const char* sem_name;       // HLSL semantic name
    int sem_index;              // HLSL semantic index
} sg_shader_attr_desc;

typedef struct sg_shader_uniform_desc {
    const char* name;
    sg_uniform_type type;
    int array_count;
} sg_shader_uniform_desc;

typedef struct sg_shader_uniform_block_desc {
    size_t size;
    sg_uniform_layout layout;
    sg_shader_uniform_desc uniforms[SG_MAX_UB_MEMBERS];
} sg_shader_uniform_block_desc;

typedef struct sg_shader_storage_buffer_desc {
    bool used;
    bool readonly;
} sg_shader_storage_buffer_desc;

typedef struct sg_shader_image_desc {
    bool used;
    bool multisampled;
    sg_image_type image_type;
    sg_image_sample_type sample_type;
} sg_shader_image_desc;

typedef struct sg_shader_sampler_desc {
    bool used;
    sg_sampler_type sampler_type;
} sg_shader_sampler_desc;

typedef struct sg_shader_image_sampler_pair_desc {
    bool used;
    int image_slot;
    int sampler_slot;
    const char* glsl_name;
} sg_shader_image_sampler_pair_desc;

typedef struct sg_shader_stage_desc {
    const char* source;
    sg_range bytecode;
    const char* entry;
    const char* d3d11_target;
    sg_shader_uniform_block_desc uniform_blocks[SG_MAX_SHADERSTAGE_UBS];
    sg_shader_storage_buffer_desc storage_buffers[SG_MAX_SHADERSTAGE_STORAGEBUFFERS];
    sg_shader_image_desc images[SG_MAX_SHADERSTAGE_IMAGES];
    sg_shader_sampler_desc samplers[SG_MAX_SHADERSTAGE_SAMPLERS];
    sg_shader_image_sampler_pair_desc image_sampler_pairs[SG_MAX_SHADERSTAGE_IMAGESAMPLERPAIRS];
} sg_shader_stage_desc;

typedef struct sg_shader_desc {
    uint32_t _start_canary;
    sg_shader_attr_desc attrs[SG_MAX_VERTEX_ATTRIBUTES];
    sg_shader_stage_desc vs;
    sg_shader_stage_desc fs;
    const char* label;
    uint32_t _end_canary;
} sg_shader_desc;

/*
    sg_pipeline_desc

    The sg_pipeline_desc struct defines all creation parameters for an
    sg_pipeline object, used as argument to the sg_make_pipeline() function:

    - the vertex layout for all input vertex buffers
    - a shader object
    - the 3D primitive type (points, lines, triangles, ...)
    - the index type (none, 16- or 32-bit)
    - all the fixed-function-pipeline state (depth-, stencil-, blend-state, etc...)

    If the vertex data has no gaps between vertex components, you can omit
    the .layout.buffers[].stride and layout.attrs[].offset items (leave them
    default-initialized to 0), sokol-gfx will then compute the offsets and
    strides from the vertex component formats (.layout.attrs[].format).
    Please note that ALL vertex attribute offsets must be 0 in order for the
    automatic offset computation to kick in.

    The default configuration is as follows:

    .shader:            0 (must be initialized with a valid sg_shader id!)
    .layout:
        .buffers[]:         vertex buffer layouts
            .stride:        0 (if no stride is given it will be computed)
            .step_func      SG_VERTEXSTEP_PER_VERTEX
            .step_rate      1
        .attrs[]:           vertex attribute declarations
            .buffer_index   0 the vertex buffer bind slot
            .offset         0 (offsets can be omitted if the vertex layout has no gaps)
            .format         SG_VERTEXFORMAT_INVALID (must be initialized!)
    .depth:
        .pixel_format:      sg_desc.context.depth_format
        .compare:           SG_COMPAREFUNC_ALWAYS
        .write_enabled:     false
        .bias:              0.0f
        .bias_slope_scale:  0.0f
        .bias_clamp:        0.0f
    .stencil:
        .enabled:           false
        .front/back:
            .compare:       SG_COMPAREFUNC_ALWAYS
            .fail_op:       SG_STENCILOP_KEEP
            .depth_fail_op: SG_STENCILOP_KEEP
            .pass_op:       SG_STENCILOP_KEEP
        .read_mask:         0
        .write_mask:        0
        .ref:               0
    .color_count            1
    .colors[0..color_count]
        .pixel_format       sg_desc.context.color_format
        .write_mask:        SG_COLORMASK_RGBA
        .blend:
            .enabled:           false
            .src_factor_rgb:    SG_BLENDFACTOR_ONE
            .dst_factor_rgb:    SG_BLENDFACTOR_ZERO
            .op_rgb:            SG_BLENDOP_ADD
            .src_factor_alpha:  SG_BLENDFACTOR_ONE
            .dst_factor_alpha:  SG_BLENDFACTOR_ZERO
            .op_alpha:          SG_BLENDOP_ADD
    .primitive_type:            SG_PRIMITIVETYPE_TRIANGLES
    .index_type:                SG_INDEXTYPE_NONE
    .cull_mode:                 SG_CULLMODE_NONE
    .face_winding:              SG_FACEWINDING_CW
    .sample_count:              sg_desc.context.sample_count
    .blend_color:               (sg_color) { 0.0f, 0.0f, 0.0f, 0.0f }
    .alpha_to_coverage_enabled: false
    .label  0       (optional string label for trace hooks)
*/
typedef struct sg_vertex_buffer_layout_state {
    int stride;
    sg_vertex_step step_func;
    int step_rate;
} sg_vertex_buffer_layout_state;

typedef struct sg_vertex_attr_state {
    int buffer_index;
    int offset;
    sg_vertex_format format;
} sg_vertex_attr_state;

typedef struct sg_vertex_layout_state {
    sg_vertex_buffer_layout_state buffers[SG_MAX_VERTEX_BUFFERS];
    sg_vertex_attr_state attrs[SG_MAX_VERTEX_ATTRIBUTES];
} sg_vertex_layout_state;

typedef struct sg_stencil_face_state {
    sg_compare_func compare;
    sg_stencil_op fail_op;
    sg_stencil_op depth_fail_op;
    sg_stencil_op pass_op;
} sg_stencil_face_state;

typedef struct sg_stencil_state {
    bool enabled;
    sg_stencil_face_state front;
    sg_stencil_face_state back;
    uint8_t read_mask;
    uint8_t write_mask;
    uint8_t ref;
} sg_stencil_state;

typedef struct sg_depth_state {
    sg_pixel_format pixel_format;
    sg_compare_func compare;
    bool write_enabled;
    float bias;
    float bias_slope_scale;
    float bias_clamp;
} sg_depth_state;

typedef struct sg_blend_state {
    bool enabled;
    sg_blend_factor src_factor_rgb;
    sg_blend_factor dst_factor_rgb;
    sg_blend_op op_rgb;
    sg_blend_factor src_factor_alpha;
    sg_blend_factor dst_factor_alpha;
    sg_blend_op op_alpha;
} sg_blend_state;

typedef struct sg_color_target_state {
    sg_pixel_format pixel_format;
    sg_color_mask write_mask;
    sg_blend_state blend;
} sg_color_target_state;

typedef struct sg_pipeline_desc {
    uint32_t _start_canary;
    sg_shader shader;
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
    const char* label;
    uint32_t _end_canary;
} sg_pipeline_desc;

/*
    sg_attachments_desc

    Creation parameters for an sg_attachments object, used as argument to the
    sg_make_attachments() function.

    An attachments object bundles 0..4 color attachments, 0..4 msaa-resolve
    attachments, and none or one depth-stencil attachmente for use
    in a render pass. At least one color attachment or one depth-stencil
    attachment must be provided (no color attachment and a depth-stencil
    attachment is useful for a depth-only render pass).

    Each attachment definition consists of an image object, and two additional indices
    describing which subimage the pass will render into: one mipmap index, and if the image
    is a cubemap, array-texture or 3D-texture, the face-index, array-layer or
    depth-slice.

    All attachments must have the same width and height.

    All color attachments and the depth-stencil attachment must have the
    same sample count.

    If a resolve attachment is set, an MSAA-resolve operation from the
    associated color attachment image into the resolve attachment image will take
    place in the sg_end_pass() function. In this case, the color attachment
    must have a (sample_count>1), and the resolve attachment a
    (sample_count==1). The resolve attachment also must have the same pixel
    format as the color attachment.

    NOTE that MSAA depth-stencil attachments cannot be msaa-resolved!
*/
typedef struct sg_attachment_desc {
    sg_image image;
    int mip_level;
    int slice;      // cube texture: face; array texture: layer; 3D texture: slice
} sg_attachment_desc;

typedef struct sg_attachments_desc {
    uint32_t _start_canary;
    sg_attachment_desc colors[SG_MAX_COLOR_ATTACHMENTS];
    sg_attachment_desc resolves[SG_MAX_COLOR_ATTACHMENTS];
    sg_attachment_desc depth_stencil;
    const char* label;
    uint32_t _end_canary;
} sg_attachments_desc;

/*
    sg_trace_hooks

    Installable callback functions to keep track of the sokol-gfx calls,
    this is useful for debugging, or keeping track of resource creation
    and destruction.

    Trace hooks are installed with sg_install_trace_hooks(), this returns
    another sg_trace_hooks struct with the previous set of
    trace hook function pointers. These should be invoked by the
    new trace hooks to form a proper call chain.
*/
typedef struct sg_trace_hooks {
    void* user_data;
    void (*reset_state_cache)(void* user_data);
    void (*make_buffer)(const sg_buffer_desc* desc, sg_buffer result, void* user_data);
    void (*make_image)(const sg_image_desc* desc, sg_image result, void* user_data);
    void (*make_sampler)(const sg_sampler_desc* desc, sg_sampler result, void* user_data);
    void (*make_shader)(const sg_shader_desc* desc, sg_shader result, void* user_data);
    void (*make_pipeline)(const sg_pipeline_desc* desc, sg_pipeline result, void* user_data);
    void (*make_attachments)(const sg_attachments_desc* desc, sg_attachments result, void* user_data);
    void (*destroy_buffer)(sg_buffer buf, void* user_data);
    void (*destroy_image)(sg_image img, void* user_data);
    void (*destroy_sampler)(sg_sampler smp, void* user_data);
    void (*destroy_shader)(sg_shader shd, void* user_data);
    void (*destroy_pipeline)(sg_pipeline pip, void* user_data);
    void (*destroy_attachments)(sg_attachments atts, void* user_data);
    void (*update_buffer)(sg_buffer buf, const sg_range* data, void* user_data);
    void (*update_image)(sg_image img, const sg_image_data* data, void* user_data);
    void (*append_buffer)(sg_buffer buf, const sg_range* data, int result, void* user_data);
    void (*begin_pass)(const sg_pass* pass, void* user_data);
    void (*apply_viewport)(int x, int y, int width, int height, bool origin_top_left, void* user_data);
    void (*apply_scissor_rect)(int x, int y, int width, int height, bool origin_top_left, void* user_data);
    void (*apply_pipeline)(sg_pipeline pip, void* user_data);
    void (*apply_bindings)(const sg_bindings* bindings, void* user_data);
    void (*apply_uniforms)(sg_shader_stage stage, int ub_index, const sg_range* data, void* user_data);
    void (*draw)(int base_element, int num_elements, int num_instances, void* user_data);
    void (*end_pass)(void* user_data);
    void (*commit)(void* user_data);
    void (*alloc_buffer)(sg_buffer result, void* user_data);
    void (*alloc_image)(sg_image result, void* user_data);
    void (*alloc_sampler)(sg_sampler result, void* user_data);
    void (*alloc_shader)(sg_shader result, void* user_data);
    void (*alloc_pipeline)(sg_pipeline result, void* user_data);
    void (*alloc_attachments)(sg_attachments result, void* user_data);
    void (*dealloc_buffer)(sg_buffer buf_id, void* user_data);
    void (*dealloc_image)(sg_image img_id, void* user_data);
    void (*dealloc_sampler)(sg_sampler smp_id, void* user_data);
    void (*dealloc_shader)(sg_shader shd_id, void* user_data);
    void (*dealloc_pipeline)(sg_pipeline pip_id, void* user_data);
    void (*dealloc_attachments)(sg_attachments atts_id, void* user_data);
    void (*init_buffer)(sg_buffer buf_id, const sg_buffer_desc* desc, void* user_data);
    void (*init_image)(sg_image img_id, const sg_image_desc* desc, void* user_data);
    void (*init_sampler)(sg_sampler smp_id, const sg_sampler_desc* desc, void* user_data);
    void (*init_shader)(sg_shader shd_id, const sg_shader_desc* desc, void* user_data);
    void (*init_pipeline)(sg_pipeline pip_id, const sg_pipeline_desc* desc, void* user_data);
    void (*init_attachments)(sg_attachments atts_id, const sg_attachments_desc* desc, void* user_data);
    void (*uninit_buffer)(sg_buffer buf_id, void* user_data);
    void (*uninit_image)(sg_image img_id, void* user_data);
    void (*uninit_sampler)(sg_sampler smp_id, void* user_data);
    void (*uninit_shader)(sg_shader shd_id, void* user_data);
    void (*uninit_pipeline)(sg_pipeline pip_id, void* user_data);
    void (*uninit_attachments)(sg_attachments atts_id, void* user_data);
    void (*fail_buffer)(sg_buffer buf_id, void* user_data);
    void (*fail_image)(sg_image img_id, void* user_data);
    void (*fail_sampler)(sg_sampler smp_id, void* user_data);
    void (*fail_shader)(sg_shader shd_id, void* user_data);
    void (*fail_pipeline)(sg_pipeline pip_id, void* user_data);
    void (*fail_attachments)(sg_attachments atts_id, void* user_data);
    void (*push_debug_group)(const char* name, void* user_data);
    void (*pop_debug_group)(void* user_data);
} sg_trace_hooks;

/*
    sg_buffer_info
    sg_image_info
    sg_sampler_info
    sg_shader_info
    sg_pipeline_info
    sg_attachments_info

    These structs contain various internal resource attributes which
    might be useful for debug-inspection. Please don't rely on the
    actual content of those structs too much, as they are quite closely
    tied to sokol_gfx.h internals and may change more frequently than
    the other public API elements.

    The *_info structs are used as the return values of the following functions:

    sg_query_buffer_info()
    sg_query_image_info()
    sg_query_sampler_info()
    sg_query_shader_info()
    sg_query_pipeline_info()
    sg_query_pass_info()
*/
typedef struct sg_slot_info {
    sg_resource_state state;    // the current state of this resource slot
    uint32_t res_id;            // type-neutral resource if (e.g. sg_buffer.id)
} sg_slot_info;

typedef struct sg_buffer_info {
    sg_slot_info slot;              // resource pool slot info
    uint32_t update_frame_index;    // frame index of last sg_update_buffer()
    uint32_t append_frame_index;    // frame index of last sg_append_buffer()
    int append_pos;                 // current position in buffer for sg_append_buffer()
    bool append_overflow;           // is buffer in overflow state (due to sg_append_buffer)
    int num_slots;                  // number of renaming-slots for dynamically updated buffers
    int active_slot;                // currently active write-slot for dynamically updated buffers
} sg_buffer_info;

typedef struct sg_image_info {
    sg_slot_info slot;              // resource pool slot info
    uint32_t upd_frame_index;       // frame index of last sg_update_image()
    int num_slots;                  // number of renaming-slots for dynamically updated images
    int active_slot;                // currently active write-slot for dynamically updated images
} sg_image_info;

typedef struct sg_sampler_info {
    sg_slot_info slot;              // resource pool slot info
} sg_sampler_info;

typedef struct sg_shader_info {
    sg_slot_info slot;              // resource pool slot info
} sg_shader_info;

typedef struct sg_pipeline_info {
    sg_slot_info slot;              // resource pool slot info
} sg_pipeline_info;

typedef struct sg_attachments_info {
    sg_slot_info slot;              // resource pool slot info
} sg_attachments_info;

/*
    sg_frame_stats

    Allows to track generic and backend-specific stats about a
    render frame. Obtained by calling sg_query_frame_stats(). The returned
    struct contains information about the *previous* frame.
*/
typedef struct sg_frame_stats_gl {
    uint32_t num_bind_buffer;
    uint32_t num_active_texture;
    uint32_t num_bind_texture;
    uint32_t num_bind_sampler;
    uint32_t num_use_program;
    uint32_t num_render_state;
    uint32_t num_vertex_attrib_pointer;
    uint32_t num_vertex_attrib_divisor;
    uint32_t num_enable_vertex_attrib_array;
    uint32_t num_disable_vertex_attrib_array;
    uint32_t num_uniform;
} sg_frame_stats_gl;

typedef struct sg_frame_stats_d3d11_pass {
    uint32_t num_om_set_render_targets;
    uint32_t num_clear_render_target_view;
    uint32_t num_clear_depth_stencil_view;
    uint32_t num_resolve_subresource;
} sg_frame_stats_d3d11_pass;

typedef struct sg_frame_stats_d3d11_pipeline {
    uint32_t num_rs_set_state;
    uint32_t num_om_set_depth_stencil_state;
    uint32_t num_om_set_blend_state;
    uint32_t num_ia_set_primitive_topology;
    uint32_t num_ia_set_input_layout;
    uint32_t num_vs_set_shader;
    uint32_t num_vs_set_constant_buffers;
    uint32_t num_ps_set_shader;
    uint32_t num_ps_set_constant_buffers;
} sg_frame_stats_d3d11_pipeline;

typedef struct sg_frame_stats_d3d11_bindings {
    uint32_t num_ia_set_vertex_buffers;
    uint32_t num_ia_set_index_buffer;
    uint32_t num_vs_set_shader_resources;
    uint32_t num_ps_set_shader_resources;
    uint32_t num_vs_set_samplers;
    uint32_t num_ps_set_samplers;
} sg_frame_stats_d3d11_bindings;

typedef struct sg_frame_stats_d3d11_uniforms {
    uint32_t num_update_subresource;
} sg_frame_stats_d3d11_uniforms;

typedef struct sg_frame_stats_d3d11_draw {
    uint32_t num_draw_indexed_instanced;
    uint32_t num_draw_indexed;
    uint32_t num_draw_instanced;
    uint32_t num_draw;
} sg_frame_stats_d3d11_draw;

typedef struct sg_frame_stats_d3d11 {
    sg_frame_stats_d3d11_pass pass;
    sg_frame_stats_d3d11_pipeline pipeline;
    sg_frame_stats_d3d11_bindings bindings;
    sg_frame_stats_d3d11_uniforms uniforms;
    sg_frame_stats_d3d11_draw draw;
    uint32_t num_map;
    uint32_t num_unmap;
} sg_frame_stats_d3d11;

typedef struct sg_frame_stats_metal_idpool {
    uint32_t num_added;
    uint32_t num_released;
    uint32_t num_garbage_collected;
} sg_frame_stats_metal_idpool;

typedef struct sg_frame_stats_metal_pipeline {
    uint32_t num_set_blend_color;
    uint32_t num_set_cull_mode;
    uint32_t num_set_front_facing_winding;
    uint32_t num_set_stencil_reference_value;
    uint32_t num_set_depth_bias;
    uint32_t num_set_render_pipeline_state;
    uint32_t num_set_depth_stencil_state;
} sg_frame_stats_metal_pipeline;

typedef struct sg_frame_stats_metal_bindings {
    uint32_t num_set_vertex_buffer;
    uint32_t num_set_vertex_texture;
    uint32_t num_set_vertex_sampler_state;
    uint32_t num_set_fragment_buffer;
    uint32_t num_set_fragment_texture;
    uint32_t num_set_fragment_sampler_state;
} sg_frame_stats_metal_bindings;

typedef struct sg_frame_stats_metal_uniforms {
    uint32_t num_set_vertex_buffer_offset;
    uint32_t num_set_fragment_buffer_offset;
} sg_frame_stats_metal_uniforms;

typedef struct sg_frame_stats_metal {
    sg_frame_stats_metal_idpool idpool;
    sg_frame_stats_metal_pipeline pipeline;
    sg_frame_stats_metal_bindings bindings;
    sg_frame_stats_metal_uniforms uniforms;
} sg_frame_stats_metal;

typedef struct sg_frame_stats {
    uint32_t frame_index;   // current frame counter, starts at 0

    uint32_t num_passes;
    uint32_t num_apply_viewport;
    uint32_t num_apply_scissor_rect;
    uint32_t num_apply_pipeline;
    uint32_t num_apply_bindings;
    uint32_t num_apply_uniforms;
    uint32_t num_draw;
    uint32_t num_update_buffer;
    uint32_t num_append_buffer;
    uint32_t num_update_image;

    uint32_t size_apply_uniforms;
    uint32_t size_update_buffer;
    uint32_t size_append_buffer;
    uint32_t size_update_image;

    sg_frame_stats_gl gl;
    sg_frame_stats_d3d11 d3d11;
    sg_frame_stats_metal metal;
} sg_frame_stats;

/*
    sg_log_item

    An enum with a unique item for each log message, warning, error
    and validation layer message.
*/
#define _SG_LOG_ITEMS \
    _SG_LOGITEM_XMACRO(OK, "Ok") \
    _SG_LOGITEM_XMACRO(MALLOC_FAILED, "memory allocation failed") \
    _SG_LOGITEM_XMACRO(GL_TEXTURE_FORMAT_NOT_SUPPORTED, "pixel format not supported for texture (gl)") \
    _SG_LOGITEM_XMACRO(GL_3D_TEXTURES_NOT_SUPPORTED, "3d textures not supported (gl)") \
    _SG_LOGITEM_XMACRO(GL_ARRAY_TEXTURES_NOT_SUPPORTED, "array textures not supported (gl)") \
    _SG_LOGITEM_XMACRO(GL_SHADER_COMPILATION_FAILED, "shader compilation failed (gl)") \
    _SG_LOGITEM_XMACRO(GL_SHADER_LINKING_FAILED, "shader linking failed (gl)") \
    _SG_LOGITEM_XMACRO(GL_VERTEX_ATTRIBUTE_NOT_FOUND_IN_SHADER, "vertex attribute not found in shader (gl)") \
    _SG_LOGITEM_XMACRO(GL_TEXTURE_NAME_NOT_FOUND_IN_SHADER, "texture name not found in shader (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_UNDEFINED, "framebuffer completeness check failed with GL_FRAMEBUFFER_UNDEFINED (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_INCOMPLETE_ATTACHMENT, "framebuffer completeness check failed with GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MISSING_ATTACHMENT, "framebuffer completeness check failed with GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_UNSUPPORTED, "framebuffer completeness check failed with GL_FRAMEBUFFER_UNSUPPORTED (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MULTISAMPLE, "framebuffer completeness check failed with GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE (gl)") \
    _SG_LOGITEM_XMACRO(GL_FRAMEBUFFER_STATUS_UNKNOWN, "framebuffer completeness check failed (unknown reason) (gl)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_BUFFER_FAILED, "CreateBuffer() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_BUFFER_SRV_FAILED, "CreateShaderResourceView() failed for storage buffer (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_DEPTH_TEXTURE_UNSUPPORTED_PIXEL_FORMAT, "pixel format not supported for depth-stencil texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_DEPTH_TEXTURE_FAILED, "CreateTexture2D() failed for depth-stencil texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_2D_TEXTURE_UNSUPPORTED_PIXEL_FORMAT, "pixel format not supported for 2d-, cube- or array-texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_2D_TEXTURE_FAILED, "CreateTexture2D() failed for 2d-, cube- or array-texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_2D_SRV_FAILED, "CreateShaderResourceView() failed for 2d-, cube- or array-texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_3D_TEXTURE_UNSUPPORTED_PIXEL_FORMAT, "pixel format not supported for 3D texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_3D_TEXTURE_FAILED, "CreateTexture3D() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_3D_SRV_FAILED, "CreateShaderResourceView() failed for 3d texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_MSAA_TEXTURE_FAILED, "CreateTexture2D() failed for MSAA render target texture (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_SAMPLER_STATE_FAILED, "CreateSamplerState() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_LOAD_D3DCOMPILER_47_DLL_FAILED, "loading d3dcompiler_47.dll failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_SHADER_COMPILATION_FAILED, "shader compilation failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_SHADER_COMPILATION_OUTPUT, "") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_CONSTANT_BUFFER_FAILED, "CreateBuffer() failed for uniform constant buffer (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_INPUT_LAYOUT_FAILED, "CreateInputLayout() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_RASTERIZER_STATE_FAILED, "CreateRasterizerState() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_DEPTH_STENCIL_STATE_FAILED, "CreateDepthStencilState() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_BLEND_STATE_FAILED, "CreateBlendState() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_RTV_FAILED, "CreateRenderTargetView() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_CREATE_DSV_FAILED, "CreateDepthStencilView() failed (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_MAP_FOR_UPDATE_BUFFER_FAILED, "Map() failed when updating buffer (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_MAP_FOR_APPEND_BUFFER_FAILED, "Map() failed when appending to buffer (d3d11)") \
    _SG_LOGITEM_XMACRO(D3D11_MAP_FOR_UPDATE_IMAGE_FAILED, "Map() failed when updating image (d3d11)") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_BUFFER_FAILED, "failed to create buffer object (metal)") \
    _SG_LOGITEM_XMACRO(METAL_TEXTURE_FORMAT_NOT_SUPPORTED, "pixel format not supported for texture (metal)") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_TEXTURE_FAILED, "failed to create texture object (metal)") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_SAMPLER_FAILED, "failed to create sampler object (metal)") \
    _SG_LOGITEM_XMACRO(METAL_SHADER_COMPILATION_FAILED, "shader compilation failed (metal)") \
    _SG_LOGITEM_XMACRO(METAL_SHADER_CREATION_FAILED, "shader creation failed (metal)") \
    _SG_LOGITEM_XMACRO(METAL_SHADER_COMPILATION_OUTPUT, "") \
    _SG_LOGITEM_XMACRO(METAL_VERTEX_SHADER_ENTRY_NOT_FOUND, "vertex shader entry function not found (metal)") \
    _SG_LOGITEM_XMACRO(METAL_FRAGMENT_SHADER_ENTRY_NOT_FOUND, "fragment shader entry not found (metal)") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_RPS_FAILED, "failed to create render pipeline state (metal)") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_RPS_OUTPUT, "") \
    _SG_LOGITEM_XMACRO(METAL_CREATE_DSS_FAILED, "failed to create depth stencil state (metal)") \
    _SG_LOGITEM_XMACRO(IDENTICAL_COMMIT_LISTENER, "attempting to add identical commit listener") \
    _SG_LOGITEM_XMACRO(COMMIT_LISTENER_ARRAY_FULL, "commit listener array full") \
    _SG_LOGITEM_XMACRO(TRACE_HOOKS_NOT_ENABLED, "sg_install_trace_hooks() called, but SG_TRACE_HOOKS is not defined") \
    _SG_LOGITEM_XMACRO(DEALLOC_BUFFER_INVALID_STATE, "sg_dealloc_buffer(): buffer must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(DEALLOC_IMAGE_INVALID_STATE, "sg_dealloc_image(): image must be in alloc state") \
    _SG_LOGITEM_XMACRO(DEALLOC_SAMPLER_INVALID_STATE, "sg_dealloc_sampler(): sampler must be in alloc state") \
    _SG_LOGITEM_XMACRO(DEALLOC_SHADER_INVALID_STATE, "sg_dealloc_shader(): shader must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(DEALLOC_PIPELINE_INVALID_STATE, "sg_dealloc_pipeline(): pipeline must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(DEALLOC_ATTACHMENTS_INVALID_STATE, "sg_dealloc_attachments(): attachments must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_BUFFER_INVALID_STATE, "sg_init_buffer(): buffer must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_IMAGE_INVALID_STATE, "sg_init_image(): image must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_SAMPLER_INVALID_STATE, "sg_init_sampler(): sampler must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_SHADER_INVALID_STATE, "sg_init_shader(): shader must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_PIPELINE_INVALID_STATE, "sg_init_pipeline(): pipeline must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(INIT_ATTACHMENTS_INVALID_STATE, "sg_init_attachments(): pass must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(UNINIT_BUFFER_INVALID_STATE, "sg_uninit_buffer(): buffer must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(UNINIT_IMAGE_INVALID_STATE, "sg_uninit_image(): image must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(UNINIT_SAMPLER_INVALID_STATE, "sg_uninit_sampler(): sampler must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(UNINIT_SHADER_INVALID_STATE, "sg_uninit_shader(): shader must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(UNINIT_PIPELINE_INVALID_STATE, "sg_uninit_pipeline(): pipeline must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(UNINIT_ATTACHMENTS_INVALID_STATE, "sg_uninit_attachments(): attachments must be in VALID or FAILED state") \
    _SG_LOGITEM_XMACRO(FAIL_BUFFER_INVALID_STATE, "sg_fail_buffer(): buffer must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(FAIL_IMAGE_INVALID_STATE, "sg_fail_image(): image must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(FAIL_SAMPLER_INVALID_STATE, "sg_fail_sampler(): sampler must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(FAIL_SHADER_INVALID_STATE, "sg_fail_shader(): shader must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(FAIL_PIPELINE_INVALID_STATE, "sg_fail_pipeline(): pipeline must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(FAIL_ATTACHMENTS_INVALID_STATE, "sg_fail_attachments(): attachments must be in ALLOC state") \
    _SG_LOGITEM_XMACRO(BUFFER_POOL_EXHAUSTED, "buffer pool exhausted") \
    _SG_LOGITEM_XMACRO(IMAGE_POOL_EXHAUSTED, "image pool exhausted") \
    _SG_LOGITEM_XMACRO(SAMPLER_POOL_EXHAUSTED, "sampler pool exhausted") \
    _SG_LOGITEM_XMACRO(SHADER_POOL_EXHAUSTED, "shader pool exhausted") \
    _SG_LOGITEM_XMACRO(PIPELINE_POOL_EXHAUSTED, "pipeline pool exhausted") \
    _SG_LOGITEM_XMACRO(PASS_POOL_EXHAUSTED, "pass pool exhausted") \
    _SG_LOGITEM_XMACRO(BEGINPASS_ATTACHMENT_INVALID, "sg_begin_pass: an attachment was provided that no longer exists") \
    _SG_LOGITEM_XMACRO(DRAW_WITHOUT_BINDINGS, "attempting to draw without resource bindings") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_CANARY, "sg_buffer_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_SIZE, "sg_buffer_desc.size and .data.size cannot both be 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_DATA, "immutable buffers must be initialized with data (sg_buffer_desc.data.ptr and sg_buffer_desc.data.size)") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_DATA_SIZE, "immutable buffer data size differs from buffer size") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_NO_DATA, "dynamic/stream usage buffers cannot be initialized with data") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_STORAGEBUFFER_SUPPORTED, "storage buffers not supported by the backend 3D API (requires OpenGL >= 4.3)") \
    _SG_LOGITEM_XMACRO(VALIDATE_BUFFERDESC_STORAGEBUFFER_SIZE_MULTIPLE_4, "size of storage buffers must be a multiple of 4") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDATA_NODATA, "sg_image_data: no data (.ptr and/or .size is zero)") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDATA_DATA_SIZE, "sg_image_data: data size doesn't match expected surface size") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_CANARY, "sg_image_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_WIDTH, "sg_image_desc.width must be > 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_HEIGHT, "sg_image_desc.height must be > 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_RT_PIXELFORMAT, "invalid pixel format for render-target image") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_NONRT_PIXELFORMAT, "invalid pixel format for non-render-target image") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_MSAA_BUT_NO_RT, "non-render-target images cannot be multisampled") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_NO_MSAA_RT_SUPPORT, "MSAA not supported for this pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_MSAA_NUM_MIPMAPS, "MSAA images must have num_mipmaps == 1") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_MSAA_3D_IMAGE, "3D images cannot have a sample_count > 1") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_DEPTH_3D_IMAGE, "3D images cannot have a depth/stencil image format") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_RT_IMMUTABLE, "render target images must be SG_USAGE_IMMUTABLE") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_RT_NO_DATA, "render target images cannot be initialized with data") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_INJECTED_NO_DATA, "images with injected textures cannot be initialized with data") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_DYNAMIC_NO_DATA, "dynamic/stream images cannot be initialized with data") \
    _SG_LOGITEM_XMACRO(VALIDATE_IMAGEDESC_COMPRESSED_IMMUTABLE, "compressed images must be immutable") \
    _SG_LOGITEM_XMACRO(VALIDATE_SAMPLERDESC_CANARY, "sg_sampler_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_SAMPLERDESC_MINFILTER_NONE, "sg_sampler_desc.min_filter cannot be SG_FILTER_NONE") \
    _SG_LOGITEM_XMACRO(VALIDATE_SAMPLERDESC_MAGFILTER_NONE, "sg_sampler_desc.mag_filter cannot be SG_FILTER_NONE") \
    _SG_LOGITEM_XMACRO(VALIDATE_SAMPLERDESC_ANISTROPIC_REQUIRES_LINEAR_FILTERING, "sg_sampler_desc.max_anisotropy > 1 requires min/mag/mipmap_filter to be SG_FILTER_LINEAR") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_CANARY, "sg_shader_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_SOURCE, "shader source code required") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_BYTECODE, "shader byte code required") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_SOURCE_OR_BYTECODE, "shader source or byte code required") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_BYTECODE_SIZE, "shader byte code length (in bytes) required") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_UBS, "shader uniform blocks must occupy continuous slots") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_UB_MEMBERS, "uniform block members must occupy continuous slots") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_UB_MEMBERS, "GL backend requires uniform block member declarations") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_UB_MEMBER_NAME, "uniform block member name missing") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_UB_SIZE_MISMATCH, "size of uniform block members doesn't match uniform block size") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_UB_ARRAY_COUNT, "uniform array count must be >= 1") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_UB_STD140_ARRAY_TYPE, "uniform arrays only allowed for FLOAT4, INT4, MAT4 in std140 layout") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_STORAGEBUFFERS, "shader stage storage buffers must occupy continuous slots (sg_shader_desc.vs|fs.storage_buffers[])") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_STORAGEBUFFER_READONLY, "shader stage storage buffers must be readonly (sg_shader_desc.vs|fs.storage_buffers[].readonly)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_IMAGES, "shader stage images must occupy continuous slots (sg_shader_desc.vs|fs.images[])") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_SAMPLERS, "shader stage samplers must occupy continuous slots (sg_shader_desc.vs|fs.samplers[])") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_IMAGE_SLOT_OUT_OF_RANGE, "shader stage: image-sampler-pair image slot index is out of range (sg_shader_desc.vs|fs.image_sampler_pairs[].image_slot)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_SAMPLER_SLOT_OUT_OF_RANGE, "shader stage: image-sampler-pair image slot index is out of range (sg_shader_desc.vs|fs.image_sampler_pairs[].sampler_slot)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_NAME_REQUIRED_FOR_GL, "shader stage: image-sampler-pairs must be named in GL (sg_shader_desc.vs|fs.image_sampler_pairs[].name)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_NAME_BUT_NOT_USED, "shader stage: image-sampler-pair has name but .used field not true") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_IMAGE_BUT_NOT_USED, "shader stage: image-sampler-pair has .image_slot != 0 but .used field not true") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_SAMPLER_PAIR_HAS_SAMPLER_BUT_NOT_USED, "shader stage: image-sampler-pair .sampler_slot != 0 but .used field not true") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NONFILTERING_SAMPLER_REQUIRED, "shader stage: image sample type UNFILTERABLE_FLOAT, UINT, SINT can only be used with NONFILTERING sampler") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_COMPARISON_SAMPLER_REQUIRED, "shader stage: image sample type DEPTH can only be used with COMPARISON sampler") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_IMAGE_NOT_REFERENCED_BY_IMAGE_SAMPLER_PAIRS, "shader stage: one or more images are note referenced by  (sg_shader_desc.vs|fs.image_sampler_pairs[].image_slot)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_SAMPLER_NOT_REFERENCED_BY_IMAGE_SAMPLER_PAIRS, "shader stage: one or more samplers are not referenced by image-sampler-pairs (sg_shader_desc.vs|fs.image_sampler_pairs[].sampler_slot)") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_NO_CONT_IMAGE_SAMPLER_PAIRS, "shader stage image-sampler-pairs must occupy continuous slots (sg_shader_desc.vs|fs.image_samplers[])") \
    _SG_LOGITEM_XMACRO(VALIDATE_SHADERDESC_ATTR_STRING_TOO_LONG, "vertex attribute name/semantic string too long (max len 16)") \
    _SG_LOGITEM_XMACRO(VALIDATE_PIPELINEDESC_CANARY, "sg_pipeline_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_PIPELINEDESC_SHADER, "sg_pipeline_desc.shader missing or invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_PIPELINEDESC_NO_CONT_ATTRS, "sg_pipeline_desc.layout.attrs is not continuous") \
    _SG_LOGITEM_XMACRO(VALIDATE_PIPELINEDESC_LAYOUT_STRIDE4, "sg_pipeline_desc.layout.buffers[].stride must be multiple of 4") \
    _SG_LOGITEM_XMACRO(VALIDATE_PIPELINEDESC_ATTR_SEMANTICS, "D3D11 missing vertex attribute semantics in shader") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_CANARY, "sg_attachments_desc not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_NO_ATTACHMENTS, "sg_attachments_desc no color or depth-stencil attachments") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_NO_CONT_COLOR_ATTS, "color attachments must occupy continuous slots") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_IMAGE, "pass attachment image is not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_MIPLEVEL, "pass attachment mip level is bigger than image has mipmaps") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_FACE, "pass attachment image is cubemap, but face index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_LAYER, "pass attachment image is array texture, but layer index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_SLICE, "pass attachment image is 3d texture, but slice value is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_IMAGE_NO_RT, "pass attachment image must be have render_target=true") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_COLOR_INV_PIXELFORMAT, "pass color-attachment images must be renderable color pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_INV_PIXELFORMAT, "pass depth-attachment image must be depth or depth-stencil pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_IMAGE_SIZES, "all pass attachments must have the same size") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_IMAGE_SAMPLE_COUNTS, "all pass attachments must have the same sample count") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_COLOR_IMAGE_MSAA, "pass resolve attachments must have a color attachment image with sample count > 1") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE, "pass resolve attachment image not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_SAMPLE_COUNT, "pass resolve attachment image sample count must be 1") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_MIPLEVEL, "pass resolve attachment mip level is bigger than image has mipmaps") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_FACE, "pass resolve attachment is cubemap, but face index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_LAYER, "pass resolve attachment is array texture, but layer index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_SLICE, "pass resolve attachment is 3d texture, but slice value is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_NO_RT, "pass resolve attachment image must have render_target=true") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_SIZES, "pass resolve attachment size must match color attachment image size") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_RESOLVE_IMAGE_FORMAT, "pass resolve attachment pixel format must match color attachment pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE, "pass depth attachment image is not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_MIPLEVEL, "pass depth attachment mip level is bigger than image has mipmaps") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_FACE, "pass depth attachment image is cubemap, but face index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_LAYER, "pass depth attachment image is array texture, but layer index is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_SLICE, "pass depth attachment image is 3d texture, but slice value is too big") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_NO_RT, "pass depth attachment image must be have render_target=true") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_SIZES, "pass depth attachment image size must match color attachment image size") \
    _SG_LOGITEM_XMACRO(VALIDATE_ATTACHMENTSDESC_DEPTH_IMAGE_SAMPLE_COUNT, "pass depth attachment sample count must match color attachment sample count") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_CANARY, "sg_begin_pass: pass struct not initialized") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_ATTACHMENTS_EXISTS, "sg_begin_pass: attachments object no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_ATTACHMENTS_VALID, "sg_begin_pass: attachments object not in resource state VALID") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_COLOR_ATTACHMENT_IMAGE, "sg_begin_pass: one or more color attachment images are not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_RESOLVE_ATTACHMENT_IMAGE, "sg_begin_pass: one or more resolve attachment images are not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_DEPTHSTENCIL_ATTACHMENT_IMAGE, "sg_begin_pass: one or more depth-stencil attachment images are not valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_WIDTH, "sg_begin_pass: expected pass.swapchain.width > 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_WIDTH_NOTSET, "sg_begin_pass: expected pass.swapchain.width == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_HEIGHT, "sg_begin_pass: expected pass.swapchain.height > 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_HEIGHT_NOTSET, "sg_begin_pass: expected pass.swapchain.height == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_SAMPLECOUNT, "sg_begin_pass: expected pass.swapchain.sample_count > 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_SAMPLECOUNT_NOTSET, "sg_begin_pass: expected pass.swapchain.sample_count == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_COLORFORMAT, "sg_begin_pass: expected pass.swapchain.color_format to be valid") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_COLORFORMAT_NOTSET, "sg_begin_pass: expected pass.swapchain.color_format to be unset") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_EXPECT_DEPTHFORMAT_NOTSET, "sg_begin_pass: expected pass.swapchain.depth_format to be unset") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_CURRENTDRAWABLE, "sg_begin_pass: expected pass.swapchain.metal.current_drawable != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_CURRENTDRAWABLE_NOTSET, "sg_begin_pass: expected pass.swapchain.metal.current_drawable == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_DEPTHSTENCILTEXTURE, "sg_begin_pass: expected pass.swapchain.metal.depth_stencil_texture != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_DEPTHSTENCILTEXTURE_NOTSET, "sg_begin_pass: expected pass.swapchain.metal.depth_stencil_texture == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_MSAACOLORTEXTURE, "sg_begin_pass: expected pass.swapchain.metal.msaa_color_texture != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_METAL_EXPECT_MSAACOLORTEXTURE_NOTSET, "sg_begin_pass: expected pass.swapchain.metal.msaa_color_texture == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RENDERVIEW, "sg_begin_pass: expected pass.swapchain.d3d11.render_view != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RENDERVIEW_NOTSET, "sg_begin_pass: expected pass.swapchain.d3d11.render_view == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RESOLVEVIEW, "sg_begin_pass: expected pass.swapchain.d3d11.resolve_view != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_RESOLVEVIEW_NOTSET, "sg_begin_pass: expected pass.swapchain.d3d11.resolve_view == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_DEPTHSTENCILVIEW, "sg_begin_pass: expected pass.swapchain.d3d11.depth_stencil_view != 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_D3D11_EXPECT_DEPTHSTENCILVIEW_NOTSET, "sg_begin_pass: expected pass.swapchain.d3d11.depth_stencil_view == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_BEGINPASS_SWAPCHAIN_GL_EXPECT_FRAMEBUFFER_NOTSET, "sg_begin_pass: expected pass.swapchain.gl.framebuffer == 0") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_PIPELINE_VALID_ID, "sg_apply_pipeline: invalid pipeline id provided") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_PIPELINE_EXISTS, "sg_apply_pipeline: pipeline object no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_PIPELINE_VALID, "sg_apply_pipeline: pipeline object not in valid state") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_SHADER_EXISTS, "sg_apply_pipeline: shader object no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_SHADER_VALID, "sg_apply_pipeline: shader object not in valid state") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_CURPASS_ATTACHMENTS_EXISTS, "sg_apply_pipeline: current pass attachments no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_CURPASS_ATTACHMENTS_VALID, "sg_apply_pipeline: current pass attachments not in valid state") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_ATT_COUNT, "sg_apply_pipeline: number of pipeline color attachments doesn't match number of pass color attachments") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_COLOR_FORMAT, "sg_apply_pipeline: pipeline color attachment pixel format doesn't match pass color attachment pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_DEPTH_FORMAT, "sg_apply_pipeline: pipeline depth pixel_format doesn't match pass depth attachment pixel format") \
    _SG_LOGITEM_XMACRO(VALIDATE_APIP_SAMPLE_COUNT, "sg_apply_pipeline: pipeline MSAA sample count doesn't match render pass attachment sample count") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_PIPELINE, "sg_apply_bindings: must be called after sg_apply_pipeline") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_PIPELINE_EXISTS, "sg_apply_bindings: currently applied pipeline object no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_PIPELINE_VALID, "sg_apply_bindings: currently applied pipeline object not in valid state") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VBS, "sg_apply_bindings: number of vertex buffers doesn't match number of pipeline vertex layouts") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VB_EXISTS, "sg_apply_bindings: vertex buffer no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VB_TYPE, "sg_apply_bindings: buffer in vertex buffer slot is not a SG_BUFFERTYPE_VERTEXBUFFER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VB_OVERFLOW, "sg_apply_bindings: buffer in vertex buffer slot is overflown") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_NO_IB, "sg_apply_bindings: pipeline object defines indexed rendering, but no index buffer provided") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_IB, "sg_apply_bindings: pipeline object defines non-indexed rendering, but index buffer provided") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_IB_EXISTS, "sg_apply_bindings: index buffer no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_IB_TYPE, "sg_apply_bindings: buffer in index buffer slot is not a SG_BUFFERTYPE_INDEXBUFFER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_IB_OVERFLOW, "sg_apply_bindings: buffer in index buffer slot is overflown") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_IMAGE_BINDING, "sg_apply_bindings: image binding on vertex stage is missing or the image handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_IMG_EXISTS, "sg_apply_bindings: image bound to vertex stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_IMAGE_TYPE_MISMATCH, "sg_apply_bindings: type of image bound to vertex stage doesn't match shader desc") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_IMAGE_MSAA, "sg_apply_bindings: cannot bind image with sample_count>1 to vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_FILTERABLE_IMAGE, "sg_apply_bindings: filterable image expected on vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_DEPTH_IMAGE, "sg_apply_bindings: depth image expected on vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_UNEXPECTED_IMAGE_BINDING, "sg_apply_bindings: unexpected image binding on vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_SAMPLER_BINDING, "sg_apply_bindings: sampler binding on vertex stage is missing or the sampler handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_UNEXPECTED_SAMPLER_COMPARE_NEVER, "sg_apply_bindings: shader expects SG_SAMPLERTYPE_COMPARISON on vertex stage but sampler has SG_COMPAREFUNC_NEVER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_SAMPLER_COMPARE_NEVER, "sg_apply_bindings: shader expects SG_SAMPLERTYPE_FILTERING or SG_SAMPLERTYPE_NONFILTERING on vertex stage but sampler doesn't have SG_COMPAREFUNC_NEVER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_NONFILTERING_SAMPLER, "sg_apply_bindings: shader expected SG_SAMPLERTYPE_NONFILTERING on vertex stage, but sampler has SG_FILTER_LINEAR filters") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_UNEXPECTED_SAMPLER_BINDING, "sg_apply_bindings: unexpected sampler binding on vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_SMP_EXISTS, "sg_apply_bindings: sampler bound to vertex stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_EXPECTED_STORAGEBUFFER_BINDING, "sg_apply_bindings: storage buffer binding on vertex stage is missing or the buffer handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_STORAGEBUFFER_EXISTS, "sg_apply_bindings: storage buffer bound to vertex stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_STORAGEBUFFER_BINDING_BUFFERTYPE, "sg_apply_bindings: buffer bound to vertex stage storage buffer slot is not of type storage buffer") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_VS_UNEXPECTED_STORAGEBUFFER_BINDING, "sg_apply_bindings: unexpected storage buffer binding on vertex stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_IMAGE_BINDING, "sg_apply_bindings: image binding on fragment stage is missing or the image handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_IMG_EXISTS, "sg_apply_bindings: image bound to fragment stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_IMAGE_TYPE_MISMATCH, "sg_apply_bindings: type of image bound to fragment stage doesn't match shader desc") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_IMAGE_MSAA, "sg_apply_bindings: cannot bind image with sample_count>1 to fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_FILTERABLE_IMAGE, "sg_apply_bindings: filterable image expected on fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_DEPTH_IMAGE, "sg_apply_bindings: depth image expected on fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_UNEXPECTED_IMAGE_BINDING, "sg_apply_bindings: unexpected image binding on fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_SAMPLER_BINDING, "sg_apply_bindings: sampler binding on fragment stage is missing or the sampler handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_UNEXPECTED_SAMPLER_COMPARE_NEVER, "sg_apply_bindings: shader expects SG_SAMPLERTYPE_COMPARISON on fragment stage but sampler has SG_COMPAREFUNC_NEVER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_SAMPLER_COMPARE_NEVER, "sg_apply_bindings: shader expects SG_SAMPLERTYPE_FILTERING on fragment stage but sampler doesn't have SG_COMPAREFUNC_NEVER") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_NONFILTERING_SAMPLER, "sg_apply_bindings: shader expected SG_SAMPLERTYPE_NONFILTERING on fragment stage, but sampler has SG_FILTER_LINEAR filters") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_UNEXPECTED_SAMPLER_BINDING, "sg_apply_bindings: unexpected sampler binding on fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_SMP_EXISTS, "sg_apply_bindings: sampler bound to fragment stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_EXPECTED_STORAGEBUFFER_BINDING, "sg_apply_bindings: storage buffer binding on fragment stage is missing or the buffer handle is invalid") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_STORAGEBUFFER_EXISTS, "sg_apply_bindings: storage buffer bound to fragment stage no longer alive") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_STORAGEBUFFER_BINDING_BUFFERTYPE, "sg_apply_bindings: buffer bound to frahment stage storage buffer slot is not of type storage buffer") \
    _SG_LOGITEM_XMACRO(VALIDATE_ABND_FS_UNEXPECTED_STORAGEBUFFER_BINDING, "sg_apply_bindings: unexpected storage buffer binding on fragment stage") \
    _SG_LOGITEM_XMACRO(VALIDATE_AUB_NO_PIPELINE, "sg_apply_uniforms: must be called after sg_apply_pipeline()") \
    _SG_LOGITEM_XMACRO(VALIDATE_AUB_NO_UB_AT_SLOT, "sg_apply_uniforms: no uniform block declaration at this shader stage UB slot") \
    _SG_LOGITEM_XMACRO(VALIDATE_AUB_SIZE, "sg_apply_uniforms: data size doesn't match declared uniform block size") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDATEBUF_USAGE, "sg_update_buffer: cannot update immutable buffer") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDATEBUF_SIZE, "sg_update_buffer: update size is bigger than buffer size") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDATEBUF_ONCE, "sg_update_buffer: only one update allowed per buffer and frame") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDATEBUF_APPEND, "sg_update_buffer: cannot call sg_update_buffer and sg_append_buffer in same frame") \
    _SG_LOGITEM_XMACRO(VALIDATE_APPENDBUF_USAGE, "sg_append_buffer: cannot append to immutable buffer") \
    _SG_LOGITEM_XMACRO(VALIDATE_APPENDBUF_SIZE, "sg_append_buffer: overall appended size is bigger than buffer size") \
    _SG_LOGITEM_XMACRO(VALIDATE_APPENDBUF_UPDATE, "sg_append_buffer: cannot call sg_append_buffer and sg_update_buffer in same frame") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDIMG_USAGE, "sg_update_image: cannot update immutable image") \
    _SG_LOGITEM_XMACRO(VALIDATE_UPDIMG_ONCE, "sg_update_image: only one update allowed per image and frame") \
    _SG_LOGITEM_XMACRO(VALIDATION_FAILED, "validation layer checks failed") \

#define _SG_LOGITEM_XMACRO(item,msg) SG_LOGITEM_##item,
typedef enum sg_log_item {
    _SG_LOG_ITEMS
} sg_log_item;
#undef _SG_LOGITEM_XMACRO

/*
    sg_desc

    The sg_desc struct contains configuration values for sokol_gfx,
    it is used as parameter to the sg_setup() call.

    The default configuration is:

    .buffer_pool_size       128
    .image_pool_size        128
    .sampler_pool_size      64
    .shader_pool_size       32
    .pipeline_pool_size     64
    .pass_pool_size         16
    .uniform_buffer_size    4 MB (4*1024*1024)
    .max_commit_listeners   1024
    .disable_validation     false
    .mtl_force_managed_storage_mode false
    .wgpu_disable_bindgroups_cache  false
    .wgpu_bindgroups_cache_size     1024

    .allocator.alloc_fn     0 (in this case, malloc() will be called)
    .allocator.free_fn      0 (in this case, free() will be called)
    .allocator.user_data    0

    .environment.defaults.color_format: default value depends on selected backend:
        all GL backends:    SG_PIXELFORMAT_RGBA8
        Metal and D3D11:    SG_PIXELFORMAT_BGRA8
        WebGPU:             *no default* (must be queried from WebGPU swapchain object)
    .environment.defaults.depth_format: SG_PIXELFORMAT_DEPTH_STENCIL
    .environment.defaults.sample_count: 1

    Metal specific:
        (NOTE: All Objective-C object references are transferred through
        a bridged (const void*) to sokol_gfx, which will use a unretained
        bridged cast (__bridged id<xxx>) to retrieve the Objective-C
        references back. Since the bridge cast is unretained, the caller
        must hold a strong reference to the Objective-C object for the
        duration of the sokol_gfx call!

        .mtl_force_managed_storage_mode
            when enabled, Metal buffers and texture resources are created in managed storage
            mode, otherwise sokol-gfx will decide whether to create buffers and
            textures in managed or shared storage mode (this is mainly a debugging option)
        .mtl_use_command_buffer_with_retained_references
            when true, the sokol-gfx Metal backend will use Metal command buffers which
            bump the reference count of resource objects as long as they are inflight,
            this is slower than the default command-buffer-with-unretained-references
            method, this may be a workaround when confronted with lifetime validation
            errors from the Metal validation layer until a proper fix has been implemented
        .environment.metal.device
            a pointer to the MTLDevice object

    D3D11 specific:
        .environment.d3d11.device
            a pointer to the ID3D11Device object, this must have been created
            before sg_setup() is called
        .environment.d3d11.device_context
            a pointer to the ID3D11DeviceContext object

    WebGPU specific:
        .wgpu_disable_bindgroups_cache
            When this is true, the WebGPU backend will create and immediately
            release a BindGroup object in the sg_apply_bindings() call, only
            use this for debugging purposes.
        .wgpu_bindgroups_cache_size
            The size of the bindgroups cache for re-using BindGroup objects
            between sg_apply_bindings() calls. The smaller the cache size,
            the more likely are cache slot collisions which will cause
            a BindGroups object to be destroyed and a new one created.
            Use the information returned by sg_query_stats() to check
            if this is a frequent occurrence, and increase the cache size as
            needed (the default is 1024).
            NOTE: wgpu_bindgroups_cache_size must be a power-of-2 number!
        .environment.wgpu.device
            a WGPUDevice handle

    When using sokol_gfx.h and sokol_app.h together, consider using the
    helper function sglue_environment() in the sokol_glue.h header to
    initialize the sg_desc.environment nested struct. sglue_environment() returns
    a completely initialized sg_environment struct with information
    provided by sokol_app.h.
*/
typedef struct sg_environment_defaults {
    sg_pixel_format color_format;
    sg_pixel_format depth_format;
    int sample_count;
} sg_environment_defaults;

typedef struct sg_metal_environment {
    const void* device;
} sg_metal_environment;

typedef struct sg_d3d11_environment {
    const void* device;
    const void* device_context;
} sg_d3d11_environment;

typedef struct sg_environment {
    sg_environment_defaults defaults;
    sg_metal_environment metal;
    sg_d3d11_environment d3d11;
} sg_environment;

/*
    sg_commit_listener

    Used with function sg_add_commit_listener() to add a callback
    which will be called in sg_commit(). This is useful for libraries
    building on top of sokol-gfx to be notified about when a frame
    ends (instead of having to guess, or add a manual 'new-frame'
    function.
*/
typedef struct sg_commit_listener {
    void (*func)(void* user_data);
    void* user_data;
} sg_commit_listener;

/*
    sg_allocator

    Used in sg_desc to provide custom memory-alloc and -free functions
    to sokol_gfx.h. If memory management should be overridden, both the
    alloc_fn and free_fn function must be provided (e.g. it's not valid to
    override one function but not the other).
*/
typedef struct sg_allocator {
    void* (*alloc_fn)(size_t size, void* user_data);
    void (*free_fn)(void* ptr, void* user_data);
    void* user_data;
} sg_allocator;

/*
    sg_logger

    Used in sg_desc to provide a logging function. Please be aware
    that without logging function, sokol-gfx will be completely
    silent, e.g. it will not report errors, warnings and
    validation layer messages. For maximum error verbosity,
    compile in debug mode (e.g. NDEBUG *not* defined) and provide a
    compatible logger function in the sg_setup() call
    (for instance the standard logging function from sokol_log.h).
*/
typedef struct sg_logger {
    void (*func)(
        const char* tag,                // always "sg"
        uint32_t log_level,             // 0=panic, 1=error, 2=warning, 3=info
        uint32_t log_item_id,           // SG_LOGITEM_*
        const char* message_or_null,    // a message string, may be nullptr in release mode
        uint32_t line_nr,               // line number in sokol_gfx.h
        const char* filename_or_null,   // source filename, may be nullptr in release mode
        void* user_data);
    void* user_data;
} sg_logger;

typedef struct sg_desc {
    uint32_t _start_canary;
    int buffer_pool_size;
    int image_pool_size;
    int sampler_pool_size;
    int shader_pool_size;
    int pipeline_pool_size;
    int attachments_pool_size;
    int uniform_buffer_size;
    int max_commit_listeners;
    bool disable_validation;    // disable validation layer even in debug mode, useful for tests
    bool mtl_force_managed_storage_mode; // for debugging: use Metal managed storage mode for resources even with UMA
    bool mtl_use_command_buffer_with_retained_references;    // Metal: use a managed MTLCommandBuffer which ref-counts used resources
    sg_allocator allocator;
    sg_logger logger; // optional log function override
    sg_environment environment;
    uint32_t _end_canary;
} sg_desc;

// setup and misc functions
SOKOL_GFX_API_DECL void sg_setup(const sg_desc* desc);
SOKOL_GFX_API_DECL void sg_shutdown(void);
SOKOL_GFX_API_DECL bool sg_isvalid(void);
SOKOL_GFX_API_DECL void sg_reset_state_cache(void);
SOKOL_GFX_API_DECL sg_trace_hooks sg_install_trace_hooks(const sg_trace_hooks* trace_hooks);
SOKOL_GFX_API_DECL void sg_push_debug_group(const char* name);
SOKOL_GFX_API_DECL void sg_pop_debug_group(void);
SOKOL_GFX_API_DECL bool sg_add_commit_listener(sg_commit_listener listener);
SOKOL_GFX_API_DECL bool sg_remove_commit_listener(sg_commit_listener listener);

// resource creation, destruction and updating
SOKOL_GFX_API_DECL sg_buffer sg_make_buffer(const sg_buffer_desc* desc);
SOKOL_GFX_API_DECL sg_image sg_make_image(const sg_image_desc* desc);
SOKOL_GFX_API_DECL sg_sampler sg_make_sampler(const sg_sampler_desc* desc);
SOKOL_GFX_API_DECL sg_shader sg_make_shader(const sg_shader_desc* desc);
SOKOL_GFX_API_DECL sg_pipeline sg_make_pipeline(const sg_pipeline_desc* desc);
SOKOL_GFX_API_DECL sg_attachments sg_make_attachments(const sg_attachments_desc* desc);
SOKOL_GFX_API_DECL void sg_destroy_buffer(sg_buffer buf);
SOKOL_GFX_API_DECL void sg_destroy_image(sg_image img);
SOKOL_GFX_API_DECL void sg_destroy_sampler(sg_sampler smp);
SOKOL_GFX_API_DECL void sg_destroy_shader(sg_shader shd);
SOKOL_GFX_API_DECL void sg_destroy_pipeline(sg_pipeline pip);
SOKOL_GFX_API_DECL void sg_destroy_attachments(sg_attachments atts);
SOKOL_GFX_API_DECL void sg_update_buffer(sg_buffer buf, const sg_range* data);
SOKOL_GFX_API_DECL void sg_update_image(sg_image img, const sg_image_data* data);
SOKOL_GFX_API_DECL int sg_append_buffer(sg_buffer buf, const sg_range* data);
SOKOL_GFX_API_DECL bool sg_query_buffer_overflow(sg_buffer buf);
SOKOL_GFX_API_DECL bool sg_query_buffer_will_overflow(sg_buffer buf, size_t size);

// rendering functions
SOKOL_GFX_API_DECL void sg_begin_pass(const sg_pass* pass);
SOKOL_GFX_API_DECL void sg_apply_viewport(int x, int y, int width, int height, bool origin_top_left);
SOKOL_GFX_API_DECL void sg_apply_viewportf(float x, float y, float width, float height, bool origin_top_left);
SOKOL_GFX_API_DECL void sg_apply_scissor_rect(int x, int y, int width, int height, bool origin_top_left);
SOKOL_GFX_API_DECL void sg_apply_scissor_rectf(float x, float y, float width, float height, bool origin_top_left);
SOKOL_GFX_API_DECL void sg_apply_pipeline(sg_pipeline pip);
SOKOL_GFX_API_DECL void sg_apply_bindings(const sg_bindings* bindings);
SOKOL_GFX_API_DECL void sg_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range* data);
SOKOL_GFX_API_DECL void sg_draw(int base_element, int num_elements, int num_instances);
SOKOL_GFX_API_DECL void sg_end_pass(void);
SOKOL_GFX_API_DECL void sg_commit(void);

// getting information
SOKOL_GFX_API_DECL sg_desc sg_query_desc(void);
SOKOL_GFX_API_DECL sg_backend sg_query_backend(void);
SOKOL_GFX_API_DECL sg_features sg_query_features(void);
SOKOL_GFX_API_DECL sg_limits sg_query_limits(void);
SOKOL_GFX_API_DECL sg_pixelformat_info sg_query_pixelformat(sg_pixel_format fmt);
SOKOL_GFX_API_DECL int sg_query_row_pitch(sg_pixel_format fmt, int width, int row_align_bytes);
SOKOL_GFX_API_DECL int sg_query_surface_pitch(sg_pixel_format fmt, int width, int height, int row_align_bytes);
// get current state of a resource (INITIAL, ALLOC, VALID, FAILED, INVALID)
SOKOL_GFX_API_DECL sg_resource_state sg_query_buffer_state(sg_buffer buf);
SOKOL_GFX_API_DECL sg_resource_state sg_query_image_state(sg_image img);
SOKOL_GFX_API_DECL sg_resource_state sg_query_sampler_state(sg_sampler smp);
SOKOL_GFX_API_DECL sg_resource_state sg_query_shader_state(sg_shader shd);
SOKOL_GFX_API_DECL sg_resource_state sg_query_pipeline_state(sg_pipeline pip);
SOKOL_GFX_API_DECL sg_resource_state sg_query_attachments_state(sg_attachments atts);
// get runtime information about a resource
SOKOL_GFX_API_DECL sg_buffer_info sg_query_buffer_info(sg_buffer buf);
SOKOL_GFX_API_DECL sg_image_info sg_query_image_info(sg_image img);
SOKOL_GFX_API_DECL sg_sampler_info sg_query_sampler_info(sg_sampler smp);
SOKOL_GFX_API_DECL sg_shader_info sg_query_shader_info(sg_shader shd);
SOKOL_GFX_API_DECL sg_pipeline_info sg_query_pipeline_info(sg_pipeline pip);
SOKOL_GFX_API_DECL sg_attachments_info sg_query_attachments_info(sg_attachments atts);
// get desc structs matching a specific resource (NOTE that not all creation attributes may be provided)
SOKOL_GFX_API_DECL sg_buffer_desc sg_query_buffer_desc(sg_buffer buf);
SOKOL_GFX_API_DECL sg_image_desc sg_query_image_desc(sg_image img);
SOKOL_GFX_API_DECL sg_sampler_desc sg_query_sampler_desc(sg_sampler smp);
SOKOL_GFX_API_DECL sg_shader_desc sg_query_shader_desc(sg_shader shd);
SOKOL_GFX_API_DECL sg_pipeline_desc sg_query_pipeline_desc(sg_pipeline pip);
SOKOL_GFX_API_DECL sg_attachments_desc sg_query_attachments_desc(sg_attachments atts);
// get resource creation desc struct with their default values replaced
SOKOL_GFX_API_DECL sg_buffer_desc sg_query_buffer_defaults(const sg_buffer_desc* desc);
SOKOL_GFX_API_DECL sg_image_desc sg_query_image_defaults(const sg_image_desc* desc);
SOKOL_GFX_API_DECL sg_sampler_desc sg_query_sampler_defaults(const sg_sampler_desc* desc);
SOKOL_GFX_API_DECL sg_shader_desc sg_query_shader_defaults(const sg_shader_desc* desc);
SOKOL_GFX_API_DECL sg_pipeline_desc sg_query_pipeline_defaults(const sg_pipeline_desc* desc);
SOKOL_GFX_API_DECL sg_attachments_desc sg_query_attachments_defaults(const sg_attachments_desc* desc);

// separate resource allocation and initialization (for async setup)
SOKOL_GFX_API_DECL sg_buffer sg_alloc_buffer(void);
SOKOL_GFX_API_DECL sg_image sg_alloc_image(void);
SOKOL_GFX_API_DECL sg_sampler sg_alloc_sampler(void);
SOKOL_GFX_API_DECL sg_shader sg_alloc_shader(void);
SOKOL_GFX_API_DECL sg_pipeline sg_alloc_pipeline(void);
SOKOL_GFX_API_DECL sg_attachments sg_alloc_attachments(void);
SOKOL_GFX_API_DECL void sg_dealloc_buffer(sg_buffer buf);
SOKOL_GFX_API_DECL void sg_dealloc_image(sg_image img);
SOKOL_GFX_API_DECL void sg_dealloc_sampler(sg_sampler smp);
SOKOL_GFX_API_DECL void sg_dealloc_shader(sg_shader shd);
SOKOL_GFX_API_DECL void sg_dealloc_pipeline(sg_pipeline pip);
SOKOL_GFX_API_DECL void sg_dealloc_attachments(sg_attachments attachments);
SOKOL_GFX_API_DECL void sg_init_buffer(sg_buffer buf, const sg_buffer_desc* desc);
SOKOL_GFX_API_DECL void sg_init_image(sg_image img, const sg_image_desc* desc);
SOKOL_GFX_API_DECL void sg_init_sampler(sg_sampler smg, const sg_sampler_desc* desc);
SOKOL_GFX_API_DECL void sg_init_shader(sg_shader shd, const sg_shader_desc* desc);
SOKOL_GFX_API_DECL void sg_init_pipeline(sg_pipeline pip, const sg_pipeline_desc* desc);
SOKOL_GFX_API_DECL void sg_init_attachments(sg_attachments attachments, const sg_attachments_desc* desc);
SOKOL_GFX_API_DECL void sg_uninit_buffer(sg_buffer buf);
SOKOL_GFX_API_DECL void sg_uninit_image(sg_image img);
SOKOL_GFX_API_DECL void sg_uninit_sampler(sg_sampler smp);
SOKOL_GFX_API_DECL void sg_uninit_shader(sg_shader shd);
SOKOL_GFX_API_DECL void sg_uninit_pipeline(sg_pipeline pip);
SOKOL_GFX_API_DECL void sg_uninit_attachments(sg_attachments atts);
SOKOL_GFX_API_DECL void sg_fail_buffer(sg_buffer buf);
SOKOL_GFX_API_DECL void sg_fail_image(sg_image img);
SOKOL_GFX_API_DECL void sg_fail_sampler(sg_sampler smp);
SOKOL_GFX_API_DECL void sg_fail_shader(sg_shader shd);
SOKOL_GFX_API_DECL void sg_fail_pipeline(sg_pipeline pip);
SOKOL_GFX_API_DECL void sg_fail_attachments(sg_attachments atts);

// frame stats
SOKOL_GFX_API_DECL void sg_enable_frame_stats(void);
SOKOL_GFX_API_DECL void sg_disable_frame_stats(void);
SOKOL_GFX_API_DECL bool sg_frame_stats_enabled(void);
SOKOL_GFX_API_DECL sg_frame_stats sg_query_frame_stats(void);

/* Backend-specific structs and functions, these may come in handy for mixing
   sokol-gfx rendering with 'native backend' rendering functions.

   This group of functions will be expanded as needed.
*/

typedef struct sg_d3d11_buffer_info {
    const void* buf;      // ID3D11Buffer*
} sg_d3d11_buffer_info;

typedef struct sg_d3d11_image_info {
    const void* tex2d;    // ID3D11Texture2D*
    const void* tex3d;    // ID3D11Texture3D*
    const void* res;      // ID3D11Resource* (either tex2d or tex3d)
    const void* srv;      // ID3D11ShaderResourceView*
} sg_d3d11_image_info;

typedef struct sg_d3d11_sampler_info {
    const void* smp;      // ID3D11SamplerState*
} sg_d3d11_sampler_info;

typedef struct sg_d3d11_shader_info {
    const void* vs_cbufs[SG_MAX_SHADERSTAGE_UBS]; // ID3D11Buffer* (vertex stage constant buffers)
    const void* fs_cbufs[SG_MAX_SHADERSTAGE_UBS]; // ID3D11Buffer* (fragment stage constant buffers)
    const void* vs;   // ID3D11VertexShader*
    const void* fs;   // ID3D11PixelShader*
} sg_d3d11_shader_info;

typedef struct sg_d3d11_pipeline_info {
    const void* il;   // ID3D11InputLayout*
    const void* rs;   // ID3D11RasterizerState*
    const void* dss;  // ID3D11DepthStencilState*
    const void* bs;   // ID3D11BlendState*
} sg_d3d11_pipeline_info;

typedef struct sg_d3d11_attachments_info {
    const void* color_rtv[SG_MAX_COLOR_ATTACHMENTS];      // ID3D11RenderTargetView
    const void* resolve_rtv[SG_MAX_COLOR_ATTACHMENTS];    // ID3D11RenderTargetView
    const void* dsv;  // ID3D11DepthStencilView
} sg_d3d11_attachments_info;

typedef struct sg_mtl_buffer_info {
    const void* buf[SG_NUM_INFLIGHT_FRAMES];  // id<MTLBuffer>
    int active_slot;
} sg_mtl_buffer_info;

typedef struct sg_mtl_image_info {
    const void* tex[SG_NUM_INFLIGHT_FRAMES]; // id<MTLTexture>
    int active_slot;
} sg_mtl_image_info;

typedef struct sg_mtl_sampler_info {
    const void* smp;  // id<MTLSamplerState>
} sg_mtl_sampler_info;

typedef struct sg_mtl_shader_info {
    const void* vs_lib;   // id<MTLLibrary>
    const void* fs_lib;   // id<MTLLibrary>
    const void* vs_func;  // id<MTLFunction>
    const void* fs_func;  // id<MTLFunction>
} sg_mtl_shader_info;

typedef struct sg_mtl_pipeline_info {
    const void* rps;      // id<MTLRenderPipelineState>
    const void* dss;      // id<MTLDepthStencilState>
} sg_mtl_pipeline_info;

typedef struct sg_gl_buffer_info {
    uint32_t buf[SG_NUM_INFLIGHT_FRAMES];
    int active_slot;
} sg_gl_buffer_info;

typedef struct sg_gl_image_info {
    uint32_t tex[SG_NUM_INFLIGHT_FRAMES];
    uint32_t tex_target;
    uint32_t msaa_render_buffer;
    int active_slot;
} sg_gl_image_info;

typedef struct sg_gl_sampler_info {
    uint32_t smp;
} sg_gl_sampler_info;

typedef struct sg_gl_shader_info {
    uint32_t prog;
} sg_gl_shader_info;

typedef struct sg_gl_attachments_info {
    uint32_t framebuffer;
    uint32_t msaa_resolve_framebuffer[SG_MAX_COLOR_ATTACHMENTS];
} sg_gl_attachments_info;

// D3D11: return ID3D11Device
SOKOL_GFX_API_DECL const void* sg_d3d11_device(void);
// D3D11: return ID3D11DeviceContext
SOKOL_GFX_API_DECL const void* sg_d3d11_device_context(void);
// D3D11: get internal buffer resource objects
SOKOL_GFX_API_DECL sg_d3d11_buffer_info sg_d3d11_query_buffer_info(sg_buffer buf);
// D3D11: get internal image resource objects
SOKOL_GFX_API_DECL sg_d3d11_image_info sg_d3d11_query_image_info(sg_image img);
// D3D11: get internal sampler resource objects
SOKOL_GFX_API_DECL sg_d3d11_sampler_info sg_d3d11_query_sampler_info(sg_sampler smp);
// D3D11: get internal shader resource objects
SOKOL_GFX_API_DECL sg_d3d11_shader_info sg_d3d11_query_shader_info(sg_shader shd);
// D3D11: get internal pipeline resource objects
SOKOL_GFX_API_DECL sg_d3d11_pipeline_info sg_d3d11_query_pipeline_info(sg_pipeline pip);
// D3D11: get internal pass resource objects
SOKOL_GFX_API_DECL sg_d3d11_attachments_info sg_d3d11_query_attachments_info(sg_attachments atts);

// Metal: return __bridge-casted MTLDevice
SOKOL_GFX_API_DECL const void* sg_mtl_device(void);
// Metal: return __bridge-casted MTLRenderCommandEncoder in current pass (or zero if outside pass)
SOKOL_GFX_API_DECL const void* sg_mtl_render_command_encoder(void);
// Metal: get internal __bridge-casted buffer resource objects
SOKOL_GFX_API_DECL sg_mtl_buffer_info sg_mtl_query_buffer_info(sg_buffer buf);
// Metal: get internal __bridge-casted image resource objects
SOKOL_GFX_API_DECL sg_mtl_image_info sg_mtl_query_image_info(sg_image img);
// Metal: get internal __bridge-casted sampler resource objects
SOKOL_GFX_API_DECL sg_mtl_sampler_info sg_mtl_query_sampler_info(sg_sampler smp);
// Metal: get internal __bridge-casted shader resource objects
SOKOL_GFX_API_DECL sg_mtl_shader_info sg_mtl_query_shader_info(sg_shader shd);
// Metal: get internal __bridge-casted pipeline resource objects
SOKOL_GFX_API_DECL sg_mtl_pipeline_info sg_mtl_query_pipeline_info(sg_pipeline pip);

// GL: get internal buffer resource objects
SOKOL_GFX_API_DECL sg_gl_buffer_info sg_gl_query_buffer_info(sg_buffer buf);
// GL: get internal image resource objects
SOKOL_GFX_API_DECL sg_gl_image_info sg_gl_query_image_info(sg_image img);
// GL: get internal sampler resource objects
SOKOL_GFX_API_DECL sg_gl_sampler_info sg_gl_query_sampler_info(sg_sampler smp);
// GL: get internal shader resource objects
SOKOL_GFX_API_DECL sg_gl_shader_info sg_gl_query_shader_info(sg_shader shd);
// GL: get internal pass resource objects
SOKOL_GFX_API_DECL sg_gl_attachments_info sg_gl_query_attachments_info(sg_attachments atts);

#endif // SOKOL_GFX_INCLUDED