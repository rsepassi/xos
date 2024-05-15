// ███    ███ ███████ ████████  █████  ██          ██████   █████   ██████ ██   ██ ███████ ███    ██ ██████
// ████  ████ ██         ██    ██   ██ ██          ██   ██ ██   ██ ██      ██  ██  ██      ████   ██ ██   ██
// ██ ████ ██ █████      ██    ███████ ██          ██████  ███████ ██      █████   █████   ██ ██  ██ ██   ██
// ██  ██  ██ ██         ██    ██   ██ ██          ██   ██ ██   ██ ██      ██  ██  ██      ██  ██ ██ ██   ██
// ██      ██ ███████    ██    ██   ██ ███████     ██████  ██   ██  ██████ ██   ██ ███████ ██   ████ ██████
//
// >>metal backend

#if __has_feature(objc_arc)
#define _SG_OBJC_RETAIN(obj) { }
#define _SG_OBJC_RELEASE(obj) { obj = nil; }
#else
#define _SG_OBJC_RETAIN(obj) { [obj retain]; }
#define _SG_OBJC_RELEASE(obj) { [obj release]; obj = nil; }
#endif

//-- enum translation functions ------------------------------------------------
_SOKOL_PRIVATE MTLLoadAction _sg_mtl_load_action(sg_load_action a) {
    switch (a) {
        case SG_LOADACTION_CLEAR:       return MTLLoadActionClear;
        case SG_LOADACTION_LOAD:        return MTLLoadActionLoad;
        case SG_LOADACTION_DONTCARE:    return MTLLoadActionDontCare;
        default: SOKOL_UNREACHABLE;     return (MTLLoadAction)0;
    }
}

_SOKOL_PRIVATE MTLStoreAction _sg_mtl_store_action(sg_store_action a, bool resolve) {
    switch (a) {
        case SG_STOREACTION_STORE:
            if (resolve) {
                return MTLStoreActionStoreAndMultisampleResolve;
            } else {
                return MTLStoreActionStore;
            }
            break;
        case SG_STOREACTION_DONTCARE:
            if (resolve) {
                return MTLStoreActionMultisampleResolve;
            } else {
                return MTLStoreActionDontCare;
            }
            break;
        default: SOKOL_UNREACHABLE; return (MTLStoreAction)0;
    }
}

_SOKOL_PRIVATE MTLResourceOptions _sg_mtl_resource_options_storage_mode_managed_or_shared(void) {
    #if defined(_SG_TARGET_MACOS)
    if (_sg.mtl.use_shared_storage_mode) {
        return MTLResourceStorageModeShared;
    } else {
        return MTLResourceStorageModeManaged;
    }
    #else
        // MTLResourceStorageModeManaged is not even defined on iOS SDK
        return MTLResourceStorageModeShared;
    #endif
}

_SOKOL_PRIVATE MTLResourceOptions _sg_mtl_buffer_resource_options(sg_usage usg) {
    switch (usg) {
        case SG_USAGE_IMMUTABLE:
            return _sg_mtl_resource_options_storage_mode_managed_or_shared();
        case SG_USAGE_DYNAMIC:
        case SG_USAGE_STREAM:
            return MTLResourceCPUCacheModeWriteCombined | _sg_mtl_resource_options_storage_mode_managed_or_shared();
        default:
            SOKOL_UNREACHABLE;
            return 0;
    }
}

_SOKOL_PRIVATE MTLVertexStepFunction _sg_mtl_step_function(sg_vertex_step step) {
    switch (step) {
        case SG_VERTEXSTEP_PER_VERTEX:      return MTLVertexStepFunctionPerVertex;
        case SG_VERTEXSTEP_PER_INSTANCE:    return MTLVertexStepFunctionPerInstance;
        default: SOKOL_UNREACHABLE; return (MTLVertexStepFunction)0;
    }
}

_SOKOL_PRIVATE MTLVertexFormat _sg_mtl_vertex_format(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_FLOAT:     return MTLVertexFormatFloat;
        case SG_VERTEXFORMAT_FLOAT2:    return MTLVertexFormatFloat2;
        case SG_VERTEXFORMAT_FLOAT3:    return MTLVertexFormatFloat3;
        case SG_VERTEXFORMAT_FLOAT4:    return MTLVertexFormatFloat4;
        case SG_VERTEXFORMAT_BYTE4:     return MTLVertexFormatChar4;
        case SG_VERTEXFORMAT_BYTE4N:    return MTLVertexFormatChar4Normalized;
        case SG_VERTEXFORMAT_UBYTE4:    return MTLVertexFormatUChar4;
        case SG_VERTEXFORMAT_UBYTE4N:   return MTLVertexFormatUChar4Normalized;
        case SG_VERTEXFORMAT_SHORT2:    return MTLVertexFormatShort2;
        case SG_VERTEXFORMAT_SHORT2N:   return MTLVertexFormatShort2Normalized;
        case SG_VERTEXFORMAT_USHORT2N:  return MTLVertexFormatUShort2Normalized;
        case SG_VERTEXFORMAT_SHORT4:    return MTLVertexFormatShort4;
        case SG_VERTEXFORMAT_SHORT4N:   return MTLVertexFormatShort4Normalized;
        case SG_VERTEXFORMAT_USHORT4N:  return MTLVertexFormatUShort4Normalized;
        case SG_VERTEXFORMAT_UINT10_N2: return MTLVertexFormatUInt1010102Normalized;
        case SG_VERTEXFORMAT_HALF2:     return MTLVertexFormatHalf2;
        case SG_VERTEXFORMAT_HALF4:     return MTLVertexFormatHalf4;
        default: SOKOL_UNREACHABLE; return (MTLVertexFormat)0;
    }
}

_SOKOL_PRIVATE MTLPrimitiveType _sg_mtl_primitive_type(sg_primitive_type t) {
    switch (t) {
        case SG_PRIMITIVETYPE_POINTS:           return MTLPrimitiveTypePoint;
        case SG_PRIMITIVETYPE_LINES:            return MTLPrimitiveTypeLine;
        case SG_PRIMITIVETYPE_LINE_STRIP:       return MTLPrimitiveTypeLineStrip;
        case SG_PRIMITIVETYPE_TRIANGLES:        return MTLPrimitiveTypeTriangle;
        case SG_PRIMITIVETYPE_TRIANGLE_STRIP:   return MTLPrimitiveTypeTriangleStrip;
        default: SOKOL_UNREACHABLE; return (MTLPrimitiveType)0;
    }
}

_SOKOL_PRIVATE MTLPixelFormat _sg_mtl_pixel_format(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:                     return MTLPixelFormatR8Unorm;
        case SG_PIXELFORMAT_R8SN:                   return MTLPixelFormatR8Snorm;
        case SG_PIXELFORMAT_R8UI:                   return MTLPixelFormatR8Uint;
        case SG_PIXELFORMAT_R8SI:                   return MTLPixelFormatR8Sint;
        case SG_PIXELFORMAT_R16:                    return MTLPixelFormatR16Unorm;
        case SG_PIXELFORMAT_R16SN:                  return MTLPixelFormatR16Snorm;
        case SG_PIXELFORMAT_R16UI:                  return MTLPixelFormatR16Uint;
        case SG_PIXELFORMAT_R16SI:                  return MTLPixelFormatR16Sint;
        case SG_PIXELFORMAT_R16F:                   return MTLPixelFormatR16Float;
        case SG_PIXELFORMAT_RG8:                    return MTLPixelFormatRG8Unorm;
        case SG_PIXELFORMAT_RG8SN:                  return MTLPixelFormatRG8Snorm;
        case SG_PIXELFORMAT_RG8UI:                  return MTLPixelFormatRG8Uint;
        case SG_PIXELFORMAT_RG8SI:                  return MTLPixelFormatRG8Sint;
        case SG_PIXELFORMAT_R32UI:                  return MTLPixelFormatR32Uint;
        case SG_PIXELFORMAT_R32SI:                  return MTLPixelFormatR32Sint;
        case SG_PIXELFORMAT_R32F:                   return MTLPixelFormatR32Float;
        case SG_PIXELFORMAT_RG16:                   return MTLPixelFormatRG16Unorm;
        case SG_PIXELFORMAT_RG16SN:                 return MTLPixelFormatRG16Snorm;
        case SG_PIXELFORMAT_RG16UI:                 return MTLPixelFormatRG16Uint;
        case SG_PIXELFORMAT_RG16SI:                 return MTLPixelFormatRG16Sint;
        case SG_PIXELFORMAT_RG16F:                  return MTLPixelFormatRG16Float;
        case SG_PIXELFORMAT_RGBA8:                  return MTLPixelFormatRGBA8Unorm;
        case SG_PIXELFORMAT_SRGB8A8:                return MTLPixelFormatRGBA8Unorm_sRGB;
        case SG_PIXELFORMAT_RGBA8SN:                return MTLPixelFormatRGBA8Snorm;
        case SG_PIXELFORMAT_RGBA8UI:                return MTLPixelFormatRGBA8Uint;
        case SG_PIXELFORMAT_RGBA8SI:                return MTLPixelFormatRGBA8Sint;
        case SG_PIXELFORMAT_BGRA8:                  return MTLPixelFormatBGRA8Unorm;
        case SG_PIXELFORMAT_RGB10A2:                return MTLPixelFormatRGB10A2Unorm;
        case SG_PIXELFORMAT_RG11B10F:               return MTLPixelFormatRG11B10Float;
        case SG_PIXELFORMAT_RGB9E5:                 return MTLPixelFormatRGB9E5Float;
        case SG_PIXELFORMAT_RG32UI:                 return MTLPixelFormatRG32Uint;
        case SG_PIXELFORMAT_RG32SI:                 return MTLPixelFormatRG32Sint;
        case SG_PIXELFORMAT_RG32F:                  return MTLPixelFormatRG32Float;
        case SG_PIXELFORMAT_RGBA16:                 return MTLPixelFormatRGBA16Unorm;
        case SG_PIXELFORMAT_RGBA16SN:               return MTLPixelFormatRGBA16Snorm;
        case SG_PIXELFORMAT_RGBA16UI:               return MTLPixelFormatRGBA16Uint;
        case SG_PIXELFORMAT_RGBA16SI:               return MTLPixelFormatRGBA16Sint;
        case SG_PIXELFORMAT_RGBA16F:                return MTLPixelFormatRGBA16Float;
        case SG_PIXELFORMAT_RGBA32UI:               return MTLPixelFormatRGBA32Uint;
        case SG_PIXELFORMAT_RGBA32SI:               return MTLPixelFormatRGBA32Sint;
        case SG_PIXELFORMAT_RGBA32F:                return MTLPixelFormatRGBA32Float;
        case SG_PIXELFORMAT_DEPTH:                  return MTLPixelFormatDepth32Float;
        case SG_PIXELFORMAT_DEPTH_STENCIL:          return MTLPixelFormatDepth32Float_Stencil8;
        #if defined(_SG_TARGET_MACOS)
        case SG_PIXELFORMAT_BC1_RGBA:               return MTLPixelFormatBC1_RGBA;
        case SG_PIXELFORMAT_BC2_RGBA:               return MTLPixelFormatBC2_RGBA;
        case SG_PIXELFORMAT_BC3_RGBA:               return MTLPixelFormatBC3_RGBA;
        case SG_PIXELFORMAT_BC3_SRGBA:              return MTLPixelFormatBC3_RGBA_sRGB;
        case SG_PIXELFORMAT_BC4_R:                  return MTLPixelFormatBC4_RUnorm;
        case SG_PIXELFORMAT_BC4_RSN:                return MTLPixelFormatBC4_RSnorm;
        case SG_PIXELFORMAT_BC5_RG:                 return MTLPixelFormatBC5_RGUnorm;
        case SG_PIXELFORMAT_BC5_RGSN:               return MTLPixelFormatBC5_RGSnorm;
        case SG_PIXELFORMAT_BC6H_RGBF:              return MTLPixelFormatBC6H_RGBFloat;
        case SG_PIXELFORMAT_BC6H_RGBUF:             return MTLPixelFormatBC6H_RGBUfloat;
        case SG_PIXELFORMAT_BC7_RGBA:               return MTLPixelFormatBC7_RGBAUnorm;
        case SG_PIXELFORMAT_BC7_SRGBA:              return MTLPixelFormatBC7_RGBAUnorm_sRGB;
        #else
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:         return MTLPixelFormatPVRTC_RGB_2BPP;
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:         return MTLPixelFormatPVRTC_RGB_4BPP;
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:        return MTLPixelFormatPVRTC_RGBA_2BPP;
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:        return MTLPixelFormatPVRTC_RGBA_4BPP;
        case SG_PIXELFORMAT_ETC2_RGB8:              return MTLPixelFormatETC2_RGB8;
        case SG_PIXELFORMAT_ETC2_SRGB8:             return MTLPixelFormatETC2_RGB8_sRGB;
        case SG_PIXELFORMAT_ETC2_RGB8A1:            return MTLPixelFormatETC2_RGB8A1;
        case SG_PIXELFORMAT_ETC2_RGBA8:             return MTLPixelFormatEAC_RGBA8;
        case SG_PIXELFORMAT_ETC2_SRGB8A8:           return MTLPixelFormatEAC_RGBA8_sRGB;
        case SG_PIXELFORMAT_EAC_R11:                return MTLPixelFormatEAC_R11Unorm;
        case SG_PIXELFORMAT_EAC_R11SN:              return MTLPixelFormatEAC_R11Snorm;
        case SG_PIXELFORMAT_EAC_RG11:               return MTLPixelFormatEAC_RG11Unorm;
        case SG_PIXELFORMAT_EAC_RG11SN:             return MTLPixelFormatEAC_RG11Snorm;
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:          return MTLPixelFormatASTC_4x4_LDR;
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:         return MTLPixelFormatASTC_4x4_sRGB;
        #endif
        default: return MTLPixelFormatInvalid;
    }
}

_SOKOL_PRIVATE MTLColorWriteMask _sg_mtl_color_write_mask(sg_color_mask m) {
    MTLColorWriteMask mtl_mask = MTLColorWriteMaskNone;
    if (m & SG_COLORMASK_R) {
        mtl_mask |= MTLColorWriteMaskRed;
    }
    if (m & SG_COLORMASK_G) {
        mtl_mask |= MTLColorWriteMaskGreen;
    }
    if (m & SG_COLORMASK_B) {
        mtl_mask |= MTLColorWriteMaskBlue;
    }
    if (m & SG_COLORMASK_A) {
        mtl_mask |= MTLColorWriteMaskAlpha;
    }
    return mtl_mask;
}

_SOKOL_PRIVATE MTLBlendOperation _sg_mtl_blend_op(sg_blend_op op) {
    switch (op) {
        case SG_BLENDOP_ADD:                return MTLBlendOperationAdd;
        case SG_BLENDOP_SUBTRACT:           return MTLBlendOperationSubtract;
        case SG_BLENDOP_REVERSE_SUBTRACT:   return MTLBlendOperationReverseSubtract;
        default: SOKOL_UNREACHABLE; return (MTLBlendOperation)0;
    }
}

_SOKOL_PRIVATE MTLBlendFactor _sg_mtl_blend_factor(sg_blend_factor f) {
    switch (f) {
        case SG_BLENDFACTOR_ZERO:                   return MTLBlendFactorZero;
        case SG_BLENDFACTOR_ONE:                    return MTLBlendFactorOne;
        case SG_BLENDFACTOR_SRC_COLOR:              return MTLBlendFactorSourceColor;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:    return MTLBlendFactorOneMinusSourceColor;
        case SG_BLENDFACTOR_SRC_ALPHA:              return MTLBlendFactorSourceAlpha;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:    return MTLBlendFactorOneMinusSourceAlpha;
        case SG_BLENDFACTOR_DST_COLOR:              return MTLBlendFactorDestinationColor;
        case SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:    return MTLBlendFactorOneMinusDestinationColor;
        case SG_BLENDFACTOR_DST_ALPHA:              return MTLBlendFactorDestinationAlpha;
        case SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:    return MTLBlendFactorOneMinusDestinationAlpha;
        case SG_BLENDFACTOR_SRC_ALPHA_SATURATED:    return MTLBlendFactorSourceAlphaSaturated;
        case SG_BLENDFACTOR_BLEND_COLOR:            return MTLBlendFactorBlendColor;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:  return MTLBlendFactorOneMinusBlendColor;
        case SG_BLENDFACTOR_BLEND_ALPHA:            return MTLBlendFactorBlendAlpha;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:  return MTLBlendFactorOneMinusBlendAlpha;
        default: SOKOL_UNREACHABLE; return (MTLBlendFactor)0;
    }
}

_SOKOL_PRIVATE MTLCompareFunction _sg_mtl_compare_func(sg_compare_func f) {
    switch (f) {
        case SG_COMPAREFUNC_NEVER:          return MTLCompareFunctionNever;
        case SG_COMPAREFUNC_LESS:           return MTLCompareFunctionLess;
        case SG_COMPAREFUNC_EQUAL:          return MTLCompareFunctionEqual;
        case SG_COMPAREFUNC_LESS_EQUAL:     return MTLCompareFunctionLessEqual;
        case SG_COMPAREFUNC_GREATER:        return MTLCompareFunctionGreater;
        case SG_COMPAREFUNC_NOT_EQUAL:      return MTLCompareFunctionNotEqual;
        case SG_COMPAREFUNC_GREATER_EQUAL:  return MTLCompareFunctionGreaterEqual;
        case SG_COMPAREFUNC_ALWAYS:         return MTLCompareFunctionAlways;
        default: SOKOL_UNREACHABLE; return (MTLCompareFunction)0;
    }
}

_SOKOL_PRIVATE MTLStencilOperation _sg_mtl_stencil_op(sg_stencil_op op) {
    switch (op) {
        case SG_STENCILOP_KEEP:         return MTLStencilOperationKeep;
        case SG_STENCILOP_ZERO:         return MTLStencilOperationZero;
        case SG_STENCILOP_REPLACE:      return MTLStencilOperationReplace;
        case SG_STENCILOP_INCR_CLAMP:   return MTLStencilOperationIncrementClamp;
        case SG_STENCILOP_DECR_CLAMP:   return MTLStencilOperationDecrementClamp;
        case SG_STENCILOP_INVERT:       return MTLStencilOperationInvert;
        case SG_STENCILOP_INCR_WRAP:    return MTLStencilOperationIncrementWrap;
        case SG_STENCILOP_DECR_WRAP:    return MTLStencilOperationDecrementWrap;
        default: SOKOL_UNREACHABLE; return (MTLStencilOperation)0;
    }
}

_SOKOL_PRIVATE MTLCullMode _sg_mtl_cull_mode(sg_cull_mode m) {
    switch (m) {
        case SG_CULLMODE_NONE:  return MTLCullModeNone;
        case SG_CULLMODE_FRONT: return MTLCullModeFront;
        case SG_CULLMODE_BACK:  return MTLCullModeBack;
        default: SOKOL_UNREACHABLE; return (MTLCullMode)0;
    }
}

_SOKOL_PRIVATE MTLWinding _sg_mtl_winding(sg_face_winding w) {
    switch (w) {
        case SG_FACEWINDING_CW:     return MTLWindingClockwise;
        case SG_FACEWINDING_CCW:    return MTLWindingCounterClockwise;
        default: SOKOL_UNREACHABLE; return (MTLWinding)0;
    }
}

_SOKOL_PRIVATE MTLIndexType _sg_mtl_index_type(sg_index_type t) {
    switch (t) {
        case SG_INDEXTYPE_UINT16:   return MTLIndexTypeUInt16;
        case SG_INDEXTYPE_UINT32:   return MTLIndexTypeUInt32;
        default: SOKOL_UNREACHABLE; return (MTLIndexType)0;
    }
}

_SOKOL_PRIVATE int _sg_mtl_index_size(sg_index_type t) {
    switch (t) {
        case SG_INDEXTYPE_NONE:     return 0;
        case SG_INDEXTYPE_UINT16:   return 2;
        case SG_INDEXTYPE_UINT32:   return 4;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE MTLTextureType _sg_mtl_texture_type(sg_image_type t) {
    switch (t) {
        case SG_IMAGETYPE_2D:       return MTLTextureType2D;
        case SG_IMAGETYPE_CUBE:     return MTLTextureTypeCube;
        case SG_IMAGETYPE_3D:       return MTLTextureType3D;
        case SG_IMAGETYPE_ARRAY:    return MTLTextureType2DArray;
        default: SOKOL_UNREACHABLE; return (MTLTextureType)0;
    }
}

_SOKOL_PRIVATE bool _sg_mtl_is_pvrtc(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
            return true;
        default:
            return false;
    }
}

_SOKOL_PRIVATE MTLSamplerAddressMode _sg_mtl_address_mode(sg_wrap w) {
    if (_sg.features.image_clamp_to_border) {
        if (@available(macOS 12.0, iOS 14.0, *)) {
            // border color feature available
            switch (w) {
                case SG_WRAP_REPEAT:            return MTLSamplerAddressModeRepeat;
                case SG_WRAP_CLAMP_TO_EDGE:     return MTLSamplerAddressModeClampToEdge;
                case SG_WRAP_CLAMP_TO_BORDER:   return MTLSamplerAddressModeClampToBorderColor;
                case SG_WRAP_MIRRORED_REPEAT:   return MTLSamplerAddressModeMirrorRepeat;
                default: SOKOL_UNREACHABLE; return (MTLSamplerAddressMode)0;
            }
        }
    }
    // fallthrough: clamp to border no supported
    switch (w) {
        case SG_WRAP_REPEAT:            return MTLSamplerAddressModeRepeat;
        case SG_WRAP_CLAMP_TO_EDGE:     return MTLSamplerAddressModeClampToEdge;
        case SG_WRAP_CLAMP_TO_BORDER:   return MTLSamplerAddressModeClampToEdge;
        case SG_WRAP_MIRRORED_REPEAT:   return MTLSamplerAddressModeMirrorRepeat;
        default: SOKOL_UNREACHABLE; return (MTLSamplerAddressMode)0;
    }
}

_SOKOL_PRIVATE API_AVAILABLE(ios(14.0), macos(12.0)) MTLSamplerBorderColor _sg_mtl_border_color(sg_border_color c) {
    switch (c) {
        case SG_BORDERCOLOR_TRANSPARENT_BLACK: return MTLSamplerBorderColorTransparentBlack;
        case SG_BORDERCOLOR_OPAQUE_BLACK: return MTLSamplerBorderColorOpaqueBlack;
        case SG_BORDERCOLOR_OPAQUE_WHITE: return MTLSamplerBorderColorOpaqueWhite;
        default: SOKOL_UNREACHABLE; return (MTLSamplerBorderColor)0;
    }
}

_SOKOL_PRIVATE MTLSamplerMinMagFilter _sg_mtl_minmag_filter(sg_filter f) {
    switch (f) {
        case SG_FILTER_NEAREST:
            return MTLSamplerMinMagFilterNearest;
        case SG_FILTER_LINEAR:
            return MTLSamplerMinMagFilterLinear;
        default:
            SOKOL_UNREACHABLE; return (MTLSamplerMinMagFilter)0;
    }
}

_SOKOL_PRIVATE MTLSamplerMipFilter _sg_mtl_mipmap_filter(sg_filter f) {
    switch (f) {
        case SG_FILTER_NONE:
            return MTLSamplerMipFilterNotMipmapped;
        case SG_FILTER_NEAREST:
            return MTLSamplerMipFilterNearest;
        case SG_FILTER_LINEAR:
            return MTLSamplerMipFilterLinear;
        default:
            SOKOL_UNREACHABLE; return (MTLSamplerMipFilter)0;
    }
}

//-- a pool for all Metal resource objects, with deferred release queue ---------
_SOKOL_PRIVATE void _sg_mtl_init_pool(const sg_desc* desc) {
    _sg.mtl.idpool.num_slots = 2 *
        (
            2 * desc->buffer_pool_size +
            4 * desc->image_pool_size +
            1 * desc->sampler_pool_size +
            4 * desc->shader_pool_size +
            2 * desc->pipeline_pool_size +
            desc->attachments_pool_size +
            128
        );
    _sg.mtl.idpool.pool = [NSMutableArray arrayWithCapacity:(NSUInteger)_sg.mtl.idpool.num_slots];
    _SG_OBJC_RETAIN(_sg.mtl.idpool.pool);
    NSNull* null = [NSNull null];
    for (int i = 0; i < _sg.mtl.idpool.num_slots; i++) {
        [_sg.mtl.idpool.pool addObject:null];
    }
    SOKOL_ASSERT([_sg.mtl.idpool.pool count] == (NSUInteger)_sg.mtl.idpool.num_slots);
    // a queue of currently free slot indices
    _sg.mtl.idpool.free_queue_top = 0;
    _sg.mtl.idpool.free_queue = (int*)_sg_malloc_clear((size_t)_sg.mtl.idpool.num_slots * sizeof(int));
    // pool slot 0 is reserved!
    for (int i = _sg.mtl.idpool.num_slots-1; i >= 1; i--) {
        _sg.mtl.idpool.free_queue[_sg.mtl.idpool.free_queue_top++] = i;
    }
    // a circular queue which holds release items (frame index when a resource is to be released, and the resource's pool index
    _sg.mtl.idpool.release_queue_front = 0;
    _sg.mtl.idpool.release_queue_back = 0;
    _sg.mtl.idpool.release_queue = (_sg_mtl_release_item_t*)_sg_malloc_clear((size_t)_sg.mtl.idpool.num_slots * sizeof(_sg_mtl_release_item_t));
    for (int i = 0; i < _sg.mtl.idpool.num_slots; i++) {
        _sg.mtl.idpool.release_queue[i].frame_index = 0;
        _sg.mtl.idpool.release_queue[i].slot_index = _SG_MTL_INVALID_SLOT_INDEX;
    }
}

_SOKOL_PRIVATE void _sg_mtl_destroy_pool(void) {
    _sg_free(_sg.mtl.idpool.release_queue);  _sg.mtl.idpool.release_queue = 0;
    _sg_free(_sg.mtl.idpool.free_queue);     _sg.mtl.idpool.free_queue = 0;
    _SG_OBJC_RELEASE(_sg.mtl.idpool.pool);
}

// get a new free resource pool slot
_SOKOL_PRIVATE int _sg_mtl_alloc_pool_slot(void) {
    SOKOL_ASSERT(_sg.mtl.idpool.free_queue_top > 0);
    const int slot_index = _sg.mtl.idpool.free_queue[--_sg.mtl.idpool.free_queue_top];
    SOKOL_ASSERT((slot_index > 0) && (slot_index < _sg.mtl.idpool.num_slots));
    return slot_index;
}

// put a free resource pool slot back into the free-queue
_SOKOL_PRIVATE void _sg_mtl_free_pool_slot(int slot_index) {
    SOKOL_ASSERT(_sg.mtl.idpool.free_queue_top < _sg.mtl.idpool.num_slots);
    SOKOL_ASSERT((slot_index > 0) && (slot_index < _sg.mtl.idpool.num_slots));
    _sg.mtl.idpool.free_queue[_sg.mtl.idpool.free_queue_top++] = slot_index;
}

// add an MTLResource to the pool, return pool index or 0 if input was 'nil'
_SOKOL_PRIVATE int _sg_mtl_add_resource(id res) {
    if (nil == res) {
        return _SG_MTL_INVALID_SLOT_INDEX;
    }
    _sg_stats_add(metal.idpool.num_added, 1);
    const int slot_index = _sg_mtl_alloc_pool_slot();
    // NOTE: the NSMutableArray will take ownership of its items
    SOKOL_ASSERT([NSNull null] == _sg.mtl.idpool.pool[(NSUInteger)slot_index]);
    _sg.mtl.idpool.pool[(NSUInteger)slot_index] = res;
    return slot_index;
}

/*  mark an MTLResource for release, this will put the resource into the
    deferred-release queue, and the resource will then be released N frames later,
    the special pool index 0 will be ignored (this means that a nil
    value was provided to _sg_mtl_add_resource()
*/
_SOKOL_PRIVATE void _sg_mtl_release_resource(uint32_t frame_index, int slot_index) {
    if (slot_index == _SG_MTL_INVALID_SLOT_INDEX) {
        return;
    }
    _sg_stats_add(metal.idpool.num_released, 1);
    SOKOL_ASSERT((slot_index > 0) && (slot_index < _sg.mtl.idpool.num_slots));
    SOKOL_ASSERT([NSNull null] != _sg.mtl.idpool.pool[(NSUInteger)slot_index]);
    int release_index = _sg.mtl.idpool.release_queue_front++;
    if (_sg.mtl.idpool.release_queue_front >= _sg.mtl.idpool.num_slots) {
        // wrap-around
        _sg.mtl.idpool.release_queue_front = 0;
    }
    // release queue full?
    SOKOL_ASSERT(_sg.mtl.idpool.release_queue_front != _sg.mtl.idpool.release_queue_back);
    SOKOL_ASSERT(0 == _sg.mtl.idpool.release_queue[release_index].frame_index);
    const uint32_t safe_to_release_frame_index = frame_index + SG_NUM_INFLIGHT_FRAMES + 1;
    _sg.mtl.idpool.release_queue[release_index].frame_index = safe_to_release_frame_index;
    _sg.mtl.idpool.release_queue[release_index].slot_index = slot_index;
}

// run garbage-collection pass on all resources in the release-queue
_SOKOL_PRIVATE void _sg_mtl_garbage_collect(uint32_t frame_index) {
    while (_sg.mtl.idpool.release_queue_back != _sg.mtl.idpool.release_queue_front) {
        if (frame_index < _sg.mtl.idpool.release_queue[_sg.mtl.idpool.release_queue_back].frame_index) {
            // don't need to check further, release-items past this are too young
            break;
        }
        _sg_stats_add(metal.idpool.num_garbage_collected, 1);
        // safe to release this resource
        const int slot_index = _sg.mtl.idpool.release_queue[_sg.mtl.idpool.release_queue_back].slot_index;
        SOKOL_ASSERT((slot_index > 0) && (slot_index < _sg.mtl.idpool.num_slots));
        // note: the NSMutableArray takes ownership of its items, assigning an NSNull object will
        // release the object, no matter if using ARC or not
        SOKOL_ASSERT(_sg.mtl.idpool.pool[(NSUInteger)slot_index] != [NSNull null]);
        _sg.mtl.idpool.pool[(NSUInteger)slot_index] = [NSNull null];
        // put the now free pool index back on the free queue
        _sg_mtl_free_pool_slot(slot_index);
        // reset the release queue slot and advance the back index
        _sg.mtl.idpool.release_queue[_sg.mtl.idpool.release_queue_back].frame_index = 0;
        _sg.mtl.idpool.release_queue[_sg.mtl.idpool.release_queue_back].slot_index = _SG_MTL_INVALID_SLOT_INDEX;
        _sg.mtl.idpool.release_queue_back++;
        if (_sg.mtl.idpool.release_queue_back >= _sg.mtl.idpool.num_slots) {
            // wrap-around
            _sg.mtl.idpool.release_queue_back = 0;
        }
    }
}

_SOKOL_PRIVATE id _sg_mtl_id(int slot_index) {
    return _sg.mtl.idpool.pool[(NSUInteger)slot_index];
}

_SOKOL_PRIVATE void _sg_mtl_clear_state_cache(void) {
    _sg_clear(&_sg.mtl.state_cache, sizeof(_sg.mtl.state_cache));
}

// https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
_SOKOL_PRIVATE void _sg_mtl_init_caps(void) {
    #if defined(_SG_TARGET_MACOS)
        _sg.backend = SG_BACKEND_METAL_MACOS;
    #elif defined(_SG_TARGET_IOS)
        #if defined(_SG_TARGET_IOS_SIMULATOR)
            _sg.backend = SG_BACKEND_METAL_SIMULATOR;
        #else
            _sg.backend = SG_BACKEND_METAL_IOS;
        #endif
    #endif
    _sg.features.origin_top_left = true;
    _sg.features.mrt_independent_blend_state = true;
    _sg.features.mrt_independent_write_mask = true;
    _sg.features.storage_buffer = true;

    _sg.features.image_clamp_to_border = false;
    #if (MAC_OS_X_VERSION_MAX_ALLOWED >= 120000) || (__IPHONE_OS_VERSION_MAX_ALLOWED >= 140000)
    if (@available(macOS 12.0, iOS 14.0, *)) {
        _sg.features.image_clamp_to_border = [_sg.mtl.device supportsFamily:MTLGPUFamilyApple7]
                                             || [_sg.mtl.device supportsFamily:MTLGPUFamilyMac2];
        #if (MAC_OS_X_VERSION_MAX_ALLOWED >= 130000) || (__IPHONE_OS_VERSION_MAX_ALLOWED >= 160000)
        if (!_sg.features.image_clamp_to_border) {
            if (@available(macOS 13.0, iOS 16.0, *)) {
                _sg.features.image_clamp_to_border = [_sg.mtl.device supportsFamily:MTLGPUFamilyMetal3];
            }
        }
        #endif
    }
    #endif

    #if defined(_SG_TARGET_MACOS)
        _sg.limits.max_image_size_2d = 16 * 1024;
        _sg.limits.max_image_size_cube = 16 * 1024;
        _sg.limits.max_image_size_3d = 2 * 1024;
        _sg.limits.max_image_size_array = 16 * 1024;
        _sg.limits.max_image_array_layers = 2 * 1024;
    #else
        // FIXME: newer iOS devices support 16k textures
        _sg.limits.max_image_size_2d = 8 * 1024;
        _sg.limits.max_image_size_cube = 8 * 1024;
        _sg.limits.max_image_size_3d = 2 * 1024;
        _sg.limits.max_image_size_array = 8 * 1024;
        _sg.limits.max_image_array_layers = 2 * 1024;
    #endif
    _sg.limits.max_vertex_attrs = SG_MAX_VERTEX_ATTRIBUTES;

    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R8SI]);
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16SN]);
    #else
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_R16]);
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_R16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R16SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16F]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG8SI]);
    _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_R32UI]);
    _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_R32SI]);
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R32F]);
    #else
        _sg_pixelformat_sbr(&_sg.formats[SG_PIXELFORMAT_R32F]);
    #endif
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16SN]);
    #else
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_RG16]);
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_RG16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG16SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16F]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_SRGB8A8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA8SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_BGRA8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGB10A2]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG11B10F]);
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RGB9E5]);
        _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG32UI]);
        _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG32SI]);
    #else
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGB9E5]);
        _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_RG32UI]);
        _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_RG32SI]);
    #endif
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG32F]);
    #else
        _sg_pixelformat_sbr(&_sg.formats[SG_PIXELFORMAT_RG32F]);
    #endif
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16SN]);
    #else
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_RGBA16]);
        _sg_pixelformat_sfbr(&_sg.formats[SG_PIXELFORMAT_RGBA16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA16SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16F]);
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA32UI]);
        _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA32SI]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
    #else
        _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_RGBA32UI]);
        _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_RGBA32SI]);
        _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
    #endif
    _sg_pixelformat_srmd(&_sg.formats[SG_PIXELFORMAT_DEPTH]);
    _sg_pixelformat_srmd(&_sg.formats[SG_PIXELFORMAT_DEPTH_STENCIL]);
    #if defined(_SG_TARGET_MACOS)
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC1_RGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC2_RGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC3_RGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC3_SRGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC4_R]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC4_RSN]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC5_RG]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC5_RGSN]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC6H_RGBF]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC6H_RGBUF]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC7_RGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC7_SRGBA]);
    #else
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGB_2BPP]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGB_4BPP]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGBA_2BPP]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGBA_4BPP]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGB8]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_SRGB8]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGB8A1]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGBA8]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_SRGB8A8]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_R11]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_R11SN]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_RG11]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_RG11SN]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ASTC_4x4_RGBA]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ASTC_4x4_SRGBA]);

    #endif
}

//-- main Metal backend state and functions ------------------------------------
_SOKOL_PRIVATE void _sg_mtl_setup_backend(const sg_desc* desc) {
    // assume already zero-initialized
    SOKOL_ASSERT(desc);
    SOKOL_ASSERT(desc->environment.metal.device);
    SOKOL_ASSERT(desc->uniform_buffer_size > 0);
    _sg_mtl_init_pool(desc);
    _sg_mtl_clear_state_cache();
    _sg.mtl.valid = true;
    _sg.mtl.ub_size = desc->uniform_buffer_size;
    _sg.mtl.sem = dispatch_semaphore_create(SG_NUM_INFLIGHT_FRAMES);
    _sg.mtl.device = (__bridge id<MTLDevice>) desc->environment.metal.device;
    _sg.mtl.cmd_queue = [_sg.mtl.device newCommandQueue];

    for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
        _sg.mtl.uniform_buffers[i] = [_sg.mtl.device
            newBufferWithLength:(NSUInteger)_sg.mtl.ub_size
            options:MTLResourceCPUCacheModeWriteCombined|MTLResourceStorageModeShared
        ];
        #if defined(SOKOL_DEBUG)
            _sg.mtl.uniform_buffers[i].label = [NSString stringWithFormat:@"sg-uniform-buffer.%d", i];
        #endif
    }

    if (desc->mtl_force_managed_storage_mode) {
        _sg.mtl.use_shared_storage_mode = false;
    } else if (@available(macOS 10.15, iOS 13.0, *)) {
        // on Intel Macs, always use managed resources even though the
        // device says it supports unified memory (because of texture restrictions)
        const bool is_apple_gpu = [_sg.mtl.device supportsFamily:MTLGPUFamilyApple1];
        if (!is_apple_gpu) {
            _sg.mtl.use_shared_storage_mode = false;
        } else {
            _sg.mtl.use_shared_storage_mode = true;
        }
    } else {
        #if defined(_SG_TARGET_MACOS)
            _sg.mtl.use_shared_storage_mode = false;
        #else
            _sg.mtl.use_shared_storage_mode = true;
        #endif
    }
    _sg_mtl_init_caps();
}

_SOKOL_PRIVATE void _sg_mtl_discard_backend(void) {
    SOKOL_ASSERT(_sg.mtl.valid);
    // wait for the last frame to finish
    for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
        dispatch_semaphore_wait(_sg.mtl.sem, DISPATCH_TIME_FOREVER);
    }
    // semaphore must be "relinquished" before destruction
    for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
        dispatch_semaphore_signal(_sg.mtl.sem);
    }
    _sg_mtl_garbage_collect(_sg.frame_index + SG_NUM_INFLIGHT_FRAMES + 2);
    _sg_mtl_destroy_pool();
    _sg.mtl.valid = false;

    _SG_OBJC_RELEASE(_sg.mtl.sem);
    _SG_OBJC_RELEASE(_sg.mtl.device);
    _SG_OBJC_RELEASE(_sg.mtl.cmd_queue);
    for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
        _SG_OBJC_RELEASE(_sg.mtl.uniform_buffers[i]);
    }
    // NOTE: MTLCommandBuffer and MTLRenderCommandEncoder are auto-released
    _sg.mtl.cmd_buffer = nil;
    _sg.mtl.cmd_encoder = nil;
}

_SOKOL_PRIVATE void _sg_mtl_bind_uniform_buffers(void) {
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);
    for (int slot = 0; slot < SG_MAX_SHADERSTAGE_UBS; slot++) {
        [_sg.mtl.cmd_encoder
            setVertexBuffer:_sg.mtl.uniform_buffers[_sg.mtl.cur_frame_rotate_index]
            offset:0
            atIndex:(NSUInteger)slot];
        [_sg.mtl.cmd_encoder
            setFragmentBuffer:_sg.mtl.uniform_buffers[_sg.mtl.cur_frame_rotate_index]
            offset:0
            atIndex:(NSUInteger)slot];
    }
}

_SOKOL_PRIVATE void _sg_mtl_reset_state_cache(void) {
    _sg_mtl_clear_state_cache();
    // need to restore the uniform buffer binding (normally happens in _sg_mtl_begin_pass()
    if (nil != _sg.mtl.cmd_encoder) {
        _sg_mtl_bind_uniform_buffers();
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(buf && desc);
    const bool injected = (0 != desc->mtl_buffers[0]);
    MTLResourceOptions mtl_options = _sg_mtl_buffer_resource_options(buf->cmn.usage);
    for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
        id<MTLBuffer> mtl_buf;
        if (injected) {
            SOKOL_ASSERT(desc->mtl_buffers[slot]);
            mtl_buf = (__bridge id<MTLBuffer>) desc->mtl_buffers[slot];
        } else {
            if (buf->cmn.usage == SG_USAGE_IMMUTABLE) {
                SOKOL_ASSERT(desc->data.ptr);
                mtl_buf = [_sg.mtl.device newBufferWithBytes:desc->data.ptr length:(NSUInteger)buf->cmn.size options:mtl_options];
            } else {
                mtl_buf = [_sg.mtl.device newBufferWithLength:(NSUInteger)buf->cmn.size options:mtl_options];
            }
            if (nil == mtl_buf) {
                _SG_ERROR(METAL_CREATE_BUFFER_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
        }
        #if defined(SOKOL_DEBUG)
            if (desc->label) {
                mtl_buf.label = [NSString stringWithFormat:@"%s.%d", desc->label, slot];
            }
        #endif
        buf->mtl.buf[slot] = _sg_mtl_add_resource(mtl_buf);
        _SG_OBJC_RELEASE(mtl_buf);
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_mtl_discard_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf);
    for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
        // it's valid to call release resource with '0'
        _sg_mtl_release_resource(_sg.frame_index, buf->mtl.buf[slot]);
    }
}

_SOKOL_PRIVATE void _sg_mtl_copy_image_data(const _sg_image_t* img, __unsafe_unretained id<MTLTexture> mtl_tex, const sg_image_data* data) {
    const int num_faces = (img->cmn.type == SG_IMAGETYPE_CUBE) ? 6:1;
    const int num_slices = (img->cmn.type == SG_IMAGETYPE_ARRAY) ? img->cmn.num_slices : 1;
    for (int face_index = 0; face_index < num_faces; face_index++) {
        for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++) {
            SOKOL_ASSERT(data->subimage[face_index][mip_index].ptr);
            SOKOL_ASSERT(data->subimage[face_index][mip_index].size > 0);
            const uint8_t* data_ptr = (const uint8_t*)data->subimage[face_index][mip_index].ptr;
            const int mip_width = _sg_miplevel_dim(img->cmn.width, mip_index);
            const int mip_height = _sg_miplevel_dim(img->cmn.height, mip_index);
            // special case PVRTC formats: bytePerRow and bytesPerImage must be 0
            int bytes_per_row = 0;
            int bytes_per_slice = 0;
            if (!_sg_mtl_is_pvrtc(img->cmn.pixel_format)) {
                bytes_per_row = _sg_row_pitch(img->cmn.pixel_format, mip_width, 1);
                bytes_per_slice = _sg_surface_pitch(img->cmn.pixel_format, mip_width, mip_height, 1);
            }
            /* bytesPerImage special case: https://developer.apple.com/documentation/metal/mtltexture/1515679-replaceregion

                "Supply a nonzero value only when you copy data to a MTLTextureType3D type texture"
            */
            MTLRegion region;
            int bytes_per_image;
            if (img->cmn.type == SG_IMAGETYPE_3D) {
                const int mip_depth = _sg_miplevel_dim(img->cmn.num_slices, mip_index);
                region = MTLRegionMake3D(0, 0, 0, (NSUInteger)mip_width, (NSUInteger)mip_height, (NSUInteger)mip_depth);
                bytes_per_image = bytes_per_slice;
                // FIXME: apparently the minimal bytes_per_image size for 3D texture is 4 KByte... somehow need to handle this
            } else {
                region = MTLRegionMake2D(0, 0, (NSUInteger)mip_width, (NSUInteger)mip_height);
                bytes_per_image = 0;
            }

            for (int slice_index = 0; slice_index < num_slices; slice_index++) {
                const int mtl_slice_index = (img->cmn.type == SG_IMAGETYPE_CUBE) ? face_index : slice_index;
                const int slice_offset = slice_index * bytes_per_slice;
                SOKOL_ASSERT((slice_offset + bytes_per_slice) <= (int)data->subimage[face_index][mip_index].size);
                [mtl_tex replaceRegion:region
                    mipmapLevel:(NSUInteger)mip_index
                    slice:(NSUInteger)mtl_slice_index
                    withBytes:data_ptr + slice_offset
                    bytesPerRow:(NSUInteger)bytes_per_row
                    bytesPerImage:(NSUInteger)bytes_per_image];
            }
        }
    }
}

// initialize MTLTextureDescriptor with common attributes
_SOKOL_PRIVATE bool _sg_mtl_init_texdesc_common(MTLTextureDescriptor* mtl_desc, _sg_image_t* img) {
    mtl_desc.textureType = _sg_mtl_texture_type(img->cmn.type);
    mtl_desc.pixelFormat = _sg_mtl_pixel_format(img->cmn.pixel_format);
    if (MTLPixelFormatInvalid == mtl_desc.pixelFormat) {
        _SG_ERROR(METAL_TEXTURE_FORMAT_NOT_SUPPORTED);
        return false;
    }
    mtl_desc.width = (NSUInteger)img->cmn.width;
    mtl_desc.height = (NSUInteger)img->cmn.height;
    if (SG_IMAGETYPE_3D == img->cmn.type) {
        mtl_desc.depth = (NSUInteger)img->cmn.num_slices;
    } else {
        mtl_desc.depth = 1;
    }
    mtl_desc.mipmapLevelCount = (NSUInteger)img->cmn.num_mipmaps;
    if (SG_IMAGETYPE_ARRAY == img->cmn.type) {
        mtl_desc.arrayLength = (NSUInteger)img->cmn.num_slices;
    } else {
        mtl_desc.arrayLength = 1;
    }
    mtl_desc.usage = MTLTextureUsageShaderRead;
    MTLResourceOptions res_options = 0;
    if (img->cmn.usage != SG_USAGE_IMMUTABLE) {
        res_options |= MTLResourceCPUCacheModeWriteCombined;
    }
    res_options |= _sg_mtl_resource_options_storage_mode_managed_or_shared();
    mtl_desc.resourceOptions = res_options;
    return true;
}

// initialize MTLTextureDescriptor with rendertarget attributes
_SOKOL_PRIVATE void _sg_mtl_init_texdesc_rt(MTLTextureDescriptor* mtl_desc, _sg_image_t* img) {
    SOKOL_ASSERT(img->cmn.render_target);
    _SOKOL_UNUSED(img);
    mtl_desc.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    mtl_desc.resourceOptions = MTLResourceStorageModePrivate;
}

// initialize MTLTextureDescriptor with MSAA attributes
_SOKOL_PRIVATE void _sg_mtl_init_texdesc_rt_msaa(MTLTextureDescriptor* mtl_desc, _sg_image_t* img) {
    SOKOL_ASSERT(img->cmn.sample_count > 1);
    mtl_desc.usage = MTLTextureUsageRenderTarget;
    mtl_desc.resourceOptions = MTLResourceStorageModePrivate;
    mtl_desc.textureType = MTLTextureType2DMultisample;
    mtl_desc.sampleCount = (NSUInteger)img->cmn.sample_count;
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_image(_sg_image_t* img, const sg_image_desc* desc) {
    SOKOL_ASSERT(img && desc);
    const bool injected = (0 != desc->mtl_textures[0]);

    // first initialize all Metal resource pool slots to 'empty'
    for (int i = 0; i < SG_NUM_INFLIGHT_FRAMES; i++) {
        img->mtl.tex[i] = _sg_mtl_add_resource(nil);
    }

    // initialize a Metal texture descriptor
    MTLTextureDescriptor* mtl_desc = [[MTLTextureDescriptor alloc] init];
    if (!_sg_mtl_init_texdesc_common(mtl_desc, img)) {
        _SG_OBJC_RELEASE(mtl_desc);
        return SG_RESOURCESTATE_FAILED;
    }
    if (img->cmn.render_target) {
        if (img->cmn.sample_count > 1) {
            _sg_mtl_init_texdesc_rt_msaa(mtl_desc, img);
        } else {
            _sg_mtl_init_texdesc_rt(mtl_desc, img);
        }
    }
    for (int slot = 0; slot < img->cmn.num_slots; slot++) {
        id<MTLTexture> mtl_tex;
        if (injected) {
            SOKOL_ASSERT(desc->mtl_textures[slot]);
            mtl_tex = (__bridge id<MTLTexture>) desc->mtl_textures[slot];
        } else {
            mtl_tex = [_sg.mtl.device newTextureWithDescriptor:mtl_desc];
            if (nil == mtl_tex) {
                _SG_OBJC_RELEASE(mtl_desc);
                _SG_ERROR(METAL_CREATE_TEXTURE_FAILED);
                return SG_RESOURCESTATE_FAILED;
            }
            if ((img->cmn.usage == SG_USAGE_IMMUTABLE) && !img->cmn.render_target) {
                _sg_mtl_copy_image_data(img, mtl_tex, &desc->data);
            }
        }
        #if defined(SOKOL_DEBUG)
            if (desc->label) {
                mtl_tex.label = [NSString stringWithFormat:@"%s.%d", desc->label, slot];
            }
        #endif
        img->mtl.tex[slot] = _sg_mtl_add_resource(mtl_tex);
        _SG_OBJC_RELEASE(mtl_tex);
    }
    _SG_OBJC_RELEASE(mtl_desc);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_mtl_discard_image(_sg_image_t* img) {
    SOKOL_ASSERT(img);
    // it's valid to call release resource with a 'null resource'
    for (int slot = 0; slot < img->cmn.num_slots; slot++) {
        _sg_mtl_release_resource(_sg.frame_index, img->mtl.tex[slot]);
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(smp && desc);
    id<MTLSamplerState> mtl_smp;
    const bool injected = (0 != desc->mtl_sampler);
    if (injected) {
        SOKOL_ASSERT(desc->mtl_sampler);
        mtl_smp = (__bridge id<MTLSamplerState>) desc->mtl_sampler;
    } else {
        MTLSamplerDescriptor* mtl_desc = [[MTLSamplerDescriptor alloc] init];
        mtl_desc.sAddressMode = _sg_mtl_address_mode(desc->wrap_u);
        mtl_desc.tAddressMode = _sg_mtl_address_mode(desc->wrap_v);
        mtl_desc.rAddressMode = _sg_mtl_address_mode(desc->wrap_w);
        if (_sg.features.image_clamp_to_border) {
            if (@available(macOS 12.0, iOS 14.0, *)) {
                mtl_desc.borderColor  = _sg_mtl_border_color(desc->border_color);
            }
        }
        mtl_desc.minFilter = _sg_mtl_minmag_filter(desc->min_filter);
        mtl_desc.magFilter = _sg_mtl_minmag_filter(desc->mag_filter);
        mtl_desc.mipFilter = _sg_mtl_mipmap_filter(desc->mipmap_filter);
        mtl_desc.lodMinClamp = desc->min_lod;
        mtl_desc.lodMaxClamp = desc->max_lod;
        // FIXME: lodAverage?
        mtl_desc.maxAnisotropy = desc->max_anisotropy;
        mtl_desc.normalizedCoordinates = YES;
        mtl_desc.compareFunction = _sg_mtl_compare_func(desc->compare);
        #if defined(SOKOL_DEBUG)
            if (desc->label) {
                mtl_desc.label = [NSString stringWithUTF8String:desc->label];
            }
        #endif
        mtl_smp = [_sg.mtl.device newSamplerStateWithDescriptor:mtl_desc];
        _SG_OBJC_RELEASE(mtl_desc);
        if (nil == mtl_smp) {
            _SG_ERROR(METAL_CREATE_SAMPLER_FAILED);
            return SG_RESOURCESTATE_FAILED;
        }
    }
    smp->mtl.sampler_state = _sg_mtl_add_resource(mtl_smp);
    _SG_OBJC_RELEASE(mtl_smp);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_mtl_discard_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp);
    // it's valid to call release resource with a 'null resource'
    _sg_mtl_release_resource(_sg.frame_index, smp->mtl.sampler_state);
}

_SOKOL_PRIVATE id<MTLLibrary> _sg_mtl_compile_library(const char* src) {
    NSError* err = NULL;
    id<MTLLibrary> lib = [_sg.mtl.device
        newLibraryWithSource:[NSString stringWithUTF8String:src]
        options:nil
        error:&err
    ];
    if (err) {
        _SG_ERROR(METAL_SHADER_COMPILATION_FAILED);
        _SG_LOGMSG(METAL_SHADER_COMPILATION_OUTPUT, [err.localizedDescription UTF8String]);
    }
    return lib;
}

_SOKOL_PRIVATE id<MTLLibrary> _sg_mtl_library_from_bytecode(const void* ptr, size_t num_bytes) {
    NSError* err = NULL;
    dispatch_data_t lib_data = dispatch_data_create(ptr, num_bytes, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    id<MTLLibrary> lib = [_sg.mtl.device newLibraryWithData:lib_data error:&err];
    if (err) {
        _SG_ERROR(METAL_SHADER_CREATION_FAILED);
        _SG_LOGMSG(METAL_SHADER_COMPILATION_OUTPUT, [err.localizedDescription UTF8String]);
    }
    _SG_OBJC_RELEASE(lib_data);
    return lib;
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    SOKOL_ASSERT(shd && desc);

    // create metal library objects and lookup entry functions
    id<MTLLibrary> vs_lib = nil;
    id<MTLLibrary> fs_lib = nil;
    id<MTLFunction> vs_func = nil;
    id<MTLFunction> fs_func = nil;
    const char* vs_entry = desc->vs.entry;
    const char* fs_entry = desc->fs.entry;
    if (desc->vs.bytecode.ptr && desc->fs.bytecode.ptr) {
        // separate byte code provided
        vs_lib = _sg_mtl_library_from_bytecode(desc->vs.bytecode.ptr, desc->vs.bytecode.size);
        fs_lib = _sg_mtl_library_from_bytecode(desc->fs.bytecode.ptr, desc->fs.bytecode.size);
        if ((nil == vs_lib) || (nil == fs_lib)) {
            goto failed;
        }
        vs_func = [vs_lib newFunctionWithName:[NSString stringWithUTF8String:vs_entry]];
        fs_func = [fs_lib newFunctionWithName:[NSString stringWithUTF8String:fs_entry]];
    } else if (desc->vs.source && desc->fs.source) {
        // separate sources provided
        vs_lib = _sg_mtl_compile_library(desc->vs.source);
        fs_lib = _sg_mtl_compile_library(desc->fs.source);
        if ((nil == vs_lib) || (nil == fs_lib)) {
            goto failed;
        }
        vs_func = [vs_lib newFunctionWithName:[NSString stringWithUTF8String:vs_entry]];
        fs_func = [fs_lib newFunctionWithName:[NSString stringWithUTF8String:fs_entry]];
    } else {
        goto failed;
    }
    if (nil == vs_func) {
        _SG_ERROR(METAL_VERTEX_SHADER_ENTRY_NOT_FOUND);
        goto failed;
    }
    if (nil == fs_func) {
        _SG_ERROR(METAL_FRAGMENT_SHADER_ENTRY_NOT_FOUND);
        goto failed;
    }
    #if defined(SOKOL_DEBUG)
        if (desc->label) {
            vs_lib.label = [NSString stringWithFormat:@"%s.vs", desc->label];
            fs_lib.label = [NSString stringWithFormat:@"%s.fs", desc->label];
        }
    #endif
    // it is legal to call _sg_mtl_add_resource with a nil value, this will return a special 0xFFFFFFFF index
    shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_lib  = _sg_mtl_add_resource(vs_lib);
    _SG_OBJC_RELEASE(vs_lib);
    shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_lib  = _sg_mtl_add_resource(fs_lib);
    _SG_OBJC_RELEASE(fs_lib);
    shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_func = _sg_mtl_add_resource(vs_func);
    _SG_OBJC_RELEASE(vs_func);
    shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_func = _sg_mtl_add_resource(fs_func);
    _SG_OBJC_RELEASE(fs_func);
    return SG_RESOURCESTATE_VALID;
failed:
    if (vs_lib != nil) {
        _SG_OBJC_RELEASE(vs_lib);
    }
    if (fs_lib != nil) {
        _SG_OBJC_RELEASE(fs_lib);
    }
    if (vs_func != nil) {
        _SG_OBJC_RELEASE(vs_func);
    }
    if (fs_func != nil) {
        _SG_OBJC_RELEASE(fs_func);
    }
    return SG_RESOURCESTATE_FAILED;
}

_SOKOL_PRIVATE void _sg_mtl_discard_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd);
    // it is valid to call _sg_mtl_release_resource with a 'null resource'
    _sg_mtl_release_resource(_sg.frame_index, shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_func);
    _sg_mtl_release_resource(_sg.frame_index, shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_lib);
    _sg_mtl_release_resource(_sg.frame_index, shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_func);
    _sg_mtl_release_resource(_sg.frame_index, shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_lib);
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_pipeline(_sg_pipeline_t* pip, _sg_shader_t* shd, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(pip && shd && desc);
    SOKOL_ASSERT(desc->shader.id == shd->slot.id);

    pip->shader = shd;

    sg_primitive_type prim_type = desc->primitive_type;
    pip->mtl.prim_type = _sg_mtl_primitive_type(prim_type);
    pip->mtl.index_size = _sg_mtl_index_size(pip->cmn.index_type);
    if (SG_INDEXTYPE_NONE != pip->cmn.index_type) {
        pip->mtl.index_type = _sg_mtl_index_type(pip->cmn.index_type);
    }
    pip->mtl.cull_mode = _sg_mtl_cull_mode(desc->cull_mode);
    pip->mtl.winding = _sg_mtl_winding(desc->face_winding);
    pip->mtl.stencil_ref = desc->stencil.ref;

    // create vertex-descriptor
    MTLVertexDescriptor* vtx_desc = [MTLVertexDescriptor vertexDescriptor];
    for (NSUInteger attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        const sg_vertex_attr_state* a_state = &desc->layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        vtx_desc.attributes[attr_index].format = _sg_mtl_vertex_format(a_state->format);
        vtx_desc.attributes[attr_index].offset = (NSUInteger)a_state->offset;
        vtx_desc.attributes[attr_index].bufferIndex = (NSUInteger)(a_state->buffer_index + SG_MAX_SHADERSTAGE_UBS);
        pip->cmn.vertex_buffer_layout_active[a_state->buffer_index] = true;
    }
    for (NSUInteger layout_index = 0; layout_index < SG_MAX_VERTEX_BUFFERS; layout_index++) {
        if (pip->cmn.vertex_buffer_layout_active[layout_index]) {
            const sg_vertex_buffer_layout_state* l_state = &desc->layout.buffers[layout_index];
            const NSUInteger mtl_vb_slot = layout_index + SG_MAX_SHADERSTAGE_UBS;
            SOKOL_ASSERT(l_state->stride > 0);
            vtx_desc.layouts[mtl_vb_slot].stride = (NSUInteger)l_state->stride;
            vtx_desc.layouts[mtl_vb_slot].stepFunction = _sg_mtl_step_function(l_state->step_func);
            vtx_desc.layouts[mtl_vb_slot].stepRate = (NSUInteger)l_state->step_rate;
            if (SG_VERTEXSTEP_PER_INSTANCE == l_state->step_func) {
                // NOTE: not actually used in _sg_mtl_draw()
                pip->cmn.use_instanced_draw = true;
            }
        }
    }

    // render-pipeline descriptor
    MTLRenderPipelineDescriptor* rp_desc = [[MTLRenderPipelineDescriptor alloc] init];
    rp_desc.vertexDescriptor = vtx_desc;
    SOKOL_ASSERT(shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_func != _SG_MTL_INVALID_SLOT_INDEX);
    rp_desc.vertexFunction = _sg_mtl_id(shd->mtl.stage[SG_SHADERSTAGE_VS].mtl_func);
    SOKOL_ASSERT(shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_func != _SG_MTL_INVALID_SLOT_INDEX);
    rp_desc.fragmentFunction = _sg_mtl_id(shd->mtl.stage[SG_SHADERSTAGE_FS].mtl_func);
    rp_desc.rasterSampleCount = (NSUInteger)desc->sample_count;
    rp_desc.alphaToCoverageEnabled = desc->alpha_to_coverage_enabled;
    rp_desc.alphaToOneEnabled = NO;
    rp_desc.rasterizationEnabled = YES;
    rp_desc.depthAttachmentPixelFormat = _sg_mtl_pixel_format(desc->depth.pixel_format);
    if (desc->depth.pixel_format == SG_PIXELFORMAT_DEPTH_STENCIL) {
        rp_desc.stencilAttachmentPixelFormat = _sg_mtl_pixel_format(desc->depth.pixel_format);
    }
    if (@available(macOS 10.13, iOS 11.0, *)) {
        for (NSUInteger i = 0; i < (SG_MAX_SHADERSTAGE_UBS+SG_MAX_VERTEX_BUFFERS); i++) {
            rp_desc.vertexBuffers[i].mutability = MTLMutabilityImmutable;
        }
        for (NSUInteger i = 0; i < SG_MAX_SHADERSTAGE_UBS; i++) {
            rp_desc.fragmentBuffers[i].mutability = MTLMutabilityImmutable;
        }
    }
    for (NSUInteger i = 0; i < (NSUInteger)desc->color_count; i++) {
        SOKOL_ASSERT(i < SG_MAX_COLOR_ATTACHMENTS);
        const sg_color_target_state* cs = &desc->colors[i];
        rp_desc.colorAttachments[i].pixelFormat = _sg_mtl_pixel_format(cs->pixel_format);
        rp_desc.colorAttachments[i].writeMask = _sg_mtl_color_write_mask(cs->write_mask);
        rp_desc.colorAttachments[i].blendingEnabled = cs->blend.enabled;
        rp_desc.colorAttachments[i].alphaBlendOperation = _sg_mtl_blend_op(cs->blend.op_alpha);
        rp_desc.colorAttachments[i].rgbBlendOperation = _sg_mtl_blend_op(cs->blend.op_rgb);
        rp_desc.colorAttachments[i].destinationAlphaBlendFactor = _sg_mtl_blend_factor(cs->blend.dst_factor_alpha);
        rp_desc.colorAttachments[i].destinationRGBBlendFactor = _sg_mtl_blend_factor(cs->blend.dst_factor_rgb);
        rp_desc.colorAttachments[i].sourceAlphaBlendFactor = _sg_mtl_blend_factor(cs->blend.src_factor_alpha);
        rp_desc.colorAttachments[i].sourceRGBBlendFactor = _sg_mtl_blend_factor(cs->blend.src_factor_rgb);
    }
    #if defined(SOKOL_DEBUG)
        if (desc->label) {
            rp_desc.label = [NSString stringWithFormat:@"%s", desc->label];
        }
    #endif
    NSError* err = NULL;
    id<MTLRenderPipelineState> mtl_rps = [_sg.mtl.device newRenderPipelineStateWithDescriptor:rp_desc error:&err];
    _SG_OBJC_RELEASE(rp_desc);
    if (nil == mtl_rps) {
        SOKOL_ASSERT(err);
        _SG_ERROR(METAL_CREATE_RPS_FAILED);
        _SG_LOGMSG(METAL_CREATE_RPS_OUTPUT, [err.localizedDescription UTF8String]);
        return SG_RESOURCESTATE_FAILED;
    }
    pip->mtl.rps = _sg_mtl_add_resource(mtl_rps);
    _SG_OBJC_RELEASE(mtl_rps);

    // depth-stencil-state
    MTLDepthStencilDescriptor* ds_desc = [[MTLDepthStencilDescriptor alloc] init];
    ds_desc.depthCompareFunction = _sg_mtl_compare_func(desc->depth.compare);
    ds_desc.depthWriteEnabled = desc->depth.write_enabled;
    if (desc->stencil.enabled) {
        const sg_stencil_face_state* sb = &desc->stencil.back;
        ds_desc.backFaceStencil = [[MTLStencilDescriptor alloc] init];
        ds_desc.backFaceStencil.stencilFailureOperation = _sg_mtl_stencil_op(sb->fail_op);
        ds_desc.backFaceStencil.depthFailureOperation = _sg_mtl_stencil_op(sb->depth_fail_op);
        ds_desc.backFaceStencil.depthStencilPassOperation = _sg_mtl_stencil_op(sb->pass_op);
        ds_desc.backFaceStencil.stencilCompareFunction = _sg_mtl_compare_func(sb->compare);
        ds_desc.backFaceStencil.readMask = desc->stencil.read_mask;
        ds_desc.backFaceStencil.writeMask = desc->stencil.write_mask;
        const sg_stencil_face_state* sf = &desc->stencil.front;
        ds_desc.frontFaceStencil = [[MTLStencilDescriptor alloc] init];
        ds_desc.frontFaceStencil.stencilFailureOperation = _sg_mtl_stencil_op(sf->fail_op);
        ds_desc.frontFaceStencil.depthFailureOperation = _sg_mtl_stencil_op(sf->depth_fail_op);
        ds_desc.frontFaceStencil.depthStencilPassOperation = _sg_mtl_stencil_op(sf->pass_op);
        ds_desc.frontFaceStencil.stencilCompareFunction = _sg_mtl_compare_func(sf->compare);
        ds_desc.frontFaceStencil.readMask = desc->stencil.read_mask;
        ds_desc.frontFaceStencil.writeMask = desc->stencil.write_mask;
    }
    #if defined(SOKOL_DEBUG)
        if (desc->label) {
            ds_desc.label = [NSString stringWithFormat:@"%s.dss", desc->label];
        }
    #endif
    id<MTLDepthStencilState> mtl_dss = [_sg.mtl.device newDepthStencilStateWithDescriptor:ds_desc];
    _SG_OBJC_RELEASE(ds_desc);
    if (nil == mtl_dss) {
        _SG_ERROR(METAL_CREATE_DSS_FAILED);
        return SG_RESOURCESTATE_FAILED;
    }
    pip->mtl.dss = _sg_mtl_add_resource(mtl_dss);
    _SG_OBJC_RELEASE(mtl_dss);
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_mtl_discard_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    // it's valid to call release resource with a 'null resource'
    _sg_mtl_release_resource(_sg.frame_index, pip->mtl.rps);
    _sg_mtl_release_resource(_sg.frame_index, pip->mtl.dss);
}

_SOKOL_PRIVATE sg_resource_state _sg_mtl_create_attachments(_sg_attachments_t* atts, _sg_image_t** color_images, _sg_image_t** resolve_images, _sg_image_t* ds_img, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(atts && desc);
    SOKOL_ASSERT(color_images && resolve_images);

    // copy image pointers
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const sg_attachment_desc* color_desc = &desc->colors[i];
        _SOKOL_UNUSED(color_desc);
        SOKOL_ASSERT(color_desc->image.id != SG_INVALID_ID);
        SOKOL_ASSERT(0 == atts->mtl.colors[i].image);
        SOKOL_ASSERT(color_images[i] && (color_images[i]->slot.id == color_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_color_format(color_images[i]->cmn.pixel_format));
        atts->mtl.colors[i].image = color_images[i];

        const sg_attachment_desc* resolve_desc = &desc->resolves[i];
        if (resolve_desc->image.id != SG_INVALID_ID) {
            SOKOL_ASSERT(0 == atts->mtl.resolves[i].image);
            SOKOL_ASSERT(resolve_images[i] && (resolve_images[i]->slot.id == resolve_desc->image.id));
            SOKOL_ASSERT(color_images[i] && (color_images[i]->cmn.pixel_format == resolve_images[i]->cmn.pixel_format));
            atts->mtl.resolves[i].image = resolve_images[i];
        }
    }
    SOKOL_ASSERT(0 == atts->mtl.depth_stencil.image);
    const sg_attachment_desc* ds_desc = &desc->depth_stencil;
    if (ds_desc->image.id != SG_INVALID_ID) {
        SOKOL_ASSERT(ds_img && (ds_img->slot.id == ds_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_depth_format(ds_img->cmn.pixel_format));
        atts->mtl.depth_stencil.image = ds_img;
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_mtl_discard_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    _SOKOL_UNUSED(atts);
}

_SOKOL_PRIVATE _sg_image_t* _sg_mtl_attachments_color_image(const _sg_attachments_t* atts, int index) {
    // NOTE: may return null
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->mtl.colors[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_mtl_attachments_resolve_image(const _sg_attachments_t* atts, int index) {
    // NOTE: may return null
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->mtl.resolves[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_mtl_attachments_ds_image(const _sg_attachments_t* atts) {
    // NOTE: may return null
    SOKOL_ASSERT(atts);
    return atts->mtl.depth_stencil.image;
}

_SOKOL_PRIVATE void _sg_mtl_begin_pass(const sg_pass* pass) {
    SOKOL_ASSERT(pass);
    SOKOL_ASSERT(_sg.mtl.cmd_queue);
    SOKOL_ASSERT(nil == _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(nil == _sg.mtl.cur_drawable);
    _sg_mtl_clear_state_cache();

    const _sg_attachments_t* atts = _sg.cur_pass.atts;
    const sg_swapchain* swapchain = &pass->swapchain;
    const sg_pass_action* action = &pass->action;

    /*
        if this is the first pass in the frame, create command buffers

        NOTE: we're creating two command buffers here, one with unretained references
        for storing the regular commands, and one with retained references for
        storing the presentDrawable call (this needs to hold on the drawable until
        presentation has happened - and the easiest way to do this is to let the
        command buffer manage the lifetime of the drawable).

        Also see: https://github.com/floooh/sokol/issues/762
    */
    if (nil == _sg.mtl.cmd_buffer) {
        // block until the oldest frame in flight has finished
        dispatch_semaphore_wait(_sg.mtl.sem, DISPATCH_TIME_FOREVER);
        if (_sg.desc.mtl_use_command_buffer_with_retained_references) {
            _sg.mtl.cmd_buffer = [_sg.mtl.cmd_queue commandBuffer];
        } else {
            _sg.mtl.cmd_buffer = [_sg.mtl.cmd_queue commandBufferWithUnretainedReferences];
        }
        [_sg.mtl.cmd_buffer enqueue];
        [_sg.mtl.cmd_buffer addCompletedHandler:^(id<MTLCommandBuffer> cmd_buf) {
            // NOTE: this code is called on a different thread!
            _SOKOL_UNUSED(cmd_buf);
            dispatch_semaphore_signal(_sg.mtl.sem);
        }];
    }

    // if this is first pass in frame, get uniform buffer base pointer
    if (0 == _sg.mtl.cur_ub_base_ptr) {
        _sg.mtl.cur_ub_base_ptr = (uint8_t*)[_sg.mtl.uniform_buffers[_sg.mtl.cur_frame_rotate_index] contents];
    }

    MTLRenderPassDescriptor* pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];
    SOKOL_ASSERT(pass_desc);
    if (atts) {
        // setup pass descriptor for offscreen rendering
        SOKOL_ASSERT(atts->slot.state == SG_RESOURCESTATE_VALID);
        for (NSUInteger i = 0; i < (NSUInteger)atts->cmn.num_colors; i++) {
            const _sg_attachment_common_t* cmn_color_att = &atts->cmn.colors[i];
            const _sg_mtl_attachment_t* mtl_color_att = &atts->mtl.colors[i];
            const _sg_image_t* color_att_img = mtl_color_att->image;
            const _sg_attachment_common_t* cmn_resolve_att = &atts->cmn.resolves[i];
            const _sg_mtl_attachment_t* mtl_resolve_att = &atts->mtl.resolves[i];
            const _sg_image_t* resolve_att_img = mtl_resolve_att->image;
            SOKOL_ASSERT(color_att_img->slot.state == SG_RESOURCESTATE_VALID);
            SOKOL_ASSERT(color_att_img->slot.id == cmn_color_att->image_id.id);
            SOKOL_ASSERT(color_att_img->mtl.tex[color_att_img->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            pass_desc.colorAttachments[i].loadAction = _sg_mtl_load_action(action->colors[i].load_action);
            pass_desc.colorAttachments[i].storeAction = _sg_mtl_store_action(action->colors[i].store_action, resolve_att_img != 0);
            sg_color c = action->colors[i].clear_value;
            pass_desc.colorAttachments[i].clearColor = MTLClearColorMake(c.r, c.g, c.b, c.a);
            pass_desc.colorAttachments[i].texture = _sg_mtl_id(color_att_img->mtl.tex[color_att_img->cmn.active_slot]);
            pass_desc.colorAttachments[i].level = (NSUInteger)cmn_color_att->mip_level;
            switch (color_att_img->cmn.type) {
                case SG_IMAGETYPE_CUBE:
                case SG_IMAGETYPE_ARRAY:
                    pass_desc.colorAttachments[i].slice = (NSUInteger)cmn_color_att->slice;
                    break;
                case SG_IMAGETYPE_3D:
                    pass_desc.colorAttachments[i].depthPlane = (NSUInteger)cmn_color_att->slice;
                    break;
                default: break;
            }
            if (resolve_att_img) {
                SOKOL_ASSERT(resolve_att_img->slot.state == SG_RESOURCESTATE_VALID);
                SOKOL_ASSERT(resolve_att_img->slot.id == cmn_resolve_att->image_id.id);
                SOKOL_ASSERT(resolve_att_img->mtl.tex[resolve_att_img->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
                pass_desc.colorAttachments[i].resolveTexture = _sg_mtl_id(resolve_att_img->mtl.tex[resolve_att_img->cmn.active_slot]);
                pass_desc.colorAttachments[i].resolveLevel = (NSUInteger)cmn_resolve_att->mip_level;
                switch (resolve_att_img->cmn.type) {
                    case SG_IMAGETYPE_CUBE:
                    case SG_IMAGETYPE_ARRAY:
                        pass_desc.colorAttachments[i].resolveSlice = (NSUInteger)cmn_resolve_att->slice;
                        break;
                    case SG_IMAGETYPE_3D:
                        pass_desc.colorAttachments[i].resolveDepthPlane = (NSUInteger)cmn_resolve_att->slice;
                        break;
                    default: break;
                }
            }
        }
        const _sg_image_t* ds_att_img = atts->mtl.depth_stencil.image;
        if (0 != ds_att_img) {
            SOKOL_ASSERT(ds_att_img->slot.state == SG_RESOURCESTATE_VALID);
            SOKOL_ASSERT(ds_att_img->slot.id == atts->cmn.depth_stencil.image_id.id);
            SOKOL_ASSERT(ds_att_img->mtl.tex[ds_att_img->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            pass_desc.depthAttachment.texture = _sg_mtl_id(ds_att_img->mtl.tex[ds_att_img->cmn.active_slot]);
            pass_desc.depthAttachment.loadAction = _sg_mtl_load_action(action->depth.load_action);
            pass_desc.depthAttachment.storeAction = _sg_mtl_store_action(action->depth.store_action, false);
            pass_desc.depthAttachment.clearDepth = action->depth.clear_value;
            const _sg_attachment_common_t* cmn_ds_att = &atts->cmn.depth_stencil;
            switch (ds_att_img->cmn.type) {
                case SG_IMAGETYPE_CUBE:
                case SG_IMAGETYPE_ARRAY:
                    pass_desc.depthAttachment.slice = (NSUInteger)cmn_ds_att->slice;
                    break;
                case SG_IMAGETYPE_3D:
                    pass_desc.depthAttachment.resolveDepthPlane = (NSUInteger)cmn_ds_att->slice;
                    break;
                default: break;
            }
            if (_sg_is_depth_stencil_format(ds_att_img->cmn.pixel_format)) {
                pass_desc.stencilAttachment.texture = _sg_mtl_id(ds_att_img->mtl.tex[ds_att_img->cmn.active_slot]);
                pass_desc.stencilAttachment.loadAction = _sg_mtl_load_action(action->stencil.load_action);
                pass_desc.stencilAttachment.storeAction = _sg_mtl_store_action(action->depth.store_action, false);
                pass_desc.stencilAttachment.clearStencil = action->stencil.clear_value;
                switch (ds_att_img->cmn.type) {
                    case SG_IMAGETYPE_CUBE:
                    case SG_IMAGETYPE_ARRAY:
                        pass_desc.stencilAttachment.slice = (NSUInteger)cmn_ds_att->slice;
                        break;
                    case SG_IMAGETYPE_3D:
                        pass_desc.stencilAttachment.resolveDepthPlane = (NSUInteger)cmn_ds_att->slice;
                        break;
                    default: break;
                }
            }
        }
    } else {
        // setup pass descriptor for swapchain rendering
        //
        // NOTE: at least in macOS Sonoma this no longer seems to be the case, the
        // current drawable is also valid in a minimized window
        // ===
        // an MTKView current_drawable will not be valid if window is minimized, don't do any rendering in this case
        if (0 == swapchain->metal.current_drawable) {
            _sg.cur_pass.valid = false;
            return;
        }
        // pin the swapchain resources into memory so that they outlive their command buffer
        // (this is necessary because the command buffer doesn't retain references)
        int pass_desc_ref = _sg_mtl_add_resource(pass_desc);
        _sg_mtl_release_resource(_sg.frame_index, pass_desc_ref);

        _sg.mtl.cur_drawable = (__bridge id<CAMetalDrawable>) swapchain->metal.current_drawable;
        if (swapchain->sample_count > 1) {
            // multi-sampling: render into msaa texture, resolve into drawable texture
            id<MTLTexture> msaa_tex = (__bridge id<MTLTexture>) swapchain->metal.msaa_color_texture;
            SOKOL_ASSERT(msaa_tex != nil);
            pass_desc.colorAttachments[0].texture = msaa_tex;
            pass_desc.colorAttachments[0].resolveTexture = _sg.mtl.cur_drawable.texture;
            pass_desc.colorAttachments[0].storeAction = MTLStoreActionMultisampleResolve;
        } else {
            // non-msaa: render into current_drawable
            pass_desc.colorAttachments[0].texture = _sg.mtl.cur_drawable.texture;
            pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
        }
        pass_desc.colorAttachments[0].loadAction = _sg_mtl_load_action(action->colors[0].load_action);
        const sg_color c = action->colors[0].clear_value;
        pass_desc.colorAttachments[0].clearColor = MTLClearColorMake(c.r, c.g, c.b, c.a);

        // optional depth-stencil texture
        if (swapchain->metal.depth_stencil_texture) {
            id<MTLTexture> ds_tex = (__bridge id<MTLTexture>) swapchain->metal.depth_stencil_texture;
            SOKOL_ASSERT(ds_tex != nil);
            pass_desc.depthAttachment.texture = ds_tex;
            pass_desc.depthAttachment.storeAction = MTLStoreActionDontCare;
            pass_desc.depthAttachment.loadAction = _sg_mtl_load_action(action->depth.load_action);
            pass_desc.depthAttachment.clearDepth = action->depth.clear_value;
            if (_sg_is_depth_stencil_format(swapchain->depth_format)) {
                pass_desc.stencilAttachment.texture = ds_tex;
                pass_desc.stencilAttachment.storeAction = MTLStoreActionDontCare;
                pass_desc.stencilAttachment.loadAction = _sg_mtl_load_action(action->stencil.load_action);
                pass_desc.stencilAttachment.clearStencil = action->stencil.clear_value;
            }
        }
    }

    // NOTE: at least in macOS Sonoma, the following is no longer the case, a valid
    // render command encoder is also returned in a minimized window
    // ===
    // create a render command encoder, this might return nil if window is minimized
    _sg.mtl.cmd_encoder = [_sg.mtl.cmd_buffer renderCommandEncoderWithDescriptor:pass_desc];
    if (nil == _sg.mtl.cmd_encoder) {
        _sg.cur_pass.valid = false;
        return;
    }

    #if defined(SOKOL_DEBUG)
        if (pass->label) {
            _sg.mtl.cmd_encoder.label = [NSString stringWithUTF8String:pass->label];
        }
    #endif

    // bind the global uniform buffer, this only happens once per pass
    _sg_mtl_bind_uniform_buffers();
}

_SOKOL_PRIVATE void _sg_mtl_end_pass(void) {
    if (nil != _sg.mtl.cmd_encoder) {
        [_sg.mtl.cmd_encoder endEncoding];
        // NOTE: MTLRenderCommandEncoder is autoreleased
        _sg.mtl.cmd_encoder = nil;
    }
    // if this is a swapchain pass, present the drawable
    if (nil != _sg.mtl.cur_drawable) {
        [_sg.mtl.cmd_buffer presentDrawable:_sg.mtl.cur_drawable];
        _sg.mtl.cur_drawable = nil;
    }
}

_SOKOL_PRIVATE void _sg_mtl_commit(void) {
    SOKOL_ASSERT(nil == _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(nil != _sg.mtl.cmd_buffer);

    // commit the frame's command buffer
    [_sg.mtl.cmd_buffer commit];

    // garbage-collect resources pending for release
    _sg_mtl_garbage_collect(_sg.frame_index);

    // rotate uniform buffer slot
    if (++_sg.mtl.cur_frame_rotate_index >= SG_NUM_INFLIGHT_FRAMES) {
        _sg.mtl.cur_frame_rotate_index = 0;
    }
    _sg.mtl.cur_ub_offset = 0;
    _sg.mtl.cur_ub_base_ptr = 0;
    // NOTE: MTLCommandBuffer is autoreleased
    _sg.mtl.cmd_buffer = nil;
}

_SOKOL_PRIVATE void _sg_mtl_apply_viewport(int x, int y, int w, int h, bool origin_top_left) {
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(_sg.cur_pass.height > 0);
    MTLViewport vp;
    vp.originX = (double) x;
    vp.originY = (double) (origin_top_left ? y : (_sg.cur_pass.height - (y + h)));
    vp.width   = (double) w;
    vp.height  = (double) h;
    vp.znear   = 0.0;
    vp.zfar    = 1.0;
    [_sg.mtl.cmd_encoder setViewport:vp];
}

_SOKOL_PRIVATE void _sg_mtl_apply_scissor_rect(int x, int y, int w, int h, bool origin_top_left) {
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(_sg.cur_pass.width > 0);
    SOKOL_ASSERT(_sg.cur_pass.height > 0);
    // clip against framebuffer rect
    const _sg_recti_t clip = _sg_clipi(x, y, w, h, _sg.cur_pass.width, _sg.cur_pass.height);
    MTLScissorRect r;
    r.x = (NSUInteger)clip.x;
    r.y = (NSUInteger) (origin_top_left ? clip.y : (_sg.cur_pass.height - (clip.y + clip.h)));
    r.width = (NSUInteger)clip.w;
    r.height = (NSUInteger)clip.h;
    [_sg.mtl.cmd_encoder setScissorRect:r];
}

_SOKOL_PRIVATE void _sg_mtl_apply_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    SOKOL_ASSERT(pip->shader && (pip->cmn.shader_id.id == pip->shader->slot.id));
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);

    if (_sg.mtl.state_cache.cur_pipeline_id.id != pip->slot.id) {
        _sg.mtl.state_cache.cur_pipeline = pip;
        _sg.mtl.state_cache.cur_pipeline_id.id = pip->slot.id;
        sg_color c = pip->cmn.blend_color;
        [_sg.mtl.cmd_encoder setBlendColorRed:c.r green:c.g blue:c.b alpha:c.a];
        _sg_stats_add(metal.pipeline.num_set_blend_color, 1);
        [_sg.mtl.cmd_encoder setCullMode:pip->mtl.cull_mode];
        _sg_stats_add(metal.pipeline.num_set_cull_mode, 1);
        [_sg.mtl.cmd_encoder setFrontFacingWinding:pip->mtl.winding];
        _sg_stats_add(metal.pipeline.num_set_front_facing_winding, 1);
        [_sg.mtl.cmd_encoder setStencilReferenceValue:pip->mtl.stencil_ref];
        _sg_stats_add(metal.pipeline.num_set_stencil_reference_value, 1);
        [_sg.mtl.cmd_encoder setDepthBias:pip->cmn.depth.bias slopeScale:pip->cmn.depth.bias_slope_scale clamp:pip->cmn.depth.bias_clamp];
        _sg_stats_add(metal.pipeline.num_set_depth_bias, 1);
        SOKOL_ASSERT(pip->mtl.rps != _SG_MTL_INVALID_SLOT_INDEX);
        [_sg.mtl.cmd_encoder setRenderPipelineState:_sg_mtl_id(pip->mtl.rps)];
        _sg_stats_add(metal.pipeline.num_set_render_pipeline_state, 1);
        SOKOL_ASSERT(pip->mtl.dss != _SG_MTL_INVALID_SLOT_INDEX);
        [_sg.mtl.cmd_encoder setDepthStencilState:_sg_mtl_id(pip->mtl.dss)];
        _sg_stats_add(metal.pipeline.num_set_depth_stencil_state, 1);
    }
}

_SOKOL_PRIVATE bool _sg_mtl_apply_bindings(_sg_bindings_t* bnd) {
    SOKOL_ASSERT(bnd);
    SOKOL_ASSERT(bnd->pip);
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);

    // store index buffer binding, this will be needed later in sg_draw()
    _sg.mtl.state_cache.cur_indexbuffer = bnd->ib;
    _sg.mtl.state_cache.cur_indexbuffer_offset = bnd->ib_offset;
    if (bnd->ib) {
        SOKOL_ASSERT(bnd->pip->cmn.index_type != SG_INDEXTYPE_NONE);
        _sg.mtl.state_cache.cur_indexbuffer_id.id = bnd->ib->slot.id;
    } else {
        SOKOL_ASSERT(bnd->pip->cmn.index_type == SG_INDEXTYPE_NONE);
        _sg.mtl.state_cache.cur_indexbuffer_id.id = SG_INVALID_ID;
    }

    // apply vertex buffers
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_vbs; slot++) {
        const _sg_buffer_t* vb = bnd->vbs[slot];
        const int vb_offset = bnd->vb_offsets[slot];
        if ((_sg.mtl.state_cache.cur_vertexbuffer_ids[slot].id != vb->slot.id) ||
            (_sg.mtl.state_cache.cur_vertexbuffer_offsets[slot] != vb_offset))
        {
            _sg.mtl.state_cache.cur_vertexbuffer_offsets[slot] = vb_offset;
            const NSUInteger mtl_slot = SG_MAX_SHADERSTAGE_UBS + slot;
            if (_sg.mtl.state_cache.cur_vertexbuffer_ids[slot].id != vb->slot.id) {
                _sg.mtl.state_cache.cur_vertexbuffer_ids[slot].id = vb->slot.id;
                SOKOL_ASSERT(vb->mtl.buf[vb->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
                [_sg.mtl.cmd_encoder setVertexBuffer:_sg_mtl_id(vb->mtl.buf[vb->cmn.active_slot])
                    offset:(NSUInteger)vb_offset
                    atIndex:mtl_slot];
            } else {
                [_sg.mtl.cmd_encoder setVertexBufferOffset:(NSUInteger)vb_offset atIndex:mtl_slot];
            }
            _sg_stats_add(metal.bindings.num_set_vertex_buffer, 1);
        }
    }

    // apply vertex stage images
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_vs_imgs; slot++) {
        const _sg_image_t* img = bnd->vs_imgs[slot];
        if (_sg.mtl.state_cache.cur_vs_image_ids[slot].id != img->slot.id) {
            _sg.mtl.state_cache.cur_vs_image_ids[slot].id = img->slot.id;
            SOKOL_ASSERT(img->mtl.tex[img->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            [_sg.mtl.cmd_encoder setVertexTexture:_sg_mtl_id(img->mtl.tex[img->cmn.active_slot]) atIndex:slot];
            _sg_stats_add(metal.bindings.num_set_vertex_texture, 1);
        }
    }

    // apply vertex stage samplers
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_vs_smps; slot++) {
        const _sg_sampler_t* smp = bnd->vs_smps[slot];
        if (_sg.mtl.state_cache.cur_vs_sampler_ids[slot].id != smp->slot.id) {
            _sg.mtl.state_cache.cur_vs_sampler_ids[slot].id = smp->slot.id;
            SOKOL_ASSERT(smp->mtl.sampler_state != _SG_MTL_INVALID_SLOT_INDEX);
            [_sg.mtl.cmd_encoder setVertexSamplerState:_sg_mtl_id(smp->mtl.sampler_state) atIndex:slot];
            _sg_stats_add(metal.bindings.num_set_vertex_sampler_state, 1);
        }
    }

    // apply vertex stage storage buffers
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_vs_sbufs; slot++) {
        const _sg_buffer_t* sbuf = bnd->vs_sbufs[slot];
        if (_sg.mtl.state_cache.cur_vs_storagebuffer_ids[slot].id != sbuf->slot.id) {
            _sg.mtl.state_cache.cur_vs_storagebuffer_ids[slot].id = sbuf->slot.id;
            SOKOL_ASSERT(sbuf->mtl.buf[sbuf->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            const NSUInteger mtl_slot = SG_MAX_SHADERSTAGE_UBS + SG_MAX_VERTEX_BUFFERS + slot;
            [_sg.mtl.cmd_encoder setVertexBuffer:_sg_mtl_id(sbuf->mtl.buf[sbuf->cmn.active_slot]) offset:0 atIndex:mtl_slot];
            _sg_stats_add(metal.bindings.num_set_vertex_buffer, 1);
        }
    }

    // apply fragment stage images
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_fs_imgs; slot++) {
        const _sg_image_t* img = bnd->fs_imgs[slot];
        if (_sg.mtl.state_cache.cur_fs_image_ids[slot].id != img->slot.id) {
            _sg.mtl.state_cache.cur_fs_image_ids[slot].id = img->slot.id;
            SOKOL_ASSERT(img->mtl.tex[img->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            [_sg.mtl.cmd_encoder setFragmentTexture:_sg_mtl_id(img->mtl.tex[img->cmn.active_slot]) atIndex:slot];
            _sg_stats_add(metal.bindings.num_set_fragment_texture, 1);
        }
    }

    // apply fragment stage samplers
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_fs_smps; slot++) {
        const _sg_sampler_t* smp = bnd->fs_smps[slot];
        if (_sg.mtl.state_cache.cur_fs_sampler_ids[slot].id != smp->slot.id) {
            _sg.mtl.state_cache.cur_fs_sampler_ids[slot].id = smp->slot.id;
            SOKOL_ASSERT(smp->mtl.sampler_state != _SG_MTL_INVALID_SLOT_INDEX);
            [_sg.mtl.cmd_encoder setFragmentSamplerState:_sg_mtl_id(smp->mtl.sampler_state) atIndex:slot];
            _sg_stats_add(metal.bindings.num_set_fragment_sampler_state, 1);
        }
    }

    // apply fragment stage storage buffers
    for (NSUInteger slot = 0; slot < (NSUInteger)bnd->num_fs_sbufs; slot++) {
        const _sg_buffer_t* sbuf = bnd->fs_sbufs[slot];
        if (_sg.mtl.state_cache.cur_fs_storagebuffer_ids[slot].id != sbuf->slot.id) {
            _sg.mtl.state_cache.cur_fs_storagebuffer_ids[slot].id = sbuf->slot.id;
            SOKOL_ASSERT(sbuf->mtl.buf[sbuf->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
            const NSUInteger mtl_slot = SG_MAX_SHADERSTAGE_UBS + slot;
            [_sg.mtl.cmd_encoder setFragmentBuffer:_sg_mtl_id(sbuf->mtl.buf[sbuf->cmn.active_slot]) offset:0 atIndex:mtl_slot];
            _sg_stats_add(metal.bindings.num_set_fragment_buffer, 1);
        }
    }

    return true;
}

_SOKOL_PRIVATE void _sg_mtl_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(((size_t)_sg.mtl.cur_ub_offset + data->size) <= (size_t)_sg.mtl.ub_size);
    SOKOL_ASSERT((_sg.mtl.cur_ub_offset & (_SG_MTL_UB_ALIGN-1)) == 0);
    SOKOL_ASSERT(_sg.mtl.state_cache.cur_pipeline && _sg.mtl.state_cache.cur_pipeline->shader);
    SOKOL_ASSERT(_sg.mtl.state_cache.cur_pipeline->slot.id == _sg.mtl.state_cache.cur_pipeline_id.id);
    SOKOL_ASSERT(_sg.mtl.state_cache.cur_pipeline->shader->slot.id == _sg.mtl.state_cache.cur_pipeline->cmn.shader_id.id);
    SOKOL_ASSERT(ub_index < _sg.mtl.state_cache.cur_pipeline->shader->cmn.stage[stage_index].num_uniform_blocks);
    SOKOL_ASSERT(data->size == _sg.mtl.state_cache.cur_pipeline->shader->cmn.stage[stage_index].uniform_blocks[ub_index].size);

    // copy to global uniform buffer, record offset into cmd encoder, and advance offset
    uint8_t* dst = &_sg.mtl.cur_ub_base_ptr[_sg.mtl.cur_ub_offset];
    memcpy(dst, data->ptr, data->size);
    if (stage_index == SG_SHADERSTAGE_VS) {
        [_sg.mtl.cmd_encoder setVertexBufferOffset:(NSUInteger)_sg.mtl.cur_ub_offset atIndex:(NSUInteger)ub_index];
        _sg_stats_add(metal.uniforms.num_set_vertex_buffer_offset, 1);
    } else {
        [_sg.mtl.cmd_encoder setFragmentBufferOffset:(NSUInteger)_sg.mtl.cur_ub_offset atIndex:(NSUInteger)ub_index];
        _sg_stats_add(metal.uniforms.num_set_fragment_buffer_offset, 1);
    }
    _sg.mtl.cur_ub_offset = _sg_roundup(_sg.mtl.cur_ub_offset + (int)data->size, _SG_MTL_UB_ALIGN);
}

_SOKOL_PRIVATE void _sg_mtl_draw(int base_element, int num_elements, int num_instances) {
    SOKOL_ASSERT(nil != _sg.mtl.cmd_encoder);
    SOKOL_ASSERT(_sg.mtl.state_cache.cur_pipeline && (_sg.mtl.state_cache.cur_pipeline->slot.id == _sg.mtl.state_cache.cur_pipeline_id.id));
    if (SG_INDEXTYPE_NONE != _sg.mtl.state_cache.cur_pipeline->cmn.index_type) {
        // indexed rendering
        SOKOL_ASSERT(_sg.mtl.state_cache.cur_indexbuffer && (_sg.mtl.state_cache.cur_indexbuffer->slot.id == _sg.mtl.state_cache.cur_indexbuffer_id.id));
        const _sg_buffer_t* ib = _sg.mtl.state_cache.cur_indexbuffer;
        SOKOL_ASSERT(ib->mtl.buf[ib->cmn.active_slot] != _SG_MTL_INVALID_SLOT_INDEX);
        const NSUInteger index_buffer_offset = (NSUInteger) (_sg.mtl.state_cache.cur_indexbuffer_offset + base_element * _sg.mtl.state_cache.cur_pipeline->mtl.index_size);
        [_sg.mtl.cmd_encoder drawIndexedPrimitives:_sg.mtl.state_cache.cur_pipeline->mtl.prim_type
            indexCount:(NSUInteger)num_elements
            indexType:_sg.mtl.state_cache.cur_pipeline->mtl.index_type
            indexBuffer:_sg_mtl_id(ib->mtl.buf[ib->cmn.active_slot])
            indexBufferOffset:index_buffer_offset
            instanceCount:(NSUInteger)num_instances];
    } else {
        // non-indexed rendering
        [_sg.mtl.cmd_encoder drawPrimitives:_sg.mtl.state_cache.cur_pipeline->mtl.prim_type
            vertexStart:(NSUInteger)base_element
            vertexCount:(NSUInteger)num_elements
            instanceCount:(NSUInteger)num_instances];
    }
}

_SOKOL_PRIVATE void _sg_mtl_update_buffer(_sg_buffer_t* buf, const sg_range* data) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
        buf->cmn.active_slot = 0;
    }
    __unsafe_unretained id<MTLBuffer> mtl_buf = _sg_mtl_id(buf->mtl.buf[buf->cmn.active_slot]);
    void* dst_ptr = [mtl_buf contents];
    memcpy(dst_ptr, data->ptr, data->size);
    #if defined(_SG_TARGET_MACOS)
    if (_sg_mtl_resource_options_storage_mode_managed_or_shared() == MTLResourceStorageModeManaged) {
        [mtl_buf didModifyRange:NSMakeRange(0, data->size)];
    }
    #endif
}

_SOKOL_PRIVATE void _sg_mtl_append_buffer(_sg_buffer_t* buf, const sg_range* data, bool new_frame) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    if (new_frame) {
        if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
            buf->cmn.active_slot = 0;
        }
    }
    __unsafe_unretained id<MTLBuffer> mtl_buf = _sg_mtl_id(buf->mtl.buf[buf->cmn.active_slot]);
    uint8_t* dst_ptr = (uint8_t*) [mtl_buf contents];
    dst_ptr += buf->cmn.append_pos;
    memcpy(dst_ptr, data->ptr, data->size);
    #if defined(_SG_TARGET_MACOS)
    if (_sg_mtl_resource_options_storage_mode_managed_or_shared() == MTLResourceStorageModeManaged) {
        [mtl_buf didModifyRange:NSMakeRange((NSUInteger)buf->cmn.append_pos, (NSUInteger)data->size)];
    }
    #endif
}

_SOKOL_PRIVATE void _sg_mtl_update_image(_sg_image_t* img, const sg_image_data* data) {
    SOKOL_ASSERT(img && data);
    if (++img->cmn.active_slot >= img->cmn.num_slots) {
        img->cmn.active_slot = 0;
    }
    __unsafe_unretained id<MTLTexture> mtl_tex = _sg_mtl_id(img->mtl.tex[img->cmn.active_slot]);
    _sg_mtl_copy_image_data(img, mtl_tex, data);
}

_SOKOL_PRIVATE void _sg_mtl_push_debug_group(const char* name) {
    SOKOL_ASSERT(name);
    if (_sg.mtl.cmd_encoder) {
        [_sg.mtl.cmd_encoder pushDebugGroup:[NSString stringWithUTF8String:name]];
    }
}

_SOKOL_PRIVATE void _sg_mtl_pop_debug_group(void) {
    if (_sg.mtl.cmd_encoder) {
        [_sg.mtl.cmd_encoder popDebugGroup];
    }
}
