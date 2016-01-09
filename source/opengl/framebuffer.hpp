/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_FRAMEBUFFER
#define H_OPENGL_FRAMEBUFFER

// Internal
#include "opengl/opengl.hpp"
#include "opengl/texture.hpp"

// C++
#include <stdexcept>
#include <memory>

// Boost
#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

// Framebuffer
class c_opengl_framebuffer : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_opengl_framebuffer(
            int width, int height,
            c_opengl_texture_2d::e_format format_color = c_opengl_texture_2d::e_format::rgb24,
            c_opengl_texture_2d::e_format format_depth = c_opengl_texture_2d::e_format::depth32f
        );
        ~c_opengl_framebuffer();

        // Binding
        void bind(int width = -1, int height = -1);
        void unbind();

        // Textures
        std::shared_ptr<c_opengl_texture_2d> color() { return m_color; };
        std::shared_ptr<c_opengl_texture_2d> depth() { return m_depth; };

    private:
        // Framebuffer
        GLuint m_object;

        // Properties
        int m_width;
        int m_height;

        // Binding
        bool m_active;

        // Textures
        std::shared_ptr<c_opengl_texture_2d> m_color;
        std::shared_ptr<c_opengl_texture_2d> m_depth;
};

#endif
