/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_FRAMING
#define H_OPENGL_FRAMING

// Internal
#include "opengl/opengl.hpp"
#include "opengl/texture.hpp"

// C++
#include <array>

// Framing functions
class c_opengl_framing : boost::noncopyable
{
    public:
        // Box frames (x1, y1, x2, y2)
        static std::array<float, 4> zoombox_quad(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst);
        static std::array<float, 4> letterbox_quad(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst);
};

#endif
