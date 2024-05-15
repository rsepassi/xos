//  ██████  ██████  ███████ ███    ██  ██████  ██          ██████   █████   ██████ ██   ██ ███████ ███    ██ ██████
// ██    ██ ██   ██ ██      ████   ██ ██       ██          ██   ██ ██   ██ ██      ██  ██  ██      ████   ██ ██   ██
// ██    ██ ██████  █████   ██ ██  ██ ██   ███ ██          ██████  ███████ ██      █████   █████   ██ ██  ██ ██   ██
// ██    ██ ██      ██      ██  ██ ██ ██    ██ ██          ██   ██ ██   ██ ██      ██  ██  ██      ██  ██ ██ ██   ██
//  ██████  ██      ███████ ██   ████  ██████  ███████     ██████  ██   ██  ██████ ██   ██ ███████ ██   ████ ██████
//
// >>opengl backend
// optional GL loader for win32
#if defined(_SOKOL_USE_WIN32_GL_LOADER)

#ifndef SG_GL_FUNCS_EXT
#define SG_GL_FUNCS_EXT
#endif

// X Macro list of GL function names and signatures
#define _SG_GL_FUNCS \
    SG_GL_FUNCS_EXT \
    _SG_XMACRO(glBindVertexArray,                 void, (GLuint array)) \
    _SG_XMACRO(glFramebufferTextureLayer,         void, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)) \
    _SG_XMACRO(glGenFramebuffers,                 void, (GLsizei n, GLuint * framebuffers)) \
    _SG_XMACRO(glBindFramebuffer,                 void, (GLenum target, GLuint framebuffer)) \
    _SG_XMACRO(glBindRenderbuffer,                void, (GLenum target, GLuint renderbuffer)) \
    _SG_XMACRO(glGetStringi,                      const GLubyte *, (GLenum name, GLuint index)) \
    _SG_XMACRO(glClearBufferfi,                   void, (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)) \
    _SG_XMACRO(glClearBufferfv,                   void, (GLenum buffer, GLint drawbuffer, const GLfloat * value)) \
    _SG_XMACRO(glClearBufferuiv,                  void, (GLenum buffer, GLint drawbuffer, const GLuint * value)) \
    _SG_XMACRO(glClearBufferiv,                   void, (GLenum buffer, GLint drawbuffer, const GLint * value)) \
    _SG_XMACRO(glDeleteRenderbuffers,             void, (GLsizei n, const GLuint * renderbuffers)) \
    _SG_XMACRO(glUniform1fv,                      void, (GLint location, GLsizei count, const GLfloat * value)) \
    _SG_XMACRO(glUniform2fv,                      void, (GLint location, GLsizei count, const GLfloat * value)) \
    _SG_XMACRO(glUniform3fv,                      void, (GLint location, GLsizei count, const GLfloat * value)) \
    _SG_XMACRO(glUniform4fv,                      void, (GLint location, GLsizei count, const GLfloat * value)) \
    _SG_XMACRO(glUniform1iv,                      void, (GLint location, GLsizei count, const GLint * value)) \
    _SG_XMACRO(glUniform2iv,                      void, (GLint location, GLsizei count, const GLint * value)) \
    _SG_XMACRO(glUniform3iv,                      void, (GLint location, GLsizei count, const GLint * value)) \
    _SG_XMACRO(glUniform4iv,                      void, (GLint location, GLsizei count, const GLint * value)) \
    _SG_XMACRO(glUniformMatrix4fv,                void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)) \
    _SG_XMACRO(glUseProgram,                      void, (GLuint program)) \
    _SG_XMACRO(glShaderSource,                    void, (GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length)) \
    _SG_XMACRO(glLinkProgram,                     void, (GLuint program)) \
    _SG_XMACRO(glGetUniformLocation,              GLint, (GLuint program, const GLchar * name)) \
    _SG_XMACRO(glGetShaderiv,                     void, (GLuint shader, GLenum pname, GLint * params)) \
    _SG_XMACRO(glGetProgramInfoLog,               void, (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)) \
    _SG_XMACRO(glGetAttribLocation,               GLint, (GLuint program, const GLchar * name)) \
    _SG_XMACRO(glDisableVertexAttribArray,        void, (GLuint index)) \
    _SG_XMACRO(glDeleteShader,                    void, (GLuint shader)) \
    _SG_XMACRO(glDeleteProgram,                   void, (GLuint program)) \
    _SG_XMACRO(glCompileShader,                   void, (GLuint shader)) \
    _SG_XMACRO(glStencilFuncSeparate,             void, (GLenum face, GLenum func, GLint ref, GLuint mask)) \
    _SG_XMACRO(glStencilOpSeparate,               void, (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)) \
    _SG_XMACRO(glRenderbufferStorageMultisample,  void, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)) \
    _SG_XMACRO(glDrawBuffers,                     void, (GLsizei n, const GLenum * bufs)) \
    _SG_XMACRO(glVertexAttribDivisor,             void, (GLuint index, GLuint divisor)) \
    _SG_XMACRO(glBufferSubData,                   void, (GLenum target, GLintptr offset, GLsizeiptr size, const void * data)) \
    _SG_XMACRO(glGenBuffers,                      void, (GLsizei n, GLuint * buffers)) \
    _SG_XMACRO(glCheckFramebufferStatus,          GLenum, (GLenum target)) \
    _SG_XMACRO(glFramebufferRenderbuffer,         void, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)) \
    _SG_XMACRO(glCompressedTexImage2D,            void, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data)) \
    _SG_XMACRO(glCompressedTexImage3D,            void, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data)) \
    _SG_XMACRO(glActiveTexture,                   void, (GLenum texture)) \
    _SG_XMACRO(glTexSubImage3D,                   void, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels)) \
    _SG_XMACRO(glRenderbufferStorage,             void, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)) \
    _SG_XMACRO(glGenTextures,                     void, (GLsizei n, GLuint * textures)) \
    _SG_XMACRO(glPolygonOffset,                   void, (GLfloat factor, GLfloat units)) \
    _SG_XMACRO(glDrawElements,                    void, (GLenum mode, GLsizei count, GLenum type, const void * indices)) \
    _SG_XMACRO(glDeleteFramebuffers,              void, (GLsizei n, const GLuint * framebuffers)) \
    _SG_XMACRO(glBlendEquationSeparate,           void, (GLenum modeRGB, GLenum modeAlpha)) \
    _SG_XMACRO(glDeleteTextures,                  void, (GLsizei n, const GLuint * textures)) \
    _SG_XMACRO(glGetProgramiv,                    void, (GLuint program, GLenum pname, GLint * params)) \
    _SG_XMACRO(glBindTexture,                     void, (GLenum target, GLuint texture)) \
    _SG_XMACRO(glTexImage3D,                      void, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels)) \
    _SG_XMACRO(glCreateShader,                    GLuint, (GLenum type)) \
    _SG_XMACRO(glTexSubImage2D,                   void, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels)) \
    _SG_XMACRO(glFramebufferTexture2D,            void, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)) \
    _SG_XMACRO(glCreateProgram,                   GLuint, (void)) \
    _SG_XMACRO(glViewport,                        void, (GLint x, GLint y, GLsizei width, GLsizei height)) \
    _SG_XMACRO(glDeleteBuffers,                   void, (GLsizei n, const GLuint * buffers)) \
    _SG_XMACRO(glDrawArrays,                      void, (GLenum mode, GLint first, GLsizei count)) \
    _SG_XMACRO(glDrawElementsInstanced,           void, (GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount)) \
    _SG_XMACRO(glVertexAttribPointer,             void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer)) \
    _SG_XMACRO(glUniform1i,                       void, (GLint location, GLint v0)) \
    _SG_XMACRO(glDisable,                         void, (GLenum cap)) \
    _SG_XMACRO(glColorMask,                       void, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)) \
    _SG_XMACRO(glColorMaski,                      void, (GLuint buf, GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)) \
    _SG_XMACRO(glBindBuffer,                      void, (GLenum target, GLuint buffer)) \
    _SG_XMACRO(glDeleteVertexArrays,              void, (GLsizei n, const GLuint * arrays)) \
    _SG_XMACRO(glDepthMask,                       void, (GLboolean flag)) \
    _SG_XMACRO(glDrawArraysInstanced,             void, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount)) \
    _SG_XMACRO(glScissor,                         void, (GLint x, GLint y, GLsizei width, GLsizei height)) \
    _SG_XMACRO(glGenRenderbuffers,                void, (GLsizei n, GLuint * renderbuffers)) \
    _SG_XMACRO(glBufferData,                      void, (GLenum target, GLsizeiptr size, const void * data, GLenum usage)) \
    _SG_XMACRO(glBlendFuncSeparate,               void, (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)) \
    _SG_XMACRO(glTexParameteri,                   void, (GLenum target, GLenum pname, GLint param)) \
    _SG_XMACRO(glGetIntegerv,                     void, (GLenum pname, GLint * data)) \
    _SG_XMACRO(glEnable,                          void, (GLenum cap)) \
    _SG_XMACRO(glBlitFramebuffer,                 void, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)) \
    _SG_XMACRO(glStencilMask,                     void, (GLuint mask)) \
    _SG_XMACRO(glAttachShader,                    void, (GLuint program, GLuint shader)) \
    _SG_XMACRO(glGetError,                        GLenum, (void)) \
    _SG_XMACRO(glBlendColor,                      void, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)) \
    _SG_XMACRO(glTexParameterf,                   void, (GLenum target, GLenum pname, GLfloat param)) \
    _SG_XMACRO(glTexParameterfv,                  void, (GLenum target, GLenum pname, const GLfloat* params)) \
    _SG_XMACRO(glGetShaderInfoLog,                void, (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)) \
    _SG_XMACRO(glDepthFunc,                       void, (GLenum func)) \
    _SG_XMACRO(glStencilOp ,                      void, (GLenum fail, GLenum zfail, GLenum zpass)) \
    _SG_XMACRO(glStencilFunc,                     void, (GLenum func, GLint ref, GLuint mask)) \
    _SG_XMACRO(glEnableVertexAttribArray,         void, (GLuint index)) \
    _SG_XMACRO(glBlendFunc,                       void, (GLenum sfactor, GLenum dfactor)) \
    _SG_XMACRO(glReadBuffer,                      void, (GLenum src)) \
    _SG_XMACRO(glTexImage2D,                      void, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels)) \
    _SG_XMACRO(glGenVertexArrays,                 void, (GLsizei n, GLuint * arrays)) \
    _SG_XMACRO(glFrontFace,                       void, (GLenum mode)) \
    _SG_XMACRO(glCullFace,                        void, (GLenum mode)) \
    _SG_XMACRO(glPixelStorei,                     void, (GLenum pname, GLint param)) \
    _SG_XMACRO(glBindSampler,                     void, (GLuint unit, GLuint sampler)) \
    _SG_XMACRO(glGenSamplers,                     void, (GLsizei n, GLuint* samplers)) \
    _SG_XMACRO(glSamplerParameteri,               void, (GLuint sampler, GLenum pname, GLint param)) \
    _SG_XMACRO(glSamplerParameterf,               void, (GLuint sampler, GLenum pname, GLfloat param)) \
    _SG_XMACRO(glSamplerParameterfv,              void, (GLuint sampler, GLenum pname, const GLfloat* params)) \
    _SG_XMACRO(glDeleteSamplers,                  void, (GLsizei n, const GLuint* samplers)) \
    _SG_XMACRO(glBindBufferBase,                  void, (GLenum target, GLuint index, GLuint buffer))

// generate GL function pointer typedefs
#define _SG_XMACRO(name, ret, args) typedef ret (GL_APIENTRY* PFN_ ## name) args;
_SG_GL_FUNCS
#undef _SG_XMACRO

// generate GL function pointers
#define _SG_XMACRO(name, ret, args) static PFN_ ## name name;
_SG_GL_FUNCS
#undef _SG_XMACRO

// helper function to lookup GL functions in GL DLL
typedef PROC (WINAPI * _sg_wglGetProcAddress)(LPCSTR);
_SOKOL_PRIVATE void* _sg_gl_getprocaddr(const char* name, _sg_wglGetProcAddress wgl_getprocaddress) {
    void* proc_addr = (void*) wgl_getprocaddress(name);
    if (0 == proc_addr) {
        proc_addr = (void*) GetProcAddress(_sg.gl.opengl32_dll, name);
    }
    SOKOL_ASSERT(proc_addr);
    return proc_addr;
}

// populate GL function pointers
_SOKOL_PRIVATE  void _sg_gl_load_opengl(void) {
    SOKOL_ASSERT(0 == _sg.gl.opengl32_dll);
    _sg.gl.opengl32_dll = LoadLibraryA("opengl32.dll");
    SOKOL_ASSERT(_sg.gl.opengl32_dll);
    _sg_wglGetProcAddress wgl_getprocaddress = (_sg_wglGetProcAddress) GetProcAddress(_sg.gl.opengl32_dll, "wglGetProcAddress");
    SOKOL_ASSERT(wgl_getprocaddress);
    #define _SG_XMACRO(name, ret, args) name = (PFN_ ## name) _sg_gl_getprocaddr(#name, wgl_getprocaddress);
    _SG_GL_FUNCS
    #undef _SG_XMACRO
}

_SOKOL_PRIVATE void _sg_gl_unload_opengl(void) {
    SOKOL_ASSERT(_sg.gl.opengl32_dll);
    FreeLibrary(_sg.gl.opengl32_dll);
    _sg.gl.opengl32_dll = 0;
}
#endif // _SOKOL_USE_WIN32_GL_LOADER

//-- type translation ----------------------------------------------------------
_SOKOL_PRIVATE GLenum _sg_gl_buffer_target(sg_buffer_type t) {
    switch (t) {
        case SG_BUFFERTYPE_VERTEXBUFFER:    return GL_ARRAY_BUFFER;
        case SG_BUFFERTYPE_INDEXBUFFER:     return GL_ELEMENT_ARRAY_BUFFER;
        case SG_BUFFERTYPE_STORAGEBUFFER:   return GL_SHADER_STORAGE_BUFFER;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_texture_target(sg_image_type t) {
    switch (t) {
        case SG_IMAGETYPE_2D:   return GL_TEXTURE_2D;
        case SG_IMAGETYPE_CUBE: return GL_TEXTURE_CUBE_MAP;
        case SG_IMAGETYPE_3D:       return GL_TEXTURE_3D;
        case SG_IMAGETYPE_ARRAY:    return GL_TEXTURE_2D_ARRAY;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_usage(sg_usage u) {
    switch (u) {
        case SG_USAGE_IMMUTABLE:    return GL_STATIC_DRAW;
        case SG_USAGE_DYNAMIC:      return GL_DYNAMIC_DRAW;
        case SG_USAGE_STREAM:       return GL_STREAM_DRAW;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_shader_stage(sg_shader_stage stage) {
    switch (stage) {
        case SG_SHADERSTAGE_VS:     return GL_VERTEX_SHADER;
        case SG_SHADERSTAGE_FS:     return GL_FRAGMENT_SHADER;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLint _sg_gl_vertexformat_size(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_FLOAT:     return 1;
        case SG_VERTEXFORMAT_FLOAT2:    return 2;
        case SG_VERTEXFORMAT_FLOAT3:    return 3;
        case SG_VERTEXFORMAT_FLOAT4:    return 4;
        case SG_VERTEXFORMAT_BYTE4:     return 4;
        case SG_VERTEXFORMAT_BYTE4N:    return 4;
        case SG_VERTEXFORMAT_UBYTE4:    return 4;
        case SG_VERTEXFORMAT_UBYTE4N:   return 4;
        case SG_VERTEXFORMAT_SHORT2:    return 2;
        case SG_VERTEXFORMAT_SHORT2N:   return 2;
        case SG_VERTEXFORMAT_USHORT2N:  return 2;
        case SG_VERTEXFORMAT_SHORT4:    return 4;
        case SG_VERTEXFORMAT_SHORT4N:   return 4;
        case SG_VERTEXFORMAT_USHORT4N:  return 4;
        case SG_VERTEXFORMAT_UINT10_N2: return 4;
        case SG_VERTEXFORMAT_HALF2:     return 2;
        case SG_VERTEXFORMAT_HALF4:     return 4;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_vertexformat_type(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_FLOAT:
        case SG_VERTEXFORMAT_FLOAT2:
        case SG_VERTEXFORMAT_FLOAT3:
        case SG_VERTEXFORMAT_FLOAT4:
            return GL_FLOAT;
        case SG_VERTEXFORMAT_BYTE4:
        case SG_VERTEXFORMAT_BYTE4N:
            return GL_BYTE;
        case SG_VERTEXFORMAT_UBYTE4:
        case SG_VERTEXFORMAT_UBYTE4N:
            return GL_UNSIGNED_BYTE;
        case SG_VERTEXFORMAT_SHORT2:
        case SG_VERTEXFORMAT_SHORT2N:
        case SG_VERTEXFORMAT_SHORT4:
        case SG_VERTEXFORMAT_SHORT4N:
            return GL_SHORT;
        case SG_VERTEXFORMAT_USHORT2N:
        case SG_VERTEXFORMAT_USHORT4N:
            return GL_UNSIGNED_SHORT;
        case SG_VERTEXFORMAT_UINT10_N2:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case SG_VERTEXFORMAT_HALF2:
        case SG_VERTEXFORMAT_HALF4:
            return GL_HALF_FLOAT;
        default:
            SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLboolean _sg_gl_vertexformat_normalized(sg_vertex_format fmt) {
    switch (fmt) {
        case SG_VERTEXFORMAT_BYTE4N:
        case SG_VERTEXFORMAT_UBYTE4N:
        case SG_VERTEXFORMAT_SHORT2N:
        case SG_VERTEXFORMAT_USHORT2N:
        case SG_VERTEXFORMAT_SHORT4N:
        case SG_VERTEXFORMAT_USHORT4N:
        case SG_VERTEXFORMAT_UINT10_N2:
            return GL_TRUE;
        default:
            return GL_FALSE;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_primitive_type(sg_primitive_type t) {
    switch (t) {
        case SG_PRIMITIVETYPE_POINTS:           return GL_POINTS;
        case SG_PRIMITIVETYPE_LINES:            return GL_LINES;
        case SG_PRIMITIVETYPE_LINE_STRIP:       return GL_LINE_STRIP;
        case SG_PRIMITIVETYPE_TRIANGLES:        return GL_TRIANGLES;
        case SG_PRIMITIVETYPE_TRIANGLE_STRIP:   return GL_TRIANGLE_STRIP;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_index_type(sg_index_type t) {
    switch (t) {
        case SG_INDEXTYPE_NONE:     return 0;
        case SG_INDEXTYPE_UINT16:   return GL_UNSIGNED_SHORT;
        case SG_INDEXTYPE_UINT32:   return GL_UNSIGNED_INT;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_compare_func(sg_compare_func cmp) {
    switch (cmp) {
        case SG_COMPAREFUNC_NEVER:          return GL_NEVER;
        case SG_COMPAREFUNC_LESS:           return GL_LESS;
        case SG_COMPAREFUNC_EQUAL:          return GL_EQUAL;
        case SG_COMPAREFUNC_LESS_EQUAL:     return GL_LEQUAL;
        case SG_COMPAREFUNC_GREATER:        return GL_GREATER;
        case SG_COMPAREFUNC_NOT_EQUAL:      return GL_NOTEQUAL;
        case SG_COMPAREFUNC_GREATER_EQUAL:  return GL_GEQUAL;
        case SG_COMPAREFUNC_ALWAYS:         return GL_ALWAYS;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_stencil_op(sg_stencil_op op) {
    switch (op) {
        case SG_STENCILOP_KEEP:         return GL_KEEP;
        case SG_STENCILOP_ZERO:         return GL_ZERO;
        case SG_STENCILOP_REPLACE:      return GL_REPLACE;
        case SG_STENCILOP_INCR_CLAMP:   return GL_INCR;
        case SG_STENCILOP_DECR_CLAMP:   return GL_DECR;
        case SG_STENCILOP_INVERT:       return GL_INVERT;
        case SG_STENCILOP_INCR_WRAP:    return GL_INCR_WRAP;
        case SG_STENCILOP_DECR_WRAP:    return GL_DECR_WRAP;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_blend_factor(sg_blend_factor f) {
    switch (f) {
        case SG_BLENDFACTOR_ZERO:                   return GL_ZERO;
        case SG_BLENDFACTOR_ONE:                    return GL_ONE;
        case SG_BLENDFACTOR_SRC_COLOR:              return GL_SRC_COLOR;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_COLOR:    return GL_ONE_MINUS_SRC_COLOR;
        case SG_BLENDFACTOR_SRC_ALPHA:              return GL_SRC_ALPHA;
        case SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA:    return GL_ONE_MINUS_SRC_ALPHA;
        case SG_BLENDFACTOR_DST_COLOR:              return GL_DST_COLOR;
        case SG_BLENDFACTOR_ONE_MINUS_DST_COLOR:    return GL_ONE_MINUS_DST_COLOR;
        case SG_BLENDFACTOR_DST_ALPHA:              return GL_DST_ALPHA;
        case SG_BLENDFACTOR_ONE_MINUS_DST_ALPHA:    return GL_ONE_MINUS_DST_ALPHA;
        case SG_BLENDFACTOR_SRC_ALPHA_SATURATED:    return GL_SRC_ALPHA_SATURATE;
        case SG_BLENDFACTOR_BLEND_COLOR:            return GL_CONSTANT_COLOR;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_COLOR:  return GL_ONE_MINUS_CONSTANT_COLOR;
        case SG_BLENDFACTOR_BLEND_ALPHA:            return GL_CONSTANT_ALPHA;
        case SG_BLENDFACTOR_ONE_MINUS_BLEND_ALPHA:  return GL_ONE_MINUS_CONSTANT_ALPHA;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_blend_op(sg_blend_op op) {
    switch (op) {
        case SG_BLENDOP_ADD:                return GL_FUNC_ADD;
        case SG_BLENDOP_SUBTRACT:           return GL_FUNC_SUBTRACT;
        case SG_BLENDOP_REVERSE_SUBTRACT:   return GL_FUNC_REVERSE_SUBTRACT;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_min_filter(sg_filter min_f, sg_filter mipmap_f) {
    if (min_f == SG_FILTER_NEAREST) {
        switch (mipmap_f) {
            case SG_FILTER_NONE:    return GL_NEAREST;
            case SG_FILTER_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
            case SG_FILTER_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
            default: SOKOL_UNREACHABLE; return (GLenum)0;
        }
    } else if (min_f == SG_FILTER_LINEAR) {
        switch (mipmap_f) {
            case SG_FILTER_NONE:    return GL_LINEAR;
            case SG_FILTER_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
            case SG_FILTER_LINEAR:  return GL_LINEAR_MIPMAP_LINEAR;
            default: SOKOL_UNREACHABLE; return (GLenum)0;
        }
    } else {
        SOKOL_UNREACHABLE; return (GLenum)0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_mag_filter(sg_filter mag_f) {
    if (mag_f == SG_FILTER_NEAREST) {
        return GL_NEAREST;
    } else {
        return GL_LINEAR;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_wrap(sg_wrap w) {
    switch (w) {
        case SG_WRAP_CLAMP_TO_EDGE:     return GL_CLAMP_TO_EDGE;
        case SG_WRAP_CLAMP_TO_BORDER:   return GL_CLAMP_TO_EDGE;
        case SG_WRAP_REPEAT:            return GL_REPEAT;
        case SG_WRAP_MIRRORED_REPEAT:   return GL_MIRRORED_REPEAT;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_teximage_type(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:
        case SG_PIXELFORMAT_R8UI:
        case SG_PIXELFORMAT_RG8:
        case SG_PIXELFORMAT_RG8UI:
        case SG_PIXELFORMAT_RGBA8:
        case SG_PIXELFORMAT_SRGB8A8:
        case SG_PIXELFORMAT_RGBA8UI:
        case SG_PIXELFORMAT_BGRA8:
            return GL_UNSIGNED_BYTE;
        case SG_PIXELFORMAT_R8SN:
        case SG_PIXELFORMAT_R8SI:
        case SG_PIXELFORMAT_RG8SN:
        case SG_PIXELFORMAT_RG8SI:
        case SG_PIXELFORMAT_RGBA8SN:
        case SG_PIXELFORMAT_RGBA8SI:
            return GL_BYTE;
        case SG_PIXELFORMAT_R16:
        case SG_PIXELFORMAT_R16UI:
        case SG_PIXELFORMAT_RG16:
        case SG_PIXELFORMAT_RG16UI:
        case SG_PIXELFORMAT_RGBA16:
        case SG_PIXELFORMAT_RGBA16UI:
            return GL_UNSIGNED_SHORT;
        case SG_PIXELFORMAT_R16SN:
        case SG_PIXELFORMAT_R16SI:
        case SG_PIXELFORMAT_RG16SN:
        case SG_PIXELFORMAT_RG16SI:
        case SG_PIXELFORMAT_RGBA16SN:
        case SG_PIXELFORMAT_RGBA16SI:
            return GL_SHORT;
        case SG_PIXELFORMAT_R16F:
        case SG_PIXELFORMAT_RG16F:
        case SG_PIXELFORMAT_RGBA16F:
            return GL_HALF_FLOAT;
        case SG_PIXELFORMAT_R32UI:
        case SG_PIXELFORMAT_RG32UI:
        case SG_PIXELFORMAT_RGBA32UI:
            return GL_UNSIGNED_INT;
        case SG_PIXELFORMAT_R32SI:
        case SG_PIXELFORMAT_RG32SI:
        case SG_PIXELFORMAT_RGBA32SI:
            return GL_INT;
        case SG_PIXELFORMAT_R32F:
        case SG_PIXELFORMAT_RG32F:
        case SG_PIXELFORMAT_RGBA32F:
            return GL_FLOAT;
        case SG_PIXELFORMAT_RGB10A2:
            return GL_UNSIGNED_INT_2_10_10_10_REV;
        case SG_PIXELFORMAT_RG11B10F:
            return GL_UNSIGNED_INT_10F_11F_11F_REV;
        case SG_PIXELFORMAT_RGB9E5:
            return GL_UNSIGNED_INT_5_9_9_9_REV;
        case SG_PIXELFORMAT_DEPTH:
            return GL_FLOAT;
        case SG_PIXELFORMAT_DEPTH_STENCIL:
            return GL_UNSIGNED_INT_24_8;
        default:
            SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_teximage_format(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:
        case SG_PIXELFORMAT_R8SN:
        case SG_PIXELFORMAT_R16:
        case SG_PIXELFORMAT_R16SN:
        case SG_PIXELFORMAT_R16F:
        case SG_PIXELFORMAT_R32F:
            return GL_RED;
        case SG_PIXELFORMAT_R8UI:
        case SG_PIXELFORMAT_R8SI:
        case SG_PIXELFORMAT_R16UI:
        case SG_PIXELFORMAT_R16SI:
        case SG_PIXELFORMAT_R32UI:
        case SG_PIXELFORMAT_R32SI:
            return GL_RED_INTEGER;
        case SG_PIXELFORMAT_RG8:
        case SG_PIXELFORMAT_RG8SN:
        case SG_PIXELFORMAT_RG16:
        case SG_PIXELFORMAT_RG16SN:
        case SG_PIXELFORMAT_RG16F:
        case SG_PIXELFORMAT_RG32F:
            return GL_RG;
        case SG_PIXELFORMAT_RG8UI:
        case SG_PIXELFORMAT_RG8SI:
        case SG_PIXELFORMAT_RG16UI:
        case SG_PIXELFORMAT_RG16SI:
        case SG_PIXELFORMAT_RG32UI:
        case SG_PIXELFORMAT_RG32SI:
            return GL_RG_INTEGER;
        case SG_PIXELFORMAT_RGBA8:
        case SG_PIXELFORMAT_SRGB8A8:
        case SG_PIXELFORMAT_RGBA8SN:
        case SG_PIXELFORMAT_RGBA16:
        case SG_PIXELFORMAT_RGBA16SN:
        case SG_PIXELFORMAT_RGBA16F:
        case SG_PIXELFORMAT_RGBA32F:
        case SG_PIXELFORMAT_RGB10A2:
            return GL_RGBA;
        case SG_PIXELFORMAT_RGBA8UI:
        case SG_PIXELFORMAT_RGBA8SI:
        case SG_PIXELFORMAT_RGBA16UI:
        case SG_PIXELFORMAT_RGBA16SI:
        case SG_PIXELFORMAT_RGBA32UI:
        case SG_PIXELFORMAT_RGBA32SI:
            return GL_RGBA_INTEGER;
        case SG_PIXELFORMAT_RG11B10F:
        case SG_PIXELFORMAT_RGB9E5:
            return GL_RGB;
        case SG_PIXELFORMAT_DEPTH:
            return GL_DEPTH_COMPONENT;
        case SG_PIXELFORMAT_DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case SG_PIXELFORMAT_BC1_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case SG_PIXELFORMAT_BC2_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case SG_PIXELFORMAT_BC3_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case SG_PIXELFORMAT_BC3_SRGBA:
            return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        case SG_PIXELFORMAT_BC4_R:
            return GL_COMPRESSED_RED_RGTC1;
        case SG_PIXELFORMAT_BC4_RSN:
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case SG_PIXELFORMAT_BC5_RG:
            return GL_COMPRESSED_RED_GREEN_RGTC2;
        case SG_PIXELFORMAT_BC5_RGSN:
            return GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2;
        case SG_PIXELFORMAT_BC6H_RGBF:
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
        case SG_PIXELFORMAT_BC6H_RGBUF:
            return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
        case SG_PIXELFORMAT_BC7_RGBA:
            return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case SG_PIXELFORMAT_BC7_SRGBA:
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:
            return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:
            return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:
            return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:
            return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        case SG_PIXELFORMAT_ETC2_RGB8:
            return GL_COMPRESSED_RGB8_ETC2;
        case SG_PIXELFORMAT_ETC2_SRGB8:
            return GL_COMPRESSED_SRGB8_ETC2;
        case SG_PIXELFORMAT_ETC2_RGB8A1:
            return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        case SG_PIXELFORMAT_ETC2_RGBA8:
            return GL_COMPRESSED_RGBA8_ETC2_EAC;
        case SG_PIXELFORMAT_ETC2_SRGB8A8:
            return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        case SG_PIXELFORMAT_EAC_R11:
            return GL_COMPRESSED_R11_EAC;
        case SG_PIXELFORMAT_EAC_R11SN:
            return GL_COMPRESSED_SIGNED_R11_EAC;
        case SG_PIXELFORMAT_EAC_RG11:
            return GL_COMPRESSED_RG11_EAC;
        case SG_PIXELFORMAT_EAC_RG11SN:
            return GL_COMPRESSED_SIGNED_RG11_EAC;
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:
            return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        default:
            SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_teximage_internal_format(sg_pixel_format fmt) {
    switch (fmt) {
        case SG_PIXELFORMAT_R8:         return GL_R8;
        case SG_PIXELFORMAT_R8SN:       return GL_R8_SNORM;
        case SG_PIXELFORMAT_R8UI:       return GL_R8UI;
        case SG_PIXELFORMAT_R8SI:       return GL_R8I;
        #if !defined(SOKOL_GLES3)
            case SG_PIXELFORMAT_R16:        return GL_R16;
            case SG_PIXELFORMAT_R16SN:      return GL_R16_SNORM;
        #endif
        case SG_PIXELFORMAT_R16UI:      return GL_R16UI;
        case SG_PIXELFORMAT_R16SI:      return GL_R16I;
        case SG_PIXELFORMAT_R16F:       return GL_R16F;
        case SG_PIXELFORMAT_RG8:        return GL_RG8;
        case SG_PIXELFORMAT_RG8SN:      return GL_RG8_SNORM;
        case SG_PIXELFORMAT_RG8UI:      return GL_RG8UI;
        case SG_PIXELFORMAT_RG8SI:      return GL_RG8I;
        case SG_PIXELFORMAT_R32UI:      return GL_R32UI;
        case SG_PIXELFORMAT_R32SI:      return GL_R32I;
        case SG_PIXELFORMAT_R32F:       return GL_R32F;
        #if !defined(SOKOL_GLES3)
            case SG_PIXELFORMAT_RG16:       return GL_RG16;
            case SG_PIXELFORMAT_RG16SN:     return GL_RG16_SNORM;
        #endif
        case SG_PIXELFORMAT_RG16UI:     return GL_RG16UI;
        case SG_PIXELFORMAT_RG16SI:     return GL_RG16I;
        case SG_PIXELFORMAT_RG16F:      return GL_RG16F;
        case SG_PIXELFORMAT_RGBA8:      return GL_RGBA8;
        case SG_PIXELFORMAT_SRGB8A8:    return GL_SRGB8_ALPHA8;
        case SG_PIXELFORMAT_RGBA8SN:    return GL_RGBA8_SNORM;
        case SG_PIXELFORMAT_RGBA8UI:    return GL_RGBA8UI;
        case SG_PIXELFORMAT_RGBA8SI:    return GL_RGBA8I;
        case SG_PIXELFORMAT_RGB10A2:    return GL_RGB10_A2;
        case SG_PIXELFORMAT_RG11B10F:   return GL_R11F_G11F_B10F;
        case SG_PIXELFORMAT_RGB9E5:     return GL_RGB9_E5;
        case SG_PIXELFORMAT_RG32UI:     return GL_RG32UI;
        case SG_PIXELFORMAT_RG32SI:     return GL_RG32I;
        case SG_PIXELFORMAT_RG32F:      return GL_RG32F;
        #if !defined(SOKOL_GLES3)
            case SG_PIXELFORMAT_RGBA16:     return GL_RGBA16;
            case SG_PIXELFORMAT_RGBA16SN:   return GL_RGBA16_SNORM;
        #endif
        case SG_PIXELFORMAT_RGBA16UI:   return GL_RGBA16UI;
        case SG_PIXELFORMAT_RGBA16SI:   return GL_RGBA16I;
        case SG_PIXELFORMAT_RGBA16F:    return GL_RGBA16F;
        case SG_PIXELFORMAT_RGBA32UI:   return GL_RGBA32UI;
        case SG_PIXELFORMAT_RGBA32SI:   return GL_RGBA32I;
        case SG_PIXELFORMAT_RGBA32F:    return GL_RGBA32F;
        case SG_PIXELFORMAT_DEPTH:      return GL_DEPTH_COMPONENT32F;
        case SG_PIXELFORMAT_DEPTH_STENCIL:      return GL_DEPTH24_STENCIL8;
        case SG_PIXELFORMAT_BC1_RGBA:           return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case SG_PIXELFORMAT_BC2_RGBA:           return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case SG_PIXELFORMAT_BC3_RGBA:           return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case SG_PIXELFORMAT_BC3_SRGBA:          return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        case SG_PIXELFORMAT_BC4_R:              return GL_COMPRESSED_RED_RGTC1;
        case SG_PIXELFORMAT_BC4_RSN:            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case SG_PIXELFORMAT_BC5_RG:             return GL_COMPRESSED_RED_GREEN_RGTC2;
        case SG_PIXELFORMAT_BC5_RGSN:           return GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2;
        case SG_PIXELFORMAT_BC6H_RGBF:          return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
        case SG_PIXELFORMAT_BC6H_RGBUF:         return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
        case SG_PIXELFORMAT_BC7_RGBA:           return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case SG_PIXELFORMAT_BC7_SRGBA:          return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case SG_PIXELFORMAT_PVRTC_RGB_2BPP:     return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGB_4BPP:     return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGBA_2BPP:    return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        case SG_PIXELFORMAT_PVRTC_RGBA_4BPP:    return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        case SG_PIXELFORMAT_ETC2_RGB8:          return GL_COMPRESSED_RGB8_ETC2;
        case SG_PIXELFORMAT_ETC2_SRGB8:         return GL_COMPRESSED_SRGB8_ETC2;
        case SG_PIXELFORMAT_ETC2_RGB8A1:        return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        case SG_PIXELFORMAT_ETC2_RGBA8:         return GL_COMPRESSED_RGBA8_ETC2_EAC;
        case SG_PIXELFORMAT_ETC2_SRGB8A8:       return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        case SG_PIXELFORMAT_EAC_R11:            return GL_COMPRESSED_R11_EAC;
        case SG_PIXELFORMAT_EAC_R11SN:          return GL_COMPRESSED_SIGNED_R11_EAC;
        case SG_PIXELFORMAT_EAC_RG11:           return GL_COMPRESSED_RG11_EAC;
        case SG_PIXELFORMAT_EAC_RG11SN:         return GL_COMPRESSED_SIGNED_RG11_EAC;
        case SG_PIXELFORMAT_ASTC_4x4_RGBA:      return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        case SG_PIXELFORMAT_ASTC_4x4_SRGBA:     return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_cubeface_target(int face_index) {
    switch (face_index) {
        case 0: return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case 1: return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case 2: return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case 3: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case 4: return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case 5: return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        default: SOKOL_UNREACHABLE; return 0;
    }
}

// see: https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml
_SOKOL_PRIVATE void _sg_gl_init_pixelformats(bool has_bgra) {
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_R8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R8SI]);
    #if !defined(SOKOL_GLES3)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_R16SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RG8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG8SI]);
    _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_R32UI]);
    _sg_pixelformat_sr(&_sg.formats[SG_PIXELFORMAT_R32SI]);
    #if !defined(SOKOL_GLES3)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG16SI]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA8]);
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_SRGB8A8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RGBA8SN]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA8UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA8SI]);
    if (has_bgra) {
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_BGRA8]);
    }
    _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGB10A2]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RG11B10F]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RGB9E5]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG32UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RG32SI]);
    #if !defined(SOKOL_GLES3)
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16SN]);
    #endif
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA16UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA16SI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA32UI]);
    _sg_pixelformat_srm(&_sg.formats[SG_PIXELFORMAT_RGBA32SI]);
    _sg_pixelformat_srmd(&_sg.formats[SG_PIXELFORMAT_DEPTH]);
    _sg_pixelformat_srmd(&_sg.formats[SG_PIXELFORMAT_DEPTH_STENCIL]);
}

// FIXME: OES_half_float_blend
_SOKOL_PRIVATE void _sg_gl_init_pixelformats_half_float(bool has_colorbuffer_half_float) {
    if (has_colorbuffer_half_float) {
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R16F]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG16F]);
        _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA16F]);
    } else {
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_R16F]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RG16F]);
        _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RGBA16F]);
    }
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_float(bool has_colorbuffer_float, bool has_texture_float_linear, bool has_float_blend) {
    if (has_texture_float_linear) {
        if (has_colorbuffer_float) {
            if (has_float_blend) {
                _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_R32F]);
                _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RG32F]);
                _sg_pixelformat_all(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
            } else {
                _sg_pixelformat_sfrm(&_sg.formats[SG_PIXELFORMAT_R32F]);
                _sg_pixelformat_sfrm(&_sg.formats[SG_PIXELFORMAT_RG32F]);
                _sg_pixelformat_sfrm(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
            }
        } else {
            _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_R32F]);
            _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RG32F]);
            _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
        }
    } else {
        if (has_colorbuffer_float) {
            _sg_pixelformat_sbrm(&_sg.formats[SG_PIXELFORMAT_R32F]);
            _sg_pixelformat_sbrm(&_sg.formats[SG_PIXELFORMAT_RG32F]);
            _sg_pixelformat_sbrm(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
        } else {
            _sg_pixelformat_s(&_sg.formats[SG_PIXELFORMAT_R32F]);
            _sg_pixelformat_s(&_sg.formats[SG_PIXELFORMAT_RG32F]);
            _sg_pixelformat_s(&_sg.formats[SG_PIXELFORMAT_RGBA32F]);
        }
    }
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_s3tc(void) {
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC1_RGBA]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC2_RGBA]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC3_RGBA]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC3_SRGBA]);
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_rgtc(void) {
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC4_R]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC4_RSN]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC5_RG]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC5_RGSN]);
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_bptc(void) {
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC6H_RGBF]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC6H_RGBUF]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC7_RGBA]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_BC7_SRGBA]);
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_pvrtc(void) {
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGB_2BPP]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGB_4BPP]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGBA_2BPP]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_PVRTC_RGBA_4BPP]);
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_etc2(void) {
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGB8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_SRGB8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGB8A1]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_RGBA8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ETC2_SRGB8A8]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_R11]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_R11SN]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_RG11]);
    _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_EAC_RG11SN]);
}

_SOKOL_PRIVATE void _sg_gl_init_pixelformats_astc(void) {
     _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ASTC_4x4_RGBA]);
     _sg_pixelformat_sf(&_sg.formats[SG_PIXELFORMAT_ASTC_4x4_SRGBA]);
 }

_SOKOL_PRIVATE void _sg_gl_init_limits(void) {
    _SG_GL_CHECK_ERROR();
    GLint gl_int;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.max_image_size_2d = gl_int;
    _sg.limits.max_image_size_array = gl_int;
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.max_image_size_cube = gl_int;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_int);
    _SG_GL_CHECK_ERROR();
    if (gl_int > SG_MAX_VERTEX_ATTRIBUTES) {
        gl_int = SG_MAX_VERTEX_ATTRIBUTES;
    }
    _sg.limits.max_vertex_attrs = gl_int;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.gl_max_vertex_uniform_components = gl_int;
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.max_image_size_3d = gl_int;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.max_image_array_layers = gl_int;
    if (_sg.gl.ext_anisotropic) {
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_int);
        _SG_GL_CHECK_ERROR();
        _sg.gl.max_anisotropy = gl_int;
    } else {
        _sg.gl.max_anisotropy = 1;
    }
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &gl_int);
    _SG_GL_CHECK_ERROR();
    _sg.limits.gl_max_combined_texture_image_units = gl_int;
}

#if defined(SOKOL_GLES3)
_SOKOL_PRIVATE void _sg_gl_init_caps_gles3(void) {
    _sg.backend = SG_BACKEND_GLES3;

    _sg.features.origin_top_left = false;
    _sg.features.image_clamp_to_border = false;
    _sg.features.mrt_independent_blend_state = false;
    _sg.features.mrt_independent_write_mask = false;
    _sg.features.storage_buffer = false;

    bool has_s3tc = false;  // BC1..BC3
    bool has_rgtc = false;  // BC4 and BC5
    bool has_bptc = false;  // BC6H and BC7
    bool has_pvrtc = false;
    #if defined(__EMSCRIPTEN__)
        bool has_etc2 = false;
    #else
        bool has_etc2 = true;
    #endif
    bool has_astc = false;
    bool has_colorbuffer_float = false;
    bool has_colorbuffer_half_float = false;
    bool has_texture_float_linear = false;
    bool has_float_blend = false;
    GLint num_ext = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
    for (int i = 0; i < num_ext; i++) {
        const char* ext = (const char*) glGetStringi(GL_EXTENSIONS, (GLuint)i);
        if (ext) {
            if (strstr(ext, "_texture_compression_s3tc")) {
                has_s3tc = true;
            } else if (strstr(ext, "_compressed_texture_s3tc")) {
                has_s3tc = true;
            } else if (strstr(ext, "_texture_compression_rgtc")) {
                has_rgtc = true;
            } else if (strstr(ext, "_texture_compression_bptc")) {
                has_bptc = true;
            } else if (strstr(ext, "_texture_compression_pvrtc")) {
                has_pvrtc = true;
            } else if (strstr(ext, "_compressed_texture_pvrtc")) {
                has_pvrtc = true;
            } else if (strstr(ext, "_compressed_texture_etc")) {
                has_etc2 = true;
            } else if (strstr(ext, "_compressed_texture_astc")) {
                has_astc = true;
            } else if (strstr(ext, "_color_buffer_float")) {
                has_colorbuffer_float = true;
            } else if (strstr(ext, "_color_buffer_half_float")) {
                has_colorbuffer_half_float = true;
            } else if (strstr(ext, "_texture_float_linear")) {
                has_texture_float_linear = true;
            } else if (strstr(ext, "_float_blend")) {
                has_float_blend = true;
            } else if (strstr(ext, "_texture_filter_anisotropic")) {
                _sg.gl.ext_anisotropic = true;
            }
        }
    }

    /* on WebGL2, color_buffer_float also includes 16-bit formats
       see: https://developer.mozilla.org/en-US/docs/Web/API/EXT_color_buffer_float
    */
    #if defined(__EMSCRIPTEN__)
    if (!has_colorbuffer_half_float && has_colorbuffer_float) {
        has_colorbuffer_half_float = has_colorbuffer_float;
    }
    #endif

    // limits
    _sg_gl_init_limits();

    // pixel formats
    const bool has_bgra = false;    // not a bug
    _sg_gl_init_pixelformats(has_bgra);
    _sg_gl_init_pixelformats_float(has_colorbuffer_float, has_texture_float_linear, has_float_blend);
    _sg_gl_init_pixelformats_half_float(has_colorbuffer_half_float);
    if (has_s3tc) {
        _sg_gl_init_pixelformats_s3tc();
    }
    if (has_rgtc) {
        _sg_gl_init_pixelformats_rgtc();
    }
    if (has_bptc) {
        _sg_gl_init_pixelformats_bptc();
    }
    if (has_pvrtc) {
        _sg_gl_init_pixelformats_pvrtc();
    }
    if (has_etc2) {
        _sg_gl_init_pixelformats_etc2();
    }
    if (has_astc) {
        _sg_gl_init_pixelformats_astc();
    }
}
#endif

//-- state cache implementation ------------------------------------------------
_SOKOL_PRIVATE GLuint _sg_gl_storagebuffer_bind_index(int stage, int slot) {
    SOKOL_ASSERT((stage >= 0) && (stage < SG_NUM_SHADER_STAGES));
    SOKOL_ASSERT((slot >= 0) && (slot < SG_MAX_SHADERSTAGE_STORAGEBUFFERS));
    return (GLuint) (stage * _SG_GL_STORAGEBUFFER_STAGE_INDEX_PITCH + slot);
}

_SOKOL_PRIVATE void _sg_gl_cache_clear_buffer_bindings(bool force) {
    if (force || (_sg.gl.cache.vertex_buffer != 0)) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        _sg.gl.cache.vertex_buffer = 0;
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    if (force || (_sg.gl.cache.index_buffer != 0)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        _sg.gl.cache.index_buffer = 0;
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    if (force || (_sg.gl.cache.storage_buffer != 0)) {
        if (_sg.features.storage_buffer) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        _sg.gl.cache.storage_buffer = 0;
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    for (int stage = 0; stage < SG_NUM_SHADER_STAGES; stage++) {
        for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++) {
            if (force || (_sg.gl.cache.stage_storage_buffers[stage][i] != 0)) {
                const GLuint bind_index = _sg_gl_storagebuffer_bind_index(stage, i);
                if (_sg.features.storage_buffer) {
                    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_index, 0);
                }
                _sg.gl.cache.stage_storage_buffers[stage][i] = 0;
                _sg_stats_add(gl.num_bind_buffer, 1);
            }
        }
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_bind_buffer(GLenum target, GLuint buffer) {
    SOKOL_ASSERT((GL_ARRAY_BUFFER == target) || (GL_ELEMENT_ARRAY_BUFFER == target) || (GL_SHADER_STORAGE_BUFFER == target));
    if (target == GL_ARRAY_BUFFER) {
        if (_sg.gl.cache.vertex_buffer != buffer) {
            _sg.gl.cache.vertex_buffer = buffer;
            glBindBuffer(target, buffer);
            _sg_stats_add(gl.num_bind_buffer, 1);
        }
    } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
        if (_sg.gl.cache.index_buffer != buffer) {
            _sg.gl.cache.index_buffer = buffer;
            glBindBuffer(target, buffer);
            _sg_stats_add(gl.num_bind_buffer, 1);
        }
    } else if (target == GL_SHADER_STORAGE_BUFFER) {
        if (_sg.gl.cache.storage_buffer != buffer) {
            _sg.gl.cache.storage_buffer = buffer;
            if (_sg.features.storage_buffer) {
                glBindBuffer(target, buffer);
            }
            _sg_stats_add(gl.num_bind_buffer, 1);
        }
    } else {
        SOKOL_UNREACHABLE;
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_bind_storage_buffer(int stage, int slot, GLuint buffer) {
    SOKOL_ASSERT((stage >= 0) && (stage < SG_NUM_SHADER_STAGES));
    SOKOL_ASSERT((slot >= 0) && (slot < SG_MAX_SHADERSTAGE_STORAGEBUFFERS));
    if (_sg.gl.cache.stage_storage_buffers[stage][slot] != buffer) {
        _sg.gl.cache.stage_storage_buffers[stage][slot] = buffer;
        _sg.gl.cache.storage_buffer = buffer; // not a bug
        GLuint bind_index = _sg_gl_storagebuffer_bind_index(stage, slot);
        if (_sg.features.storage_buffer) {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_index, buffer);
        }
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_store_buffer_binding(GLenum target) {
    if (target == GL_ARRAY_BUFFER) {
        _sg.gl.cache.stored_vertex_buffer = _sg.gl.cache.vertex_buffer;
    } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
        _sg.gl.cache.stored_index_buffer = _sg.gl.cache.index_buffer;
    } else if (target == GL_SHADER_STORAGE_BUFFER) {
        _sg.gl.cache.stored_storage_buffer = _sg.gl.cache.storage_buffer;
    } else {
        SOKOL_UNREACHABLE;
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_restore_buffer_binding(GLenum target) {
    if (target == GL_ARRAY_BUFFER) {
        if (_sg.gl.cache.stored_vertex_buffer != 0) {
            // we only care about restoring valid ids
            _sg_gl_cache_bind_buffer(target, _sg.gl.cache.stored_vertex_buffer);
            _sg.gl.cache.stored_vertex_buffer = 0;
        }
    } else if (target == GL_ELEMENT_ARRAY_BUFFER) {
        if (_sg.gl.cache.stored_index_buffer != 0) {
            // we only care about restoring valid ids
            _sg_gl_cache_bind_buffer(target, _sg.gl.cache.stored_index_buffer);
            _sg.gl.cache.stored_index_buffer = 0;
        }
    } else if (target == GL_SHADER_STORAGE_BUFFER) {
        if (_sg.gl.cache.stored_storage_buffer != 0) {
            // we only care about restoring valid ids
            _sg_gl_cache_bind_buffer(target, _sg.gl.cache.stored_storage_buffer);
            _sg.gl.cache.stored_storage_buffer = 0;
        }
    } else {
        SOKOL_UNREACHABLE;
    }
}

// called when _sg_gl_discard_buffer()
_SOKOL_PRIVATE void _sg_gl_cache_invalidate_buffer(GLuint buf) {
    if (buf == _sg.gl.cache.vertex_buffer) {
        _sg.gl.cache.vertex_buffer = 0;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    if (buf == _sg.gl.cache.index_buffer) {
        _sg.gl.cache.index_buffer = 0;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    if (buf == _sg.gl.cache.storage_buffer) {
        _sg.gl.cache.storage_buffer = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        _sg_stats_add(gl.num_bind_buffer, 1);
    }
    for (int stage = 0; stage < SG_NUM_SHADER_STAGES; stage++) {
        for (int i = 0; i < SG_MAX_SHADERSTAGE_STORAGEBUFFERS; i++) {
            if (buf == _sg.gl.cache.stage_storage_buffers[stage][i]) {
                _sg.gl.cache.stage_storage_buffers[stage][i] = 0;
                _sg.gl.cache.storage_buffer = 0; // not a bug!
                const GLuint bind_index = _sg_gl_storagebuffer_bind_index(stage, i);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bind_index, 0);
                _sg_stats_add(gl.num_bind_buffer, 1);
            }
        }
    }
    if (buf == _sg.gl.cache.stored_vertex_buffer) {
        _sg.gl.cache.stored_vertex_buffer = 0;
    }
    if (buf == _sg.gl.cache.stored_index_buffer) {
        _sg.gl.cache.stored_index_buffer = 0;
    }
    if (buf == _sg.gl.cache.stored_storage_buffer) {
        _sg.gl.cache.stored_storage_buffer = 0;
    }
    for (int i = 0; i < SG_MAX_VERTEX_ATTRIBUTES; i++) {
        if (buf == _sg.gl.cache.attrs[i].gl_vbuf) {
            _sg.gl.cache.attrs[i].gl_vbuf = 0;
        }
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_active_texture(GLenum texture) {
    _SG_GL_CHECK_ERROR();
    if (_sg.gl.cache.cur_active_texture != texture) {
        _sg.gl.cache.cur_active_texture = texture;
        glActiveTexture(texture);
        _sg_stats_add(gl.num_active_texture, 1);
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_cache_clear_texture_sampler_bindings(bool force) {
    _SG_GL_CHECK_ERROR();
    for (int i = 0; (i < _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE) && (i < _sg.limits.gl_max_combined_texture_image_units); i++) {
        if (force || (_sg.gl.cache.texture_samplers[i].texture != 0)) {
            GLenum gl_texture_unit = (GLenum) (GL_TEXTURE0 + i);
            glActiveTexture(gl_texture_unit);
            _sg_stats_add(gl.num_active_texture, 1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            _sg_stats_add(gl.num_bind_texture, 4);
            glBindSampler((GLuint)i, 0);
            _sg_stats_add(gl.num_bind_sampler, 1);
            _sg.gl.cache.texture_samplers[i].target = 0;
            _sg.gl.cache.texture_samplers[i].texture = 0;
            _sg.gl.cache.texture_samplers[i].sampler = 0;
            _sg.gl.cache.cur_active_texture = gl_texture_unit;
        }
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_cache_bind_texture_sampler(int slot_index, GLenum target, GLuint texture, GLuint sampler) {
    /* it's valid to call this function with target=0 and/or texture=0
       target=0 will unbind the previous binding, texture=0 will clear
       the new binding
    */
    SOKOL_ASSERT((slot_index >= 0) && (slot_index < _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE));
    if (slot_index >= _sg.limits.gl_max_combined_texture_image_units) {
        return;
    }
    _SG_GL_CHECK_ERROR();
    _sg_gl_cache_texture_sampler_bind_slot* slot = &_sg.gl.cache.texture_samplers[slot_index];
    if ((slot->target != target) || (slot->texture != texture) || (slot->sampler != sampler)) {
        _sg_gl_cache_active_texture((GLenum)(GL_TEXTURE0 + slot_index));
        // if the target has changed, clear the previous binding on that target
        if ((target != slot->target) && (slot->target != 0)) {
            glBindTexture(slot->target, 0);
            _SG_GL_CHECK_ERROR();
            _sg_stats_add(gl.num_bind_texture, 1);
        }
        // apply new binding (can be 0 to unbind)
        if (target != 0) {
            glBindTexture(target, texture);
            _SG_GL_CHECK_ERROR();
            _sg_stats_add(gl.num_bind_texture, 1);
        }
        // apply new sampler (can be 0 to unbind)
        glBindSampler((GLuint)slot_index, sampler);
        _SG_GL_CHECK_ERROR();
        _sg_stats_add(gl.num_bind_sampler, 1);

        slot->target = target;
        slot->texture = texture;
        slot->sampler = sampler;
    }
}

_SOKOL_PRIVATE void _sg_gl_cache_store_texture_sampler_binding(int slot_index) {
    SOKOL_ASSERT((slot_index >= 0) && (slot_index < _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE));
    _sg.gl.cache.stored_texture_sampler = _sg.gl.cache.texture_samplers[slot_index];
}

_SOKOL_PRIVATE void _sg_gl_cache_restore_texture_sampler_binding(int slot_index) {
    SOKOL_ASSERT((slot_index >= 0) && (slot_index < _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE));
    _sg_gl_cache_texture_sampler_bind_slot* slot = &_sg.gl.cache.stored_texture_sampler;
    if (slot->texture != 0) {
        // we only care about restoring valid ids
        SOKOL_ASSERT(slot->target != 0);
        _sg_gl_cache_bind_texture_sampler(slot_index, slot->target, slot->texture, slot->sampler);
        slot->target = 0;
        slot->texture = 0;
        slot->sampler = 0;
    }
}

// called from _sg_gl_discard_texture() and _sg_gl_discard_sampler()
_SOKOL_PRIVATE void _sg_gl_cache_invalidate_texture_sampler(GLuint tex, GLuint smp) {
    _SG_GL_CHECK_ERROR();
    for (int i = 0; i < _SG_GL_TEXTURE_SAMPLER_CACHE_SIZE; i++) {
        _sg_gl_cache_texture_sampler_bind_slot* slot = &_sg.gl.cache.texture_samplers[i];
        if ((0 != slot->target) && ((tex == slot->texture) || (smp == slot->sampler))) {
            _sg_gl_cache_active_texture((GLenum)(GL_TEXTURE0 + i));
            glBindTexture(slot->target, 0);
            _SG_GL_CHECK_ERROR();
            _sg_stats_add(gl.num_bind_texture, 1);
            glBindSampler((GLuint)i, 0);
            _SG_GL_CHECK_ERROR();
            _sg_stats_add(gl.num_bind_sampler, 1);
            slot->target = 0;
            slot->texture = 0;
            slot->sampler = 0;
        }
    }
    if ((tex == _sg.gl.cache.stored_texture_sampler.texture) || (smp == _sg.gl.cache.stored_texture_sampler.sampler)) {
        _sg.gl.cache.stored_texture_sampler.target = 0;
        _sg.gl.cache.stored_texture_sampler.texture = 0;
        _sg.gl.cache.stored_texture_sampler.sampler = 0;
    }
}

// called from _sg_gl_discard_shader()
_SOKOL_PRIVATE void _sg_gl_cache_invalidate_program(GLuint prog) {
    if (prog == _sg.gl.cache.prog) {
        _sg.gl.cache.prog = 0;
        glUseProgram(0);
        _sg_stats_add(gl.num_use_program, 1);
    }
}

// called from _sg_gl_discard_pipeline()
_SOKOL_PRIVATE void _sg_gl_cache_invalidate_pipeline(_sg_pipeline_t* pip) {
    if (pip == _sg.gl.cache.cur_pipeline) {
        _sg.gl.cache.cur_pipeline = 0;
        _sg.gl.cache.cur_pipeline_id.id = SG_INVALID_ID;
    }
}

_SOKOL_PRIVATE void _sg_gl_reset_state_cache(void) {
    _SG_GL_CHECK_ERROR();
    glBindVertexArray(_sg.gl.vao);
    _SG_GL_CHECK_ERROR();
    _sg_clear(&_sg.gl.cache, sizeof(_sg.gl.cache));
    _sg_gl_cache_clear_buffer_bindings(true);
    _SG_GL_CHECK_ERROR();
    _sg_gl_cache_clear_texture_sampler_bindings(true);
    _SG_GL_CHECK_ERROR();
    for (int i = 0; i < _sg.limits.max_vertex_attrs; i++) {
        _sg_gl_attr_t* attr = &_sg.gl.cache.attrs[i].gl_attr;
        attr->vb_index = -1;
        attr->divisor = -1;
        glDisableVertexAttribArray((GLuint)i);
        _SG_GL_CHECK_ERROR();
        _sg_stats_add(gl.num_disable_vertex_attrib_array, 1);
    }
    _sg.gl.cache.cur_primitive_type = GL_TRIANGLES;

    // shader program
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&_sg.gl.cache.prog);
    _SG_GL_CHECK_ERROR();

    // depth and stencil state
    _sg.gl.cache.depth.compare = SG_COMPAREFUNC_ALWAYS;
    _sg.gl.cache.stencil.front.compare = SG_COMPAREFUNC_ALWAYS;
    _sg.gl.cache.stencil.front.fail_op = SG_STENCILOP_KEEP;
    _sg.gl.cache.stencil.front.depth_fail_op = SG_STENCILOP_KEEP;
    _sg.gl.cache.stencil.front.pass_op = SG_STENCILOP_KEEP;
    _sg.gl.cache.stencil.back.compare = SG_COMPAREFUNC_ALWAYS;
    _sg.gl.cache.stencil.back.fail_op = SG_STENCILOP_KEEP;
    _sg.gl.cache.stencil.back.depth_fail_op = SG_STENCILOP_KEEP;
    _sg.gl.cache.stencil.back.pass_op = SG_STENCILOP_KEEP;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0);
    _sg_stats_add(gl.num_render_state, 7);

    // blend state
    _sg.gl.cache.blend.src_factor_rgb = SG_BLENDFACTOR_ONE;
    _sg.gl.cache.blend.dst_factor_rgb = SG_BLENDFACTOR_ZERO;
    _sg.gl.cache.blend.op_rgb = SG_BLENDOP_ADD;
    _sg.gl.cache.blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    _sg.gl.cache.blend.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
    _sg.gl.cache.blend.op_alpha = SG_BLENDOP_ADD;
    glDisable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
    _sg_stats_add(gl.num_render_state, 4);

    // standalone state
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        _sg.gl.cache.color_write_mask[i] = SG_COLORMASK_RGBA;
    }
    _sg.gl.cache.cull_mode = SG_CULLMODE_NONE;
    _sg.gl.cache.face_winding = SG_FACEWINDING_CW;
    _sg.gl.cache.sample_count = 1;
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPolygonOffset(0.0f, 0.0f);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    glEnable(GL_DITHER);
    glDisable(GL_POLYGON_OFFSET_FILL);
    _sg_stats_add(gl.num_render_state, 10);
}

_SOKOL_PRIVATE void _sg_gl_setup_backend(const sg_desc* desc) {
    _SOKOL_UNUSED(desc);

    // assumes that _sg.gl is already zero-initialized
    _sg.gl.valid = true;

    #if defined(_SOKOL_USE_WIN32_GL_LOADER)
    _sg_gl_load_opengl();
    #endif

    // clear initial GL error state
    #if defined(SOKOL_DEBUG)
        while (glGetError() != GL_NO_ERROR);
    #endif
    _sg_gl_init_caps_gles3();

    glGenVertexArrays(1, &_sg.gl.vao);
    glBindVertexArray(_sg.gl.vao);
    _SG_GL_CHECK_ERROR();
    // incoming texture data is generally expected to be packed tightly
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    _sg_gl_reset_state_cache();
}

_SOKOL_PRIVATE void _sg_gl_discard_backend(void) {
    SOKOL_ASSERT(_sg.gl.valid);
    if (_sg.gl.vao) {
        glDeleteVertexArrays(1, &_sg.gl.vao);
    }
    #if defined(_SOKOL_USE_WIN32_GL_LOADER)
    _sg_gl_unload_opengl();
    #endif
    _sg.gl.valid = false;
}

//-- GL backend resource creation and destruction ------------------------------
_SOKOL_PRIVATE sg_resource_state _sg_gl_create_buffer(_sg_buffer_t* buf, const sg_buffer_desc* desc) {
    SOKOL_ASSERT(buf && desc);
    _SG_GL_CHECK_ERROR();
    buf->gl.injected = (0 != desc->gl_buffers[0]);
    const GLenum gl_target = _sg_gl_buffer_target(buf->cmn.type);
    const GLenum gl_usage  = _sg_gl_usage(buf->cmn.usage);
    for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
        GLuint gl_buf = 0;
        if (buf->gl.injected) {
            SOKOL_ASSERT(desc->gl_buffers[slot]);
            gl_buf = desc->gl_buffers[slot];
        } else {
            glGenBuffers(1, &gl_buf);
            SOKOL_ASSERT(gl_buf);
            _sg_gl_cache_store_buffer_binding(gl_target);
            _sg_gl_cache_bind_buffer(gl_target, gl_buf);
            glBufferData(gl_target, buf->cmn.size, 0, gl_usage);
            if (buf->cmn.usage == SG_USAGE_IMMUTABLE) {
                SOKOL_ASSERT(desc->data.ptr);
                glBufferSubData(gl_target, 0, buf->cmn.size, desc->data.ptr);
            }
            _sg_gl_cache_restore_buffer_binding(gl_target);
        }
        buf->gl.buf[slot] = gl_buf;
    }
    _SG_GL_CHECK_ERROR();
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_buffer(_sg_buffer_t* buf) {
    SOKOL_ASSERT(buf);
    _SG_GL_CHECK_ERROR();
    for (int slot = 0; slot < buf->cmn.num_slots; slot++) {
        if (buf->gl.buf[slot]) {
            _sg_gl_cache_invalidate_buffer(buf->gl.buf[slot]);
            if (!buf->gl.injected) {
                glDeleteBuffers(1, &buf->gl.buf[slot]);
            }
        }
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE bool _sg_gl_supported_texture_format(sg_pixel_format fmt) {
    const int fmt_index = (int) fmt;
    SOKOL_ASSERT((fmt_index > SG_PIXELFORMAT_NONE) && (fmt_index < _SG_PIXELFORMAT_NUM));
    return _sg.formats[fmt_index].sample;
}

_SOKOL_PRIVATE sg_resource_state _sg_gl_create_image(_sg_image_t* img, const sg_image_desc* desc) {
    SOKOL_ASSERT(img && desc);
    _SG_GL_CHECK_ERROR();
    img->gl.injected = (0 != desc->gl_textures[0]);

    // check if texture format is support
    if (!_sg_gl_supported_texture_format(img->cmn.pixel_format)) {
        _SG_ERROR(GL_TEXTURE_FORMAT_NOT_SUPPORTED);
        return SG_RESOURCESTATE_FAILED;
    }
    const GLenum gl_internal_format = _sg_gl_teximage_internal_format(img->cmn.pixel_format);

    // if this is a MSAA render target, a render buffer object will be created instead of a regulat texture
    // (since GLES3 has no multisampled texture objects)
    if (img->cmn.render_target && (img->cmn.sample_count > 1)) {
        glGenRenderbuffers(1, &img->gl.msaa_render_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, img->gl.msaa_render_buffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, img->cmn.sample_count, gl_internal_format, img->cmn.width, img->cmn.height);
    } else if (img->gl.injected) {
        img->gl.target = _sg_gl_texture_target(img->cmn.type);
        // inject externally GL textures
        for (int slot = 0; slot < img->cmn.num_slots; slot++) {
            SOKOL_ASSERT(desc->gl_textures[slot]);
            img->gl.tex[slot] = desc->gl_textures[slot];
        }
        if (desc->gl_texture_target) {
            img->gl.target = (GLenum)desc->gl_texture_target;
        }
    } else {
        // create our own GL texture(s)
        img->gl.target = _sg_gl_texture_target(img->cmn.type);
        const GLenum gl_format = _sg_gl_teximage_format(img->cmn.pixel_format);
        const bool is_compressed = _sg_is_compressed_pixel_format(img->cmn.pixel_format);
        for (int slot = 0; slot < img->cmn.num_slots; slot++) {
            glGenTextures(1, &img->gl.tex[slot]);
            SOKOL_ASSERT(img->gl.tex[slot]);
            _sg_gl_cache_store_texture_sampler_binding(0);
            _sg_gl_cache_bind_texture_sampler(0, img->gl.target, img->gl.tex[slot], 0);
            glTexParameteri(img->gl.target, GL_TEXTURE_MAX_LEVEL, img->cmn.num_mipmaps - 1);
            const int num_faces = img->cmn.type == SG_IMAGETYPE_CUBE ? 6 : 1;
            int data_index = 0;
            for (int face_index = 0; face_index < num_faces; face_index++) {
                for (int mip_index = 0; mip_index < img->cmn.num_mipmaps; mip_index++, data_index++) {
                    GLenum gl_img_target = img->gl.target;
                    if (SG_IMAGETYPE_CUBE == img->cmn.type) {
                        gl_img_target = _sg_gl_cubeface_target(face_index);
                    }
                    const GLvoid* data_ptr = desc->data.subimage[face_index][mip_index].ptr;
                    const int mip_width = _sg_miplevel_dim(img->cmn.width, mip_index);
                    const int mip_height = _sg_miplevel_dim(img->cmn.height, mip_index);
                    if ((SG_IMAGETYPE_2D == img->cmn.type) || (SG_IMAGETYPE_CUBE == img->cmn.type)) {
                        if (is_compressed) {
                            const GLsizei data_size = (GLsizei) desc->data.subimage[face_index][mip_index].size;
                            glCompressedTexImage2D(gl_img_target, mip_index, gl_internal_format,
                                mip_width, mip_height, 0, data_size, data_ptr);
                        } else {
                            const GLenum gl_type = _sg_gl_teximage_type(img->cmn.pixel_format);
                            glTexImage2D(gl_img_target, mip_index, (GLint)gl_internal_format,
                                mip_width, mip_height, 0, gl_format, gl_type, data_ptr);
                        }
                    } else if ((SG_IMAGETYPE_3D == img->cmn.type) || (SG_IMAGETYPE_ARRAY == img->cmn.type)) {
                        int mip_depth = img->cmn.num_slices;
                        if (SG_IMAGETYPE_3D == img->cmn.type) {
                            mip_depth = _sg_miplevel_dim(mip_depth, mip_index);
                        }
                        if (is_compressed) {
                            const GLsizei data_size = (GLsizei) desc->data.subimage[face_index][mip_index].size;
                            glCompressedTexImage3D(gl_img_target, mip_index, gl_internal_format,
                                mip_width, mip_height, mip_depth, 0, data_size, data_ptr);
                        } else {
                            const GLenum gl_type = _sg_gl_teximage_type(img->cmn.pixel_format);
                            glTexImage3D(gl_img_target, mip_index, (GLint)gl_internal_format,
                                mip_width, mip_height, mip_depth, 0, gl_format, gl_type, data_ptr);
                        }
                    }
                }
            }
            _sg_gl_cache_restore_texture_sampler_binding(0);
        }
    }
    _SG_GL_CHECK_ERROR();
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_image(_sg_image_t* img) {
    SOKOL_ASSERT(img);
    _SG_GL_CHECK_ERROR();
    for (int slot = 0; slot < img->cmn.num_slots; slot++) {
        if (img->gl.tex[slot]) {
            _sg_gl_cache_invalidate_texture_sampler(img->gl.tex[slot], 0);
            if (!img->gl.injected) {
                glDeleteTextures(1, &img->gl.tex[slot]);
            }
        }
    }
    if (img->gl.msaa_render_buffer) {
        glDeleteRenderbuffers(1, &img->gl.msaa_render_buffer);
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE sg_resource_state _sg_gl_create_sampler(_sg_sampler_t* smp, const sg_sampler_desc* desc) {
    SOKOL_ASSERT(smp && desc);
    _SG_GL_CHECK_ERROR();
    smp->gl.injected = (0 != desc->gl_sampler);
    if (smp->gl.injected) {
        smp->gl.smp = (GLuint) desc->gl_sampler;
    } else {
        glGenSamplers(1, &smp->gl.smp);
        SOKOL_ASSERT(smp->gl.smp);

        const GLenum gl_min_filter = _sg_gl_min_filter(smp->cmn.min_filter, smp->cmn.mipmap_filter);
        const GLenum gl_mag_filter = _sg_gl_mag_filter(smp->cmn.mag_filter);
        glSamplerParameteri(smp->gl.smp, GL_TEXTURE_MIN_FILTER, (GLint)gl_min_filter);
        glSamplerParameteri(smp->gl.smp, GL_TEXTURE_MAG_FILTER, (GLint)gl_mag_filter);
        // GL spec has strange defaults for mipmap min/max lod: -1000 to +1000
        const float min_lod = _sg_clamp(desc->min_lod, 0.0f, 1000.0f);
        const float max_lod = _sg_clamp(desc->max_lod, 0.0f, 1000.0f);
        glSamplerParameterf(smp->gl.smp, GL_TEXTURE_MIN_LOD, min_lod);
        glSamplerParameterf(smp->gl.smp, GL_TEXTURE_MAX_LOD, max_lod);
        glSamplerParameteri(smp->gl.smp, GL_TEXTURE_WRAP_S, (GLint)_sg_gl_wrap(smp->cmn.wrap_u));
        glSamplerParameteri(smp->gl.smp, GL_TEXTURE_WRAP_T, (GLint)_sg_gl_wrap(smp->cmn.wrap_v));
        glSamplerParameteri(smp->gl.smp, GL_TEXTURE_WRAP_R, (GLint)_sg_gl_wrap(smp->cmn.wrap_w));
        if (smp->cmn.compare != SG_COMPAREFUNC_NEVER) {
            glSamplerParameteri(smp->gl.smp, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glSamplerParameteri(smp->gl.smp, GL_TEXTURE_COMPARE_FUNC, (GLint)_sg_gl_compare_func(smp->cmn.compare));
        } else {
            glSamplerParameteri(smp->gl.smp, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }
        if (_sg.gl.ext_anisotropic && (smp->cmn.max_anisotropy > 1)) {
            GLint max_aniso = (GLint) smp->cmn.max_anisotropy;
            if (max_aniso > _sg.gl.max_anisotropy) {
                max_aniso = _sg.gl.max_anisotropy;
            }
            glSamplerParameteri(smp->gl.smp, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
        }
    }
    _SG_GL_CHECK_ERROR();
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_sampler(_sg_sampler_t* smp) {
    SOKOL_ASSERT(smp);
    _SG_GL_CHECK_ERROR();
    _sg_gl_cache_invalidate_texture_sampler(0, smp->gl.smp);
    if (!smp->gl.injected) {
        glDeleteSamplers(1, &smp->gl.smp);
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE GLuint _sg_gl_compile_shader(sg_shader_stage stage, const char* src) {
    SOKOL_ASSERT(src);
    _SG_GL_CHECK_ERROR();
    GLuint gl_shd = glCreateShader(_sg_gl_shader_stage(stage));
    glShaderSource(gl_shd, 1, &src, 0);
    glCompileShader(gl_shd);
    GLint compile_status = 0;
    glGetShaderiv(gl_shd, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        // compilation failed, log error and delete shader
        GLint log_len = 0;
        glGetShaderiv(gl_shd, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_buf = (GLchar*) _sg_malloc((size_t)log_len);
            glGetShaderInfoLog(gl_shd, log_len, &log_len, log_buf);
            _SG_ERROR(GL_SHADER_COMPILATION_FAILED);
            _SG_LOGMSG(GL_SHADER_COMPILATION_FAILED, log_buf);
            _sg_free(log_buf);
        }
        glDeleteShader(gl_shd);
        gl_shd = 0;
    }
    _SG_GL_CHECK_ERROR();
    return gl_shd;
}

_SOKOL_PRIVATE sg_resource_state _sg_gl_create_shader(_sg_shader_t* shd, const sg_shader_desc* desc) {
    SOKOL_ASSERT(shd && desc);
    SOKOL_ASSERT(!shd->gl.prog);
    _SG_GL_CHECK_ERROR();

    // copy the optional vertex attribute names over
    for (int i = 0; i < SG_MAX_VERTEX_ATTRIBUTES; i++) {
        _sg_strcpy(&shd->gl.attrs[i].name, desc->attrs[i].name);
    }

    GLuint gl_vs = _sg_gl_compile_shader(SG_SHADERSTAGE_VS, desc->vs.source);
    GLuint gl_fs = _sg_gl_compile_shader(SG_SHADERSTAGE_FS, desc->fs.source);
    if (!(gl_vs && gl_fs)) {
        return SG_RESOURCESTATE_FAILED;
    }
    GLuint gl_prog = glCreateProgram();
    glAttachShader(gl_prog, gl_vs);
    glAttachShader(gl_prog, gl_fs);
    glLinkProgram(gl_prog);
    glDeleteShader(gl_vs);
    glDeleteShader(gl_fs);
    _SG_GL_CHECK_ERROR();

    GLint link_status;
    glGetProgramiv(gl_prog, GL_LINK_STATUS, &link_status);
    if (!link_status) {
        GLint log_len = 0;
        glGetProgramiv(gl_prog, GL_INFO_LOG_LENGTH, &log_len);
        if (log_len > 0) {
            GLchar* log_buf = (GLchar*) _sg_malloc((size_t)log_len);
            glGetProgramInfoLog(gl_prog, log_len, &log_len, log_buf);
            _SG_ERROR(GL_SHADER_LINKING_FAILED);
            _SG_LOGMSG(GL_SHADER_LINKING_FAILED, log_buf);
            _sg_free(log_buf);
        }
        glDeleteProgram(gl_prog);
        return SG_RESOURCESTATE_FAILED;
    }
    shd->gl.prog = gl_prog;

    // resolve uniforms
    _SG_GL_CHECK_ERROR();
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        const sg_shader_stage_desc* stage_desc = (stage_index == SG_SHADERSTAGE_VS)? &desc->vs : &desc->fs;
        const _sg_shader_stage_t* stage = &shd->cmn.stage[stage_index];
        _sg_gl_shader_stage_t* gl_stage = &shd->gl.stage[stage_index];
        for (int ub_index = 0; ub_index < stage->num_uniform_blocks; ub_index++) {
            const sg_shader_uniform_block_desc* ub_desc = &stage_desc->uniform_blocks[ub_index];
            SOKOL_ASSERT(ub_desc->size > 0);
            _sg_gl_uniform_block_t* ub = &gl_stage->uniform_blocks[ub_index];
            SOKOL_ASSERT(ub->num_uniforms == 0);
            uint32_t cur_uniform_offset = 0;
            for (int u_index = 0; u_index < SG_MAX_UB_MEMBERS; u_index++) {
                const sg_shader_uniform_desc* u_desc = &ub_desc->uniforms[u_index];
                if (u_desc->type == SG_UNIFORMTYPE_INVALID) {
                    break;
                }
                const uint32_t u_align = _sg_uniform_alignment(u_desc->type, u_desc->array_count, ub_desc->layout);
                const uint32_t u_size = _sg_uniform_size(u_desc->type, u_desc->array_count, ub_desc->layout);
                cur_uniform_offset = _sg_align_u32(cur_uniform_offset, u_align);
                _sg_gl_uniform_t* u = &ub->uniforms[u_index];
                u->type = u_desc->type;
                u->count = (uint16_t) u_desc->array_count;
                u->offset = (uint16_t) cur_uniform_offset;
                cur_uniform_offset += u_size;
                if (u_desc->name) {
                    u->gl_loc = glGetUniformLocation(gl_prog, u_desc->name);
                } else {
                    u->gl_loc = u_index;
                }
                ub->num_uniforms++;
            }
            if (ub_desc->layout == SG_UNIFORMLAYOUT_STD140) {
                cur_uniform_offset = _sg_align_u32(cur_uniform_offset, 16);
            }
            SOKOL_ASSERT(ub_desc->size == (size_t)cur_uniform_offset);
            _SOKOL_UNUSED(cur_uniform_offset);
        }
    }

    // resolve combined image samplers
    _SG_GL_CHECK_ERROR();
    GLuint cur_prog = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&cur_prog);
    glUseProgram(gl_prog);
    int gl_tex_slot = 0;
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        const sg_shader_stage_desc* stage_desc = (stage_index == SG_SHADERSTAGE_VS)? &desc->vs : &desc->fs;
        const _sg_shader_stage_t* stage = &shd->cmn.stage[stage_index];
        _sg_gl_shader_stage_t* gl_stage = &shd->gl.stage[stage_index];
        for (int img_smp_index = 0; img_smp_index < stage->num_image_samplers; img_smp_index++) {
            const sg_shader_image_sampler_pair_desc* img_smp_desc = &stage_desc->image_sampler_pairs[img_smp_index];
            _sg_gl_shader_image_sampler_t* gl_img_smp = &gl_stage->image_samplers[img_smp_index];
            SOKOL_ASSERT(img_smp_desc->glsl_name);
            GLint gl_loc = glGetUniformLocation(gl_prog, img_smp_desc->glsl_name);
            if (gl_loc != -1) {
                gl_img_smp->gl_tex_slot = gl_tex_slot++;
                glUniform1i(gl_loc, gl_img_smp->gl_tex_slot);
            } else {
                gl_img_smp->gl_tex_slot = -1;
                _SG_ERROR(GL_TEXTURE_NAME_NOT_FOUND_IN_SHADER);
                _SG_LOGMSG(GL_TEXTURE_NAME_NOT_FOUND_IN_SHADER, img_smp_desc->glsl_name);
            }
        }
    }
    // it's legal to call glUseProgram with 0
    glUseProgram(cur_prog);
    _SG_GL_CHECK_ERROR();
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_shader(_sg_shader_t* shd) {
    SOKOL_ASSERT(shd);
    _SG_GL_CHECK_ERROR();
    if (shd->gl.prog) {
        _sg_gl_cache_invalidate_program(shd->gl.prog);
        glDeleteProgram(shd->gl.prog);
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE sg_resource_state _sg_gl_create_pipeline(_sg_pipeline_t* pip, _sg_shader_t* shd, const sg_pipeline_desc* desc) {
    SOKOL_ASSERT(pip && shd && desc);
    SOKOL_ASSERT((pip->shader == 0) && (pip->cmn.shader_id.id != SG_INVALID_ID));
    SOKOL_ASSERT(desc->shader.id == shd->slot.id);
    SOKOL_ASSERT(shd->gl.prog);
    pip->shader = shd;
    pip->gl.primitive_type = desc->primitive_type;
    pip->gl.depth = desc->depth;
    pip->gl.stencil = desc->stencil;
    // FIXME: blend color and write mask per draw-buffer-attachment (requires GL4)
    pip->gl.blend = desc->colors[0].blend;
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        pip->gl.color_write_mask[i] = desc->colors[i].write_mask;
    }
    pip->gl.cull_mode = desc->cull_mode;
    pip->gl.face_winding = desc->face_winding;
    pip->gl.sample_count = desc->sample_count;
    pip->gl.alpha_to_coverage_enabled = desc->alpha_to_coverage_enabled;

    // resolve vertex attributes
    for (int attr_index = 0; attr_index < SG_MAX_VERTEX_ATTRIBUTES; attr_index++) {
        pip->gl.attrs[attr_index].vb_index = -1;
    }
    for (int attr_index = 0; attr_index < _sg.limits.max_vertex_attrs; attr_index++) {
        const sg_vertex_attr_state* a_state = &desc->layout.attrs[attr_index];
        if (a_state->format == SG_VERTEXFORMAT_INVALID) {
            break;
        }
        SOKOL_ASSERT(a_state->buffer_index < SG_MAX_VERTEX_BUFFERS);
        const sg_vertex_buffer_layout_state* l_state = &desc->layout.buffers[a_state->buffer_index];
        const sg_vertex_step step_func = l_state->step_func;
        const int step_rate = l_state->step_rate;
        GLint attr_loc = attr_index;
        if (!_sg_strempty(&shd->gl.attrs[attr_index].name)) {
            attr_loc = glGetAttribLocation(pip->shader->gl.prog, _sg_strptr(&shd->gl.attrs[attr_index].name));
        }
        SOKOL_ASSERT(attr_loc < (GLint)_sg.limits.max_vertex_attrs);
        if (attr_loc != -1) {
            _sg_gl_attr_t* gl_attr = &pip->gl.attrs[attr_loc];
            SOKOL_ASSERT(gl_attr->vb_index == -1);
            gl_attr->vb_index = (int8_t) a_state->buffer_index;
            if (step_func == SG_VERTEXSTEP_PER_VERTEX) {
                gl_attr->divisor = 0;
            } else {
                gl_attr->divisor = (int8_t) step_rate;
                pip->cmn.use_instanced_draw = true;
            }
            SOKOL_ASSERT(l_state->stride > 0);
            gl_attr->stride = (uint8_t) l_state->stride;
            gl_attr->offset = a_state->offset;
            gl_attr->size = (uint8_t) _sg_gl_vertexformat_size(a_state->format);
            gl_attr->type = _sg_gl_vertexformat_type(a_state->format);
            gl_attr->normalized = _sg_gl_vertexformat_normalized(a_state->format);
            pip->cmn.vertex_buffer_layout_active[a_state->buffer_index] = true;
        } else {
            _SG_ERROR(GL_VERTEX_ATTRIBUTE_NOT_FOUND_IN_SHADER);
            _SG_LOGMSG(GL_VERTEX_ATTRIBUTE_NOT_FOUND_IN_SHADER, _sg_strptr(&shd->gl.attrs[attr_index].name));
        }
    }
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    _sg_gl_cache_invalidate_pipeline(pip);
}

_SOKOL_PRIVATE void _sg_gl_fb_attach_texture(const _sg_gl_attachment_t* gl_att, const _sg_attachment_common_t* cmn_att, GLenum gl_att_type) {
    const _sg_image_t* img = gl_att->image;
    SOKOL_ASSERT(img);
    const GLuint gl_tex = img->gl.tex[0];
    SOKOL_ASSERT(gl_tex);
    const GLuint gl_target = img->gl.target;
    SOKOL_ASSERT(gl_target);
    const int mip_level = cmn_att->mip_level;
    const int slice = cmn_att->slice;
    switch (img->cmn.type) {
        case SG_IMAGETYPE_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_att_type, gl_target, gl_tex, mip_level);
            break;
        case SG_IMAGETYPE_CUBE:
            glFramebufferTexture2D(GL_FRAMEBUFFER, gl_att_type, _sg_gl_cubeface_target(slice), gl_tex, mip_level);
            break;
        default:
            glFramebufferTextureLayer(GL_FRAMEBUFFER, gl_att_type, gl_tex, mip_level, slice);
            break;
    }
}

_SOKOL_PRIVATE GLenum _sg_gl_depth_stencil_attachment_type(const _sg_gl_attachment_t* ds_att) {
    const _sg_image_t* img = ds_att->image;
    SOKOL_ASSERT(img);
    if (_sg_is_depth_stencil_format(img->cmn.pixel_format)) {
        return GL_DEPTH_STENCIL_ATTACHMENT;
    } else {
        return GL_DEPTH_ATTACHMENT;
    }
}

_SOKOL_PRIVATE sg_resource_state _sg_gl_create_attachments(_sg_attachments_t* atts, _sg_image_t** color_images, _sg_image_t** resolve_images, _sg_image_t* ds_image, const sg_attachments_desc* desc) {
    SOKOL_ASSERT(atts && desc);
    SOKOL_ASSERT(color_images && resolve_images);
    _SG_GL_CHECK_ERROR();

    // copy image pointers
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const sg_attachment_desc* color_desc = &desc->colors[i];
        _SOKOL_UNUSED(color_desc);
        SOKOL_ASSERT(color_desc->image.id != SG_INVALID_ID);
        SOKOL_ASSERT(0 == atts->gl.colors[i].image);
        SOKOL_ASSERT(color_images[i] && (color_images[i]->slot.id == color_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_color_format(color_images[i]->cmn.pixel_format));
        atts->gl.colors[i].image = color_images[i];

        const sg_attachment_desc* resolve_desc = &desc->resolves[i];
        if (resolve_desc->image.id != SG_INVALID_ID) {
            SOKOL_ASSERT(0 == atts->gl.resolves[i].image);
            SOKOL_ASSERT(resolve_images[i] && (resolve_images[i]->slot.id == resolve_desc->image.id));
            SOKOL_ASSERT(color_images[i] && (color_images[i]->cmn.pixel_format == resolve_images[i]->cmn.pixel_format));
            atts->gl.resolves[i].image = resolve_images[i];
        }
    }
    SOKOL_ASSERT(0 == atts->gl.depth_stencil.image);
    const sg_attachment_desc* ds_desc = &desc->depth_stencil;
    if (ds_desc->image.id != SG_INVALID_ID) {
        SOKOL_ASSERT(ds_image && (ds_image->slot.id == ds_desc->image.id));
        SOKOL_ASSERT(_sg_is_valid_rendertarget_depth_format(ds_image->cmn.pixel_format));
        atts->gl.depth_stencil.image = ds_image;
    }

    // store current framebuffer binding (restored at end of function)
    GLuint gl_orig_fb;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&gl_orig_fb);

    // create a framebuffer object
    glGenFramebuffers(1, &atts->gl.fb);
    glBindFramebuffer(GL_FRAMEBUFFER, atts->gl.fb);

    // attach color attachments to framebuffer
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        const _sg_image_t* color_img = atts->gl.colors[i].image;
        SOKOL_ASSERT(color_img);
        const GLuint gl_msaa_render_buffer = color_img->gl.msaa_render_buffer;
        if (gl_msaa_render_buffer) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0+i), GL_RENDERBUFFER, gl_msaa_render_buffer);
        } else {
            const GLenum gl_att_type = (GLenum)(GL_COLOR_ATTACHMENT0 + i);
            _sg_gl_fb_attach_texture(&atts->gl.colors[i], &atts->cmn.colors[i], gl_att_type);
        }
    }
    // attach depth-stencil attachment
    if (atts->gl.depth_stencil.image) {
        const GLenum gl_att = _sg_gl_depth_stencil_attachment_type(&atts->gl.depth_stencil);
        const _sg_image_t* ds_img = atts->gl.depth_stencil.image;
        const GLuint gl_msaa_render_buffer = ds_img->gl.msaa_render_buffer;
        if (gl_msaa_render_buffer) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, gl_att, GL_RENDERBUFFER, gl_msaa_render_buffer);
        } else {
            const GLenum gl_att_type = _sg_gl_depth_stencil_attachment_type(&atts->gl.depth_stencil);
            _sg_gl_fb_attach_texture(&atts->gl.depth_stencil, &atts->cmn.depth_stencil, gl_att_type);
        }
    }

    // check if framebuffer is complete
    {
        const GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fb_status != GL_FRAMEBUFFER_COMPLETE) {
            switch (fb_status) {
                case GL_FRAMEBUFFER_UNDEFINED:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNDEFINED);
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_ATTACHMENT);
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MISSING_ATTACHMENT);
                    break;
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNSUPPORTED);
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MULTISAMPLE);
                    break;
                default:
                    _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNKNOWN);
                    break;
            }
            return SG_RESOURCESTATE_FAILED;
        }
    }

    // setup color attachments for the framebuffer
    static const GLenum gl_draw_bufs[SG_MAX_COLOR_ATTACHMENTS] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    glDrawBuffers(atts->cmn.num_colors, gl_draw_bufs);

    // create MSAA resolve framebuffers if necessary
    for (int i = 0; i < atts->cmn.num_colors; i++) {
        _sg_gl_attachment_t* gl_resolve_att = &atts->gl.resolves[i];
        if (gl_resolve_att->image) {
            _sg_attachment_common_t* cmn_resolve_att = &atts->cmn.resolves[i];
            SOKOL_ASSERT(0 == atts->gl.msaa_resolve_framebuffer[i]);
            glGenFramebuffers(1, &atts->gl.msaa_resolve_framebuffer[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, atts->gl.msaa_resolve_framebuffer[i]);
            _sg_gl_fb_attach_texture(gl_resolve_att, cmn_resolve_att, GL_COLOR_ATTACHMENT0);
            // check if framebuffer is complete
            const GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fb_status != GL_FRAMEBUFFER_COMPLETE) {
                switch (fb_status) {
                    case GL_FRAMEBUFFER_UNDEFINED:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNDEFINED);
                        break;
                    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_ATTACHMENT);
                        break;
                    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MISSING_ATTACHMENT);
                        break;
                    case GL_FRAMEBUFFER_UNSUPPORTED:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNSUPPORTED);
                        break;
                    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_INCOMPLETE_MULTISAMPLE);
                        break;
                    default:
                        _SG_ERROR(GL_FRAMEBUFFER_STATUS_UNKNOWN);
                        break;
                }
                return SG_RESOURCESTATE_FAILED;
            }
            // setup color attachments for the framebuffer
            glDrawBuffers(1, &gl_draw_bufs[0]);
        }
    }

    // restore original framebuffer binding
    glBindFramebuffer(GL_FRAMEBUFFER, gl_orig_fb);
    _SG_GL_CHECK_ERROR();
    return SG_RESOURCESTATE_VALID;
}

_SOKOL_PRIVATE void _sg_gl_discard_attachments(_sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    _SG_GL_CHECK_ERROR();
    if (0 != atts->gl.fb) {
        glDeleteFramebuffers(1, &atts->gl.fb);
    }
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        if (atts->gl.msaa_resolve_framebuffer[i]) {
            glDeleteFramebuffers(1, &atts->gl.msaa_resolve_framebuffer[i]);
        }
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE _sg_image_t* _sg_gl_attachments_color_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->gl.colors[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_gl_attachments_resolve_image(const _sg_attachments_t* atts, int index) {
    SOKOL_ASSERT(atts && (index >= 0) && (index < SG_MAX_COLOR_ATTACHMENTS));
    return atts->gl.resolves[index].image;
}

_SOKOL_PRIVATE _sg_image_t* _sg_gl_attachments_ds_image(const _sg_attachments_t* atts) {
    SOKOL_ASSERT(atts);
    return atts->gl.depth_stencil.image;
}

_SOKOL_PRIVATE void _sg_gl_begin_pass(const sg_pass* pass) {
    // FIXME: what if a texture used as render target is still bound, should we
    // unbind all currently bound textures in begin pass?
    SOKOL_ASSERT(pass);
    _SG_GL_CHECK_ERROR();
    const _sg_attachments_t* atts = _sg.cur_pass.atts;
    const sg_swapchain* swapchain = &pass->swapchain;
    const sg_pass_action* action = &pass->action;

    // bind the render pass framebuffer
    //
    // FIXME: Disabling SRGB conversion for the default framebuffer is
    // a crude hack to make behaviour for sRGB render target textures
    // identical with the Metal and D3D11 swapchains created by sokol-app.
    //
    // This will need a cleaner solution (e.g. allowing to configure
    // sokol_app.h with an sRGB or RGB framebuffer.
    if (atts) {
        // offscreen pass
        SOKOL_ASSERT(atts->gl.fb);
        glBindFramebuffer(GL_FRAMEBUFFER, atts->gl.fb);
    } else {
        // default pass
        // NOTE: on some platforms, the default framebuffer of a context
        // is null, so we can't actually assert here that the
        // framebuffer has been provided
        glBindFramebuffer(GL_FRAMEBUFFER, swapchain->gl.framebuffer);
    }
    glViewport(0, 0, _sg.cur_pass.width, _sg.cur_pass.height);
    glScissor(0, 0, _sg.cur_pass.width, _sg.cur_pass.height);

    // number of color attachments
    const int num_color_atts = atts ? atts->cmn.num_colors : 1;

    // clear color and depth-stencil attachments if needed
    bool clear_any_color = false;
    for (int i = 0; i < num_color_atts; i++) {
        if (SG_LOADACTION_CLEAR == action->colors[i].load_action) {
            clear_any_color = true;
            break;
        }
    }
    const bool clear_depth = (action->depth.load_action == SG_LOADACTION_CLEAR);
    const bool clear_stencil = (action->stencil.load_action == SG_LOADACTION_CLEAR);

    bool need_pip_cache_flush = false;
    if (clear_any_color) {
        bool need_color_mask_flush = false;
        // NOTE: not a bug to iterate over all possible color attachments
        for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
            if (SG_COLORMASK_RGBA != _sg.gl.cache.color_write_mask[i]) {
                need_pip_cache_flush = true;
                need_color_mask_flush = true;
                _sg.gl.cache.color_write_mask[i] = SG_COLORMASK_RGBA;
            }
        }
        if (need_color_mask_flush) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        }
    }
    if (clear_depth) {
        if (!_sg.gl.cache.depth.write_enabled) {
            need_pip_cache_flush = true;
            _sg.gl.cache.depth.write_enabled = true;
            glDepthMask(GL_TRUE);
        }
        if (_sg.gl.cache.depth.compare != SG_COMPAREFUNC_ALWAYS) {
            need_pip_cache_flush = true;
            _sg.gl.cache.depth.compare = SG_COMPAREFUNC_ALWAYS;
            glDepthFunc(GL_ALWAYS);
        }
    }
    if (clear_stencil) {
        if (_sg.gl.cache.stencil.write_mask != 0xFF) {
            need_pip_cache_flush = true;
            _sg.gl.cache.stencil.write_mask = 0xFF;
            glStencilMask(0xFF);
        }
    }
    if (need_pip_cache_flush) {
        // we messed with the state cache directly, need to clear cached
        // pipeline to force re-evaluation in next sg_apply_pipeline()
        _sg.gl.cache.cur_pipeline = 0;
        _sg.gl.cache.cur_pipeline_id.id = SG_INVALID_ID;
    }
    for (int i = 0; i < num_color_atts; i++) {
        if (action->colors[i].load_action == SG_LOADACTION_CLEAR) {
            glClearBufferfv(GL_COLOR, i, &action->colors[i].clear_value.r);
        }
    }
    if ((atts == 0) || (atts->gl.depth_stencil.image)) {
        if (clear_depth && clear_stencil) {
            glClearBufferfi(GL_DEPTH_STENCIL, 0, action->depth.clear_value, action->stencil.clear_value);
        } else if (clear_depth) {
            glClearBufferfv(GL_DEPTH, 0, &action->depth.clear_value);
        } else if (clear_stencil) {
            GLint val = (GLint) action->stencil.clear_value;
            glClearBufferiv(GL_STENCIL, 0, &val);
        }
    }
    // keep store actions for end-pass
    for (int i = 0; i < SG_MAX_COLOR_ATTACHMENTS; i++) {
        _sg.gl.color_store_actions[i] = action->colors[i].store_action;
    }
    _sg.gl.depth_store_action = action->depth.store_action;
    _sg.gl.stencil_store_action = action->stencil.store_action;

    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_end_pass(void) {
    _SG_GL_CHECK_ERROR();

    if (_sg.cur_pass.atts) {
        const _sg_attachments_t* atts = _sg.cur_pass.atts;
        SOKOL_ASSERT(atts->slot.id == _sg.cur_pass.atts_id.id);
        bool fb_read_bound = false;
        bool fb_draw_bound = false;
        const int num_color_atts = atts->cmn.num_colors;
        for (int i = 0; i < num_color_atts; i++) {
            // perform MSAA resolve if needed
            if (atts->gl.msaa_resolve_framebuffer[i] != 0) {
                if (!fb_read_bound) {
                    SOKOL_ASSERT(atts->gl.fb);
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, atts->gl.fb);
                    fb_read_bound = true;
                }
                const int w = atts->gl.colors[i].image->cmn.width;
                const int h = atts->gl.colors[i].image->cmn.height;
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, atts->gl.msaa_resolve_framebuffer[i]);
                glReadBuffer((GLenum)(GL_COLOR_ATTACHMENT0 + i));
                glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                fb_draw_bound = true;
            }
        }

        // invalidate framebuffers
        _SOKOL_UNUSED(fb_draw_bound);
        #if defined(SOKOL_GLES3)
        // need to restore framebuffer binding before invalidate if the MSAA resolve had changed the binding
        if (fb_draw_bound) {
            glBindFramebuffer(GL_FRAMEBUFFER, atts->gl.fb);
        }
        GLenum invalidate_atts[SG_MAX_COLOR_ATTACHMENTS + 2] = { 0 };
        int att_index = 0;
        for (int i = 0; i < num_color_atts; i++) {
            if (_sg.gl.color_store_actions[i] == SG_STOREACTION_DONTCARE) {
                invalidate_atts[att_index++] = (GLenum)(GL_COLOR_ATTACHMENT0 + i);
            }
        }
        if ((_sg.gl.depth_store_action == SG_STOREACTION_DONTCARE) && (_sg.cur_pass.atts->cmn.depth_stencil.image_id.id != SG_INVALID_ID)) {
            invalidate_atts[att_index++] = GL_DEPTH_ATTACHMENT;
        }
        if ((_sg.gl.stencil_store_action == SG_STOREACTION_DONTCARE) && (_sg.cur_pass.atts->cmn.depth_stencil.image_id.id != SG_INVALID_ID)) {
            invalidate_atts[att_index++] = GL_STENCIL_ATTACHMENT;
        }
        if (att_index > 0) {
            glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, att_index, invalidate_atts);
        }
        #endif
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_apply_viewport(int x, int y, int w, int h, bool origin_top_left) {
    y = origin_top_left ? (_sg.cur_pass.height - (y+h)) : y;
    glViewport(x, y, w, h);
}

_SOKOL_PRIVATE void _sg_gl_apply_scissor_rect(int x, int y, int w, int h, bool origin_top_left) {
    y = origin_top_left ? (_sg.cur_pass.height - (y+h)) : y;
    glScissor(x, y, w, h);
}

_SOKOL_PRIVATE void _sg_gl_apply_pipeline(_sg_pipeline_t* pip) {
    SOKOL_ASSERT(pip);
    SOKOL_ASSERT(pip->shader && (pip->cmn.shader_id.id == pip->shader->slot.id));
    _SG_GL_CHECK_ERROR();
    if ((_sg.gl.cache.cur_pipeline != pip) || (_sg.gl.cache.cur_pipeline_id.id != pip->slot.id)) {
        _sg.gl.cache.cur_pipeline = pip;
        _sg.gl.cache.cur_pipeline_id.id = pip->slot.id;
        _sg.gl.cache.cur_primitive_type = _sg_gl_primitive_type(pip->gl.primitive_type);
        _sg.gl.cache.cur_index_type = _sg_gl_index_type(pip->cmn.index_type);

        // update depth state
        {
            const sg_depth_state* state_ds = &pip->gl.depth;
            sg_depth_state* cache_ds = &_sg.gl.cache.depth;
            if (state_ds->compare != cache_ds->compare) {
                cache_ds->compare = state_ds->compare;
                glDepthFunc(_sg_gl_compare_func(state_ds->compare));
                _sg_stats_add(gl.num_render_state, 1);
            }
            if (state_ds->write_enabled != cache_ds->write_enabled) {
                cache_ds->write_enabled = state_ds->write_enabled;
                glDepthMask(state_ds->write_enabled);
                _sg_stats_add(gl.num_render_state, 1);
            }
            if (!_sg_fequal(state_ds->bias, cache_ds->bias, 0.000001f) ||
                !_sg_fequal(state_ds->bias_slope_scale, cache_ds->bias_slope_scale, 0.000001f))
            {
                /* according to ANGLE's D3D11 backend:
                    D3D11 SlopeScaledDepthBias ==> GL polygonOffsetFactor
                    D3D11 DepthBias ==> GL polygonOffsetUnits
                    DepthBiasClamp has no meaning on GL
                */
                cache_ds->bias = state_ds->bias;
                cache_ds->bias_slope_scale = state_ds->bias_slope_scale;
                glPolygonOffset(state_ds->bias_slope_scale, state_ds->bias);
                _sg_stats_add(gl.num_render_state, 1);
                bool po_enabled = true;
                if (_sg_fequal(state_ds->bias, 0.0f, 0.000001f) &&
                    _sg_fequal(state_ds->bias_slope_scale, 0.0f, 0.000001f))
                {
                    po_enabled = false;
                }
                if (po_enabled != _sg.gl.cache.polygon_offset_enabled) {
                    _sg.gl.cache.polygon_offset_enabled = po_enabled;
                    if (po_enabled) {
                        glEnable(GL_POLYGON_OFFSET_FILL);
                    } else {
                        glDisable(GL_POLYGON_OFFSET_FILL);
                    }
                    _sg_stats_add(gl.num_render_state, 1);
                }
            }
        }

        // update stencil state
        {
            const sg_stencil_state* state_ss = &pip->gl.stencil;
            sg_stencil_state* cache_ss = &_sg.gl.cache.stencil;
            if (state_ss->enabled != cache_ss->enabled) {
                cache_ss->enabled = state_ss->enabled;
                if (state_ss->enabled) {
                    glEnable(GL_STENCIL_TEST);
                } else {
                    glDisable(GL_STENCIL_TEST);
                }
                _sg_stats_add(gl.num_render_state, 1);
            }
            if (state_ss->write_mask != cache_ss->write_mask) {
                cache_ss->write_mask = state_ss->write_mask;
                glStencilMask(state_ss->write_mask);
                _sg_stats_add(gl.num_render_state, 1);
            }
            for (int i = 0; i < 2; i++) {
                const sg_stencil_face_state* state_sfs = (i==0)? &state_ss->front : &state_ss->back;
                sg_stencil_face_state* cache_sfs = (i==0)? &cache_ss->front : &cache_ss->back;
                GLenum gl_face = (i==0)? GL_FRONT : GL_BACK;
                if ((state_sfs->compare != cache_sfs->compare) ||
                    (state_ss->read_mask != cache_ss->read_mask) ||
                    (state_ss->ref != cache_ss->ref))
                {
                    cache_sfs->compare = state_sfs->compare;
                    glStencilFuncSeparate(gl_face,
                        _sg_gl_compare_func(state_sfs->compare),
                        state_ss->ref,
                        state_ss->read_mask);
                    _sg_stats_add(gl.num_render_state, 1);
                }
                if ((state_sfs->fail_op != cache_sfs->fail_op) ||
                    (state_sfs->depth_fail_op != cache_sfs->depth_fail_op) ||
                    (state_sfs->pass_op != cache_sfs->pass_op))
                {
                    cache_sfs->fail_op = state_sfs->fail_op;
                    cache_sfs->depth_fail_op = state_sfs->depth_fail_op;
                    cache_sfs->pass_op = state_sfs->pass_op;
                    glStencilOpSeparate(gl_face,
                        _sg_gl_stencil_op(state_sfs->fail_op),
                        _sg_gl_stencil_op(state_sfs->depth_fail_op),
                        _sg_gl_stencil_op(state_sfs->pass_op));
                    _sg_stats_add(gl.num_render_state, 1);
                }
            }
            cache_ss->read_mask = state_ss->read_mask;
            cache_ss->ref = state_ss->ref;
        }

        if (pip->cmn.color_count > 0) {
            // update blend state
            // FIXME: separate blend state per color attachment not support, needs GL4
            const sg_blend_state* state_bs = &pip->gl.blend;
            sg_blend_state* cache_bs = &_sg.gl.cache.blend;
            if (state_bs->enabled != cache_bs->enabled) {
                cache_bs->enabled = state_bs->enabled;
                if (state_bs->enabled) {
                    glEnable(GL_BLEND);
                } else {
                    glDisable(GL_BLEND);
                }
                _sg_stats_add(gl.num_render_state, 1);
            }
            if ((state_bs->src_factor_rgb != cache_bs->src_factor_rgb) ||
                (state_bs->dst_factor_rgb != cache_bs->dst_factor_rgb) ||
                (state_bs->src_factor_alpha != cache_bs->src_factor_alpha) ||
                (state_bs->dst_factor_alpha != cache_bs->dst_factor_alpha))
            {
                cache_bs->src_factor_rgb = state_bs->src_factor_rgb;
                cache_bs->dst_factor_rgb = state_bs->dst_factor_rgb;
                cache_bs->src_factor_alpha = state_bs->src_factor_alpha;
                cache_bs->dst_factor_alpha = state_bs->dst_factor_alpha;
                glBlendFuncSeparate(_sg_gl_blend_factor(state_bs->src_factor_rgb),
                    _sg_gl_blend_factor(state_bs->dst_factor_rgb),
                    _sg_gl_blend_factor(state_bs->src_factor_alpha),
                    _sg_gl_blend_factor(state_bs->dst_factor_alpha));
                _sg_stats_add(gl.num_render_state, 1);
            }
            if ((state_bs->op_rgb != cache_bs->op_rgb) || (state_bs->op_alpha != cache_bs->op_alpha)) {
                cache_bs->op_rgb = state_bs->op_rgb;
                cache_bs->op_alpha = state_bs->op_alpha;
                glBlendEquationSeparate(_sg_gl_blend_op(state_bs->op_rgb), _sg_gl_blend_op(state_bs->op_alpha));
                _sg_stats_add(gl.num_render_state, 1);
            }

            // standalone color target state
            for (GLuint i = 0; i < (GLuint)pip->cmn.color_count; i++) {
                if (pip->gl.color_write_mask[i] != _sg.gl.cache.color_write_mask[i]) {
                    const sg_color_mask cm = pip->gl.color_write_mask[i];
                    _sg.gl.cache.color_write_mask[i] = cm;
                    if (0 == i) {
                        glColorMask((cm & SG_COLORMASK_R) != 0,
                                    (cm & SG_COLORMASK_G) != 0,
                                    (cm & SG_COLORMASK_B) != 0,
                                    (cm & SG_COLORMASK_A) != 0);
                    }
                    _sg_stats_add(gl.num_render_state, 1);
                }
            }

            if (!_sg_fequal(pip->cmn.blend_color.r, _sg.gl.cache.blend_color.r, 0.0001f) ||
                !_sg_fequal(pip->cmn.blend_color.g, _sg.gl.cache.blend_color.g, 0.0001f) ||
                !_sg_fequal(pip->cmn.blend_color.b, _sg.gl.cache.blend_color.b, 0.0001f) ||
                !_sg_fequal(pip->cmn.blend_color.a, _sg.gl.cache.blend_color.a, 0.0001f))
            {
                sg_color c = pip->cmn.blend_color;
                _sg.gl.cache.blend_color = c;
                glBlendColor(c.r, c.g, c.b, c.a);
                _sg_stats_add(gl.num_render_state, 1);
            }
        } // pip->cmn.color_count > 0

        if (pip->gl.cull_mode != _sg.gl.cache.cull_mode) {
            _sg.gl.cache.cull_mode = pip->gl.cull_mode;
            if (SG_CULLMODE_NONE == pip->gl.cull_mode) {
                glDisable(GL_CULL_FACE);
                _sg_stats_add(gl.num_render_state, 1);
            } else {
                glEnable(GL_CULL_FACE);
                GLenum gl_mode = (SG_CULLMODE_FRONT == pip->gl.cull_mode) ? GL_FRONT : GL_BACK;
                glCullFace(gl_mode);
                _sg_stats_add(gl.num_render_state, 2);
            }
        }
        if (pip->gl.face_winding != _sg.gl.cache.face_winding) {
            _sg.gl.cache.face_winding = pip->gl.face_winding;
            GLenum gl_winding = (SG_FACEWINDING_CW == pip->gl.face_winding) ? GL_CW : GL_CCW;
            glFrontFace(gl_winding);
            _sg_stats_add(gl.num_render_state, 1);
        }
        if (pip->gl.alpha_to_coverage_enabled != _sg.gl.cache.alpha_to_coverage_enabled) {
            _sg.gl.cache.alpha_to_coverage_enabled = pip->gl.alpha_to_coverage_enabled;
            if (pip->gl.alpha_to_coverage_enabled) {
                glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            } else {
                glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
            }
            _sg_stats_add(gl.num_render_state, 1);
        }
        // bind shader program
        if (pip->shader->gl.prog != _sg.gl.cache.prog) {
            _sg.gl.cache.prog = pip->shader->gl.prog;
            glUseProgram(pip->shader->gl.prog);
            _sg_stats_add(gl.num_use_program, 1);
        }
    }
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE bool _sg_gl_apply_bindings(_sg_bindings_t* bnd) {
    SOKOL_ASSERT(bnd);
    SOKOL_ASSERT(bnd->pip);
    _SG_GL_CHECK_ERROR();

    // bind combined image-samplers
    _SG_GL_CHECK_ERROR();
    for (int stage_index = 0; stage_index < SG_NUM_SHADER_STAGES; stage_index++) {
        const _sg_shader_stage_t* stage = &bnd->pip->shader->cmn.stage[stage_index];
        const _sg_gl_shader_stage_t* gl_stage = &bnd->pip->shader->gl.stage[stage_index];
        _sg_image_t** imgs = (stage_index == SG_SHADERSTAGE_VS) ? bnd->vs_imgs : bnd->fs_imgs;
        _sg_sampler_t** smps = (stage_index == SG_SHADERSTAGE_VS) ? bnd->vs_smps : bnd->fs_smps;
        const int num_imgs = (stage_index == SG_SHADERSTAGE_VS) ? bnd->num_vs_imgs : bnd->num_fs_imgs;
        const int num_smps = (stage_index == SG_SHADERSTAGE_VS) ? bnd->num_vs_smps : bnd->num_fs_smps;
        SOKOL_ASSERT(num_imgs == stage->num_images); _SOKOL_UNUSED(num_imgs);
        SOKOL_ASSERT(num_smps == stage->num_samplers); _SOKOL_UNUSED(num_smps);
        for (int img_smp_index = 0; img_smp_index < stage->num_image_samplers; img_smp_index++) {
            const int gl_tex_slot = gl_stage->image_samplers[img_smp_index].gl_tex_slot;
            if (gl_tex_slot != -1) {
                const int img_index = stage->image_samplers[img_smp_index].image_slot;
                const int smp_index = stage->image_samplers[img_smp_index].sampler_slot;
                SOKOL_ASSERT(img_index < num_imgs);
                SOKOL_ASSERT(smp_index < num_smps);
                _sg_image_t* img = imgs[img_index];
                _sg_sampler_t* smp = smps[smp_index];
                const GLenum gl_tgt = img->gl.target;
                const GLuint gl_tex = img->gl.tex[img->cmn.active_slot];
                const GLuint gl_smp = smp->gl.smp;
                _sg_gl_cache_bind_texture_sampler(gl_tex_slot, gl_tgt, gl_tex, gl_smp);
            }
        }
    }
    _SG_GL_CHECK_ERROR();

    // bind storage buffers
    for (int slot = 0; slot < bnd->num_vs_sbufs; slot++) {
        _sg_buffer_t* sb = bnd->vs_sbufs[slot];
        GLuint gl_sb = sb->gl.buf[sb->cmn.active_slot];
        _sg_gl_cache_bind_storage_buffer(SG_SHADERSTAGE_VS, slot, gl_sb);
    }
    for (int slot = 0; slot < bnd->num_fs_sbufs; slot++) {
        _sg_buffer_t* sb = bnd->fs_sbufs[slot];
        GLuint gl_sb = sb->gl.buf[sb->cmn.active_slot];
        _sg_gl_cache_bind_storage_buffer(SG_SHADERSTAGE_FS, slot, gl_sb);
    }

    // index buffer (can be 0)
    const GLuint gl_ib = bnd->ib ? bnd->ib->gl.buf[bnd->ib->cmn.active_slot] : 0;
    _sg_gl_cache_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, gl_ib);
    _sg.gl.cache.cur_ib_offset = bnd->ib_offset;

    // vertex attributes
    for (GLuint attr_index = 0; attr_index < (GLuint)_sg.limits.max_vertex_attrs; attr_index++) {
        _sg_gl_attr_t* attr = &bnd->pip->gl.attrs[attr_index];
        _sg_gl_cache_attr_t* cache_attr = &_sg.gl.cache.attrs[attr_index];
        bool cache_attr_dirty = false;
        int vb_offset = 0;
        GLuint gl_vb = 0;
        if (attr->vb_index >= 0) {
            // attribute is enabled
            SOKOL_ASSERT(attr->vb_index < bnd->num_vbs);
            _sg_buffer_t* vb = bnd->vbs[attr->vb_index];
            SOKOL_ASSERT(vb);
            gl_vb = vb->gl.buf[vb->cmn.active_slot];
            vb_offset = bnd->vb_offsets[attr->vb_index] + attr->offset;
            if ((gl_vb != cache_attr->gl_vbuf) ||
                (attr->size != cache_attr->gl_attr.size) ||
                (attr->type != cache_attr->gl_attr.type) ||
                (attr->normalized != cache_attr->gl_attr.normalized) ||
                (attr->stride != cache_attr->gl_attr.stride) ||
                (vb_offset != cache_attr->gl_attr.offset) ||
                (cache_attr->gl_attr.divisor != attr->divisor))
            {
                _sg_gl_cache_bind_buffer(GL_ARRAY_BUFFER, gl_vb);
                glVertexAttribPointer(attr_index, attr->size, attr->type, attr->normalized, attr->stride, (const GLvoid*)(GLintptr)vb_offset);
                _sg_stats_add(gl.num_vertex_attrib_pointer, 1);
                glVertexAttribDivisor(attr_index, (GLuint)attr->divisor);
                _sg_stats_add(gl.num_vertex_attrib_divisor, 1);
                cache_attr_dirty = true;
            }
            if (cache_attr->gl_attr.vb_index == -1) {
                glEnableVertexAttribArray(attr_index);
                _sg_stats_add(gl.num_enable_vertex_attrib_array, 1);
                cache_attr_dirty = true;
            }
        } else {
            // attribute is disabled
            if (cache_attr->gl_attr.vb_index != -1) {
                glDisableVertexAttribArray(attr_index);
                _sg_stats_add(gl.num_disable_vertex_attrib_array, 1);
                cache_attr_dirty = true;
            }
        }
        if (cache_attr_dirty) {
            cache_attr->gl_attr = *attr;
            cache_attr->gl_attr.offset = vb_offset;
            cache_attr->gl_vbuf = gl_vb;
        }
    }
    _SG_GL_CHECK_ERROR();
    return true;
}

_SOKOL_PRIVATE void _sg_gl_apply_uniforms(sg_shader_stage stage_index, int ub_index, const sg_range* data) {
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline);
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline->slot.id == _sg.gl.cache.cur_pipeline_id.id);
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline->shader->slot.id == _sg.gl.cache.cur_pipeline->cmn.shader_id.id);
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline->shader->cmn.stage[stage_index].num_uniform_blocks > ub_index);
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline->shader->cmn.stage[stage_index].uniform_blocks[ub_index].size == data->size);
    const _sg_gl_shader_stage_t* gl_stage = &_sg.gl.cache.cur_pipeline->shader->gl.stage[stage_index];
    const _sg_gl_uniform_block_t* gl_ub = &gl_stage->uniform_blocks[ub_index];
    for (int u_index = 0; u_index < gl_ub->num_uniforms; u_index++) {
        const _sg_gl_uniform_t* u = &gl_ub->uniforms[u_index];
        SOKOL_ASSERT(u->type != SG_UNIFORMTYPE_INVALID);
        if (u->gl_loc == -1) {
            continue;
        }
        _sg_stats_add(gl.num_uniform, 1);
        GLfloat* fptr = (GLfloat*) (((uint8_t*)data->ptr) + u->offset);
        GLint* iptr = (GLint*) (((uint8_t*)data->ptr) + u->offset);
        switch (u->type) {
            case SG_UNIFORMTYPE_INVALID:
                break;
            case SG_UNIFORMTYPE_FLOAT:
                glUniform1fv(u->gl_loc, u->count, fptr);
                break;
            case SG_UNIFORMTYPE_FLOAT2:
                glUniform2fv(u->gl_loc, u->count, fptr);
                break;
            case SG_UNIFORMTYPE_FLOAT3:
                glUniform3fv(u->gl_loc, u->count, fptr);
                break;
            case SG_UNIFORMTYPE_FLOAT4:
                glUniform4fv(u->gl_loc, u->count, fptr);
                break;
            case SG_UNIFORMTYPE_INT:
                glUniform1iv(u->gl_loc, u->count, iptr);
                break;
            case SG_UNIFORMTYPE_INT2:
                glUniform2iv(u->gl_loc, u->count, iptr);
                break;
            case SG_UNIFORMTYPE_INT3:
                glUniform3iv(u->gl_loc, u->count, iptr);
                break;
            case SG_UNIFORMTYPE_INT4:
                glUniform4iv(u->gl_loc, u->count, iptr);
                break;
            case SG_UNIFORMTYPE_MAT4:
                glUniformMatrix4fv(u->gl_loc, u->count, GL_FALSE, fptr);
                break;
            default:
                SOKOL_UNREACHABLE;
                break;
        }
    }
}

_SOKOL_PRIVATE void _sg_gl_draw(int base_element, int num_elements, int num_instances) {
    SOKOL_ASSERT(_sg.gl.cache.cur_pipeline);
    const GLenum i_type = _sg.gl.cache.cur_index_type;
    const GLenum p_type = _sg.gl.cache.cur_primitive_type;
    const bool use_instanced_draw = (num_instances > 1) || (_sg.gl.cache.cur_pipeline->cmn.use_instanced_draw);
    if (0 != i_type) {
        // indexed rendering
        const int i_size = (i_type == GL_UNSIGNED_SHORT) ? 2 : 4;
        const int ib_offset = _sg.gl.cache.cur_ib_offset;
        const GLvoid* indices = (const GLvoid*)(GLintptr)(base_element*i_size+ib_offset);
        if (use_instanced_draw) {
            glDrawElementsInstanced(p_type, num_elements, i_type, indices, num_instances);
        } else {
            glDrawElements(p_type, num_elements, i_type, indices);
        }
    } else {
        // non-indexed rendering
        if (use_instanced_draw) {
            glDrawArraysInstanced(p_type, base_element, num_elements, num_instances);
        } else {
            glDrawArrays(p_type, base_element, num_elements);
        }
    }
}

_SOKOL_PRIVATE void _sg_gl_commit(void) {
    // "soft" clear bindings (only those that are actually bound)
    _sg_gl_cache_clear_buffer_bindings(false);
    _sg_gl_cache_clear_texture_sampler_bindings(false);
}

_SOKOL_PRIVATE void _sg_gl_update_buffer(_sg_buffer_t* buf, const sg_range* data) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    // only one update per buffer per frame allowed
    if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
        buf->cmn.active_slot = 0;
    }
    GLenum gl_tgt = _sg_gl_buffer_target(buf->cmn.type);
    SOKOL_ASSERT(buf->cmn.active_slot < SG_NUM_INFLIGHT_FRAMES);
    GLuint gl_buf = buf->gl.buf[buf->cmn.active_slot];
    SOKOL_ASSERT(gl_buf);
    _SG_GL_CHECK_ERROR();
    _sg_gl_cache_store_buffer_binding(gl_tgt);
    _sg_gl_cache_bind_buffer(gl_tgt, gl_buf);
    glBufferSubData(gl_tgt, 0, (GLsizeiptr)data->size, data->ptr);
    _sg_gl_cache_restore_buffer_binding(gl_tgt);
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_append_buffer(_sg_buffer_t* buf, const sg_range* data, bool new_frame) {
    SOKOL_ASSERT(buf && data && data->ptr && (data->size > 0));
    if (new_frame) {
        if (++buf->cmn.active_slot >= buf->cmn.num_slots) {
            buf->cmn.active_slot = 0;
        }
    }
    GLenum gl_tgt = _sg_gl_buffer_target(buf->cmn.type);
    SOKOL_ASSERT(buf->cmn.active_slot < SG_NUM_INFLIGHT_FRAMES);
    GLuint gl_buf = buf->gl.buf[buf->cmn.active_slot];
    SOKOL_ASSERT(gl_buf);
    _SG_GL_CHECK_ERROR();
    _sg_gl_cache_store_buffer_binding(gl_tgt);
    _sg_gl_cache_bind_buffer(gl_tgt, gl_buf);
    glBufferSubData(gl_tgt, buf->cmn.append_pos, (GLsizeiptr)data->size, data->ptr);
    _sg_gl_cache_restore_buffer_binding(gl_tgt);
    _SG_GL_CHECK_ERROR();
}

_SOKOL_PRIVATE void _sg_gl_update_image(_sg_image_t* img, const sg_image_data* data) {
    SOKOL_ASSERT(img && data);
    // only one update per image per frame allowed
    if (++img->cmn.active_slot >= img->cmn.num_slots) {
        img->cmn.active_slot = 0;
    }
    SOKOL_ASSERT(img->cmn.active_slot < SG_NUM_INFLIGHT_FRAMES);
    SOKOL_ASSERT(0 != img->gl.tex[img->cmn.active_slot]);
    _sg_gl_cache_store_texture_sampler_binding(0);
    _sg_gl_cache_bind_texture_sampler(0, img->gl.target, img->gl.tex[img->cmn.active_slot], 0);
    const GLenum gl_img_format = _sg_gl_teximage_format(img->cmn.pixel_format);
    const GLenum gl_img_type = _sg_gl_teximage_type(img->cmn.pixel_format);
    const int num_faces = img->cmn.type == SG_IMAGETYPE_CUBE ? 6 : 1;
    const int num_mips = img->cmn.num_mipmaps;
    for (int face_index = 0; face_index < num_faces; face_index++) {
        for (int mip_index = 0; mip_index < num_mips; mip_index++) {
            GLenum gl_img_target = img->gl.target;
            if (SG_IMAGETYPE_CUBE == img->cmn.type) {
                gl_img_target = _sg_gl_cubeface_target(face_index);
            }
            const GLvoid* data_ptr = data->subimage[face_index][mip_index].ptr;
            int mip_width = _sg_miplevel_dim(img->cmn.width, mip_index);
            int mip_height = _sg_miplevel_dim(img->cmn.height, mip_index);
            if ((SG_IMAGETYPE_2D == img->cmn.type) || (SG_IMAGETYPE_CUBE == img->cmn.type)) {
                glTexSubImage2D(gl_img_target, mip_index,
                    0, 0,
                    mip_width, mip_height,
                    gl_img_format, gl_img_type,
                    data_ptr);
            } else if ((SG_IMAGETYPE_3D == img->cmn.type) || (SG_IMAGETYPE_ARRAY == img->cmn.type)) {
                int mip_depth = img->cmn.num_slices;
                if (SG_IMAGETYPE_3D == img->cmn.type) {
                    mip_depth = _sg_miplevel_dim(img->cmn.num_slices, mip_index);
                }
                glTexSubImage3D(gl_img_target, mip_index,
                    0, 0, 0,
                    mip_width, mip_height, mip_depth,
                    gl_img_format, gl_img_type,
                    data_ptr);

            }
        }
    }
    _sg_gl_cache_restore_texture_sampler_binding(0);
}

