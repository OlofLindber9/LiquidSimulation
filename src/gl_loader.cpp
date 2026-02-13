#include "gl_loader.h"
#include <cstdio>

// ---- Define all function pointers (initially null) ----

PFNGLACTIVETEXTUREPROC            glActiveTexture            = nullptr;

PFNGLCREATESHADERPROC             glCreateShader             = nullptr;
PFNGLDELETESHADERPROC             glDeleteShader             = nullptr;
PFNGLSHADERSOURCEPROC             glShaderSource             = nullptr;
PFNGLCOMPILESHADERPROC            glCompileShader            = nullptr;
PFNGLGETSHADERIVPROC              glGetShaderiv              = nullptr;
PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog         = nullptr;

PFNGLCREATEPROGRAMPROC            glCreateProgram            = nullptr;
PFNGLDELETEPROGRAMPROC            glDeleteProgram            = nullptr;
PFNGLATTACHSHADERPROC             glAttachShader             = nullptr;
PFNGLLINKPROGRAMPROC              glLinkProgram              = nullptr;
PFNGLUSEPROGRAMPROC               glUseProgram               = nullptr;
PFNGLGETPROGRAMIVPROC             glGetProgramiv             = nullptr;
PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog        = nullptr;

PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = nullptr;
PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation       = nullptr;
PFNGLUNIFORM1IPROC               glUniform1i               = nullptr;
PFNGLUNIFORM1FPROC               glUniform1f               = nullptr;
PFNGLUNIFORM2FPROC               glUniform2f               = nullptr;

PFNGLGENBUFFERSPROC               glGenBuffers               = nullptr;
PFNGLDELETEBUFFERSPROC            glDeleteBuffers            = nullptr;
PFNGLBINDBUFFERPROC               glBindBuffer               = nullptr;
PFNGLBUFFERDATAPROC               glBufferData               = nullptr;
PFNGLBUFFERSUBDATAPROC            glBufferSubData            = nullptr;

PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer     = nullptr;

PFNGLGENVERTEXARRAYSPROC          glGenVertexArrays          = nullptr;
PFNGLDELETEVERTEXARRAYSPROC       glDeleteVertexArrays       = nullptr;
PFNGLBINDVERTEXARRAYPROC          glBindVertexArray          = nullptr;

PFNGLGENFRAMEBUFFERSPROC          glGenFramebuffers          = nullptr;
PFNGLDELETEFRAMEBUFFERSPROC       glDeleteFramebuffers       = nullptr;
PFNGLBINDFRAMEBUFFERPROC          glBindFramebuffer          = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC     glFramebufferTexture2D     = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC   glCheckFramebufferStatus   = nullptr;

// ---- Loader ----

#define LOAD(type, name) \
    name = (type)glfwGetProcAddress(#name); \
    if (!name) { fprintf(stderr, "Failed to load: %s\n", #name); ok = false; }

bool loadGL() {
    bool ok = true;

    LOAD(PFNGLACTIVETEXTUREPROC,            glActiveTexture);

    LOAD(PFNGLCREATESHADERPROC,             glCreateShader);
    LOAD(PFNGLDELETESHADERPROC,             glDeleteShader);
    LOAD(PFNGLSHADERSOURCEPROC,             glShaderSource);
    LOAD(PFNGLCOMPILESHADERPROC,            glCompileShader);
    LOAD(PFNGLGETSHADERIVPROC,              glGetShaderiv);
    LOAD(PFNGLGETSHADERINFOLOGPROC,         glGetShaderInfoLog);

    LOAD(PFNGLCREATEPROGRAMPROC,            glCreateProgram);
    LOAD(PFNGLDELETEPROGRAMPROC,            glDeleteProgram);
    LOAD(PFNGLATTACHSHADERPROC,             glAttachShader);
    LOAD(PFNGLLINKPROGRAMPROC,              glLinkProgram);
    LOAD(PFNGLUSEPROGRAMPROC,               glUseProgram);
    LOAD(PFNGLGETPROGRAMIVPROC,             glGetProgramiv);
    LOAD(PFNGLGETPROGRAMINFOLOGPROC,        glGetProgramInfoLog);

    LOAD(PFNGLGETATTRIBLOCATIONPROC,        glGetAttribLocation);
    LOAD(PFNGLGETUNIFORMLOCATIONPROC,       glGetUniformLocation);
    LOAD(PFNGLUNIFORM1IPROC,               glUniform1i);
    LOAD(PFNGLUNIFORM1FPROC,               glUniform1f);
    LOAD(PFNGLUNIFORM2FPROC,               glUniform2f);

    LOAD(PFNGLGENBUFFERSPROC,               glGenBuffers);
    LOAD(PFNGLDELETEBUFFERSPROC,            glDeleteBuffers);
    LOAD(PFNGLBINDBUFFERPROC,               glBindBuffer);
    LOAD(PFNGLBUFFERDATAPROC,               glBufferData);
    LOAD(PFNGLBUFFERSUBDATAPROC,            glBufferSubData);

    LOAD(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray);
    LOAD(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
    LOAD(PFNGLVERTEXATTRIBPOINTERPROC,     glVertexAttribPointer);

    LOAD(PFNGLGENVERTEXARRAYSPROC,          glGenVertexArrays);
    LOAD(PFNGLDELETEVERTEXARRAYSPROC,       glDeleteVertexArrays);
    LOAD(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray);

    LOAD(PFNGLGENFRAMEBUFFERSPROC,          glGenFramebuffers);
    LOAD(PFNGLDELETEFRAMEBUFFERSPROC,       glDeleteFramebuffers);
    LOAD(PFNGLBINDFRAMEBUFFERPROC,          glBindFramebuffer);
    LOAD(PFNGLFRAMEBUFFERTEXTURE2DPROC,     glFramebufferTexture2D);
    LOAD(PFNGLCHECKFRAMEBUFFERSTATUSPROC,   glCheckFramebufferStatus);

    return ok;
}

#undef LOAD
