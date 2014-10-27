/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_OPENGL
#define H_VO_OPENGL

// Internal
#include "mj_config.h"

// SDL
#include <SDL.h>

// OpenGL
#include <GL/glew.h>
#include <GL/gl.h>

// Debug macros
#ifndef NDEBUG
#define dbg_gl_check() {\
    GLenum err = glGetError();\
    if (err != GL_NO_ERROR)\
        std::cout << boost::format("OpenGL: Error %1%!") % g_gl_error(err) << std::endl;\
}
#else
#define dbg_gl_check() ((void)0)
#endif

// Helper functions
inline std::string g_gl_error(GLenum err)
{
    switch (err) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_TABLE_TOO_LARGE: return "GL_TABLE_TOO_LARGE";
        default: return "GL_UNKNOWN";
    }
}

#endif
