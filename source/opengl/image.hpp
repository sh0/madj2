/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_IMAGE
#define H_OPENGL_IMAGE

// Internal
#include "opengl/opengl.hpp"

// C++
#include <memory>
#include <string>

// Boost
#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

// Image object class
class c_opengl_image : boost::noncopyable
{
    public:
        // Image type
        enum class e_type {
            rgb24,
            bgr24,
            rgba32,
            bgra32
        };

        // Constructors
        c_opengl_image(e_type type, int width, int height, float aspect);
        c_opengl_image(e_type type, int width, int height, float aspect, int stride);
        c_opengl_image(e_type type, int width, int height, float aspect, std::unique_ptr<uint8_t[]>&& data);

        // Parameters
        int width() { return m_width; }
        int height() { return m_height; }
        float aspect() { return m_aspect; }

        // Data
        uint stride() { return m_stride; }
        uint8_t* data() { return m_data.get(); }

        // GL
        GLenum gl_format() { return m_gl_format; }
        GLenum gl_type() { return m_gl_type; }

    private:
        // Initialize
        void init();

        // Parameters
        e_type m_type;
        int m_width;
        int m_height;
        float m_aspect;

        // Data
        uint m_stride;
        std::unique_ptr<uint8_t[]> m_data;

        // OpenGL
        GLenum m_gl_format;
        GLenum m_gl_type;
};

#endif
