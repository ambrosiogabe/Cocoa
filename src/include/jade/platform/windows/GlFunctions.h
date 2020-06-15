#ifndef GL_FUNCTIONS_H
#define GL_FUNCTIONS_H

#include <windows.h>

#include <gl/GL.h>
#include <gl/glext.h>

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define GLDECL WINAPI
#define JADE_GL_LIST \
    /* return name parameters */ \
    GLE(void,      BlendEquation,              GLenum mode) \
    GLE(void,      ActiveTexture,              GLenum texture) \
    GLE(void,      AttachShader,               GLuint program, GLuint shader) \
    GLE(void,      BindBuffer,                 GLenum target, GLuint buffer) \
    GLE(void,      BindFramebuffer,            GLenum target, GLuint framebuffer) \
    GLE(void,      BindSampler,                GLuint unit, GLuint sampler) \
    GLE(void,      BindVertexArray,            GLuint array) \
    GLE(void,      BlendEquationSeparate,      GLenum modeRGB, GLenum modeAlpha) \
    GLE(void,      BlendFuncSeparate,          GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) \
    GLE(void,      BufferData,                 GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
    GLE(void,      BufferSubData,              GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data) \
    GLE(GLenum,    CheckFramebufferStatus,     GLenum target) \
    GLE(void,      ClearBufferfv,              GLenum buffer, GLint drawbuffer, const GLfloat * value) \
    GLE(void,      CompileShader,              GLuint shader) \
    GLE(GLuint,    CreateProgram,              void) \
    GLE(GLuint,    CreateShader,               GLenum type) \
    GLE(void,      DeleteBuffers,              GLsizei n, const GLuint *buffers) \
    GLE(void,      DeleteFramebuffers,         GLsizei n, const GLuint *framebuffers) \
    GLE(void,      DeleteProgram,              GLuint program) \
    GLE(void,      DeleteShader,               GLuint shader) \
    GLE(void,      DeleteVertexArrays,         GLsizei n, const GLuint *arrays) \
    GLE(void,      DetachShader,               GLuint program, GLuint shader) \
    GLE(void,      DisableVertexAttribArray,   GLuint index) \
    GLE(void,      DrawBuffers,                GLsizei n, const GLenum *bufs) \
    GLE(void,      DrawElementsBaseVertex,     GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex) \
    GLE(void,      EnableVertexAttribArray,    GLuint index) \
    GLE(void,      FramebufferTexture2D,       GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
    GLE(void,      FramebufferRenderbuffer,    GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) \
    GLE(void,      GenBuffers,                 GLsizei n, GLuint *buffers) \
    GLE(void,      GenFramebuffers,            GLsizei n, GLuint * framebuffers) \
    GLE(void,      GenRenderbuffers,           GLsizei n, GLuint *renderbuffers) \
    GLE(void,      GenVertexArrays,            GLsizei n, GLuint *arrays) \
    GLE(GLint,     GetAttribLocation,          GLuint program, const GLchar *name) \
    GLE(void,      GetProgramInfoLog,          GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    GLE(void,      GetProgramiv,               GLuint program, GLenum pname, GLint *params) \
    GLE(void,      GetShaderInfoLog,           GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
    GLE(void,      GetShaderiv,                GLuint shader, GLenum pname, GLint *params) \
    GLE(GLubyte*,  GetStringi,                 GLenum name, GLuint index) \
    GLE(GLint,     GetUniformLocation,         GLuint program, const GLchar *name) \
    GLE(void,      LinkProgram,                GLuint program) \
    GLE(void,      RenderbufferStorage,        GLenum target, GLenum internalformat, GLsizei width, GLsizei height) \
    GLE(void,      ShaderSource,               GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) \
    GLE(void,      Uniform1i,                  GLint location, GLint v0) \
    GLE(void,      Uniform1iv,                 GLint location, GLsizei count, const GLint *value) \
    GLE(void,      Uniform1f,                  GLint location, GLfloat v0) \
    GLE(void,      Uniform2f,                  GLint location, GLfloat v0, GLfloat v1) \
    GLE(void,      Uniform3f,                  GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
    GLE(void,      Uniform4f,                  GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
    GLE(void,      UniformMatrix3fv,           GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    GLE(void,      UniformMatrix4fv,           GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
    GLE(void,      UseProgram,                 GLuint program) \
    GLE(void,      VertexAttribPointer,        GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) \
    /* end */

#define GLE(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); extern name##proc * gl##name;
JADE_GL_LIST
#undef GLE

bool glLiteInit();

#endif //GL_LITE_H

#ifdef GL_LITE_IMPLEMENTATION

#define GLE(ret, name, ...) name##proc* gl##name;
JADE_GL_LIST
#undef GLE

bool glLiteInit() {
    HINSTANCE dll = LoadLibraryA("opengl32.dll");
    typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);
    if (!dll) {
        OutputDebugStringA("opengl32.dll not found.\n");
        return false;
    }

    wglGetProcAddressproc* wglGetProcAddress = (wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

    #define GLE(ret, name, ...)                                                                     \
        gl##name = (name##proc*)wglGetProcAddress("gl"#name);                                       \
        if (!gl##name) {                                                                            \
            OutputDebugStringA("Function gl"#name" couldn't be loaded from opengl32.dll\n");        \
            return false;                                                                           \
        }                                                                                           
        JADE_GL_LIST                                                                      
    #undef GLE

    return true;
}

#endif // GL_LITE_IMPLEMENTATION