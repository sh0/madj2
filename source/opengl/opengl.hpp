/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_OPENGL
#define H_OPENGL_OPENGL

// Internal
#include "cmake.hpp"

// C++
#include <string>
#include <iostream>

// Boost
#include <boost/format.hpp>

// SDL
#include <SDL.h>

// OpenGL
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Helper functions
inline std::string g_opengl_error(GLenum err)
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

#define g_opengl_check() g_opengl_check_int(__FILENAME__, __func__, __LINE__)
inline void g_opengl_check_int(std::string dbg_file, std::string dbg_func, int dbg_line)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        std::cout << boost::format("OpenGL: %s! (%s:%s:%d)") % g_opengl_error(err) % dbg_file % dbg_func % dbg_line << std::endl;
}

#endif
