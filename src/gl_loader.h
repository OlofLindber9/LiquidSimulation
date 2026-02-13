#pragma once

// Include GLFW which pulls in the system GL 1.1 header on Windows.
// We only need to add GL 2.0+ types, constants, and function pointers.
#include <GLFW/glfw3.h>
#include <cstddef>

// ---- Types not in the GL 1.1 header ----
typedef char      GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// ---- Constants not in the GL 1.1 header ----
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER        0x8B30
#endif
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER          0x8B31
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS         0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS            0x8B82
#endif
#ifndef GL_INFO_LOG_LENGTH
#define GL_INFO_LOG_LENGTH        0x8B84
#endif
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER           0x8892
#endif
#ifndef GL_STATIC_DRAW
#define GL_STATIC_DRAW            0x88E4
#endif
#ifndef GL_DYNAMIC_DRAW
#define GL_DYNAMIC_DRAW           0x88E8
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER            0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0      0x8CE0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE   0x8CD5
#endif
#ifndef GL_TEXTURE0
#define GL_TEXTURE0               0x84C0
#endif
#ifndef GL_RGBA16F
#define GL_RGBA16F                0x881A
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE          0x812F
#endif
#ifndef GL_PROGRAM_POINT_SIZE
#define GL_PROGRAM_POINT_SIZE     0x8642
#endif

// ---- Function pointer types (APIENTRY = __stdcall on Windows) ----

// GL 1.3
typedef void     (APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);

// GL 2.0 — Shaders
typedef GLuint   (APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void     (APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef void     (APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void     (APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void     (APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void     (APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLen, GLsizei *length, GLchar *infoLog);

// GL 2.0 — Programs
typedef GLuint   (APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void     (APIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void     (APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void     (APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void     (APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void     (APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void     (APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei maxLen, GLsizei *length, GLchar *infoLog);

// GL 2.0 — Uniforms / Attributes
typedef GLint    (APIENTRY *PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar *name);
typedef GLint    (APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void     (APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void     (APIENTRY *PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void     (APIENTRY *PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);

// GL 2.0 — Buffers
typedef void     (APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void     (APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void     (APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void     (APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void     (APIENTRY *PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

// GL 2.0 — Vertex attribs
typedef void     (APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void     (APIENTRY *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void     (APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

// GL 3.0 — VAO
typedef void     (APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void     (APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void     (APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);

// GL 3.0 — FBO
typedef void     (APIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint *ids);
typedef void     (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint *ids);
typedef void     (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void     (APIENTRY *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum   (APIENTRY *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);

// ---- Extern function pointer declarations ----

extern PFNGLACTIVETEXTUREPROC            glActiveTexture;

extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;

extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;

extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORM1IPROC               glUniform1i;
extern PFNGLUNIFORM1FPROC               glUniform1f;
extern PFNGLUNIFORM2FPROC               glUniform2f;

extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLBUFFERSUBDATAPROC            glBufferSubData;

extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;

extern PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC          glBindVertexArray;

extern PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus;

// Load all GL 2.0+ function pointers via glfwGetProcAddress.
// Call AFTER glfwMakeContextCurrent(). Returns true on success.
bool loadGL();
