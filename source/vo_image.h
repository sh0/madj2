/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_IMAGE
#define H_VO_IMAGE

// Internal
#include "mj_config.h"
#include "vo_opengl.h"

// Image object class
class c_image : c_noncopiable
{
    public:
        // Image type
        enum class e_type {
            rgb24,
            bgr24,
            rgba32,
            bgra32
        };

        // Constructor
        c_image(e_type type, int width, int height) :
            m_type(type), m_width(width), m_height(height)
        {
            init();
        }
        c_image(e_type type, int width, int height, std::unique_ptr<uint8_t[]>&& data) :
            m_type(type), m_width(width), m_height(height), m_data(std::move(data))
        {
            init();
        }

        // Parameters
        int width() { return m_width; }
        int height() { return m_height; }

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

        // Data
        uint m_stride;
        std::unique_ptr<uint8_t[]> m_data;

        // OpenGL
        GLenum m_gl_format;
        GLenum m_gl_type;
};

#endif
