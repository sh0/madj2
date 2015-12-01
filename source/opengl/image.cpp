/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/image.hpp"

// Boost
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

// Constructors
c_opengl_image::c_opengl_image(e_type type, int width, int height) :
    m_type(type), m_width(width), m_height(height)
{
    // Initialize
    init();
}

c_opengl_image::c_opengl_image(e_type type, int width, int height, std::unique_ptr<uint8_t[]>&& data) :
    m_type(type), m_width(width), m_height(height), m_data(std::move(data))
{
    // Initialize
    init();
}

// Initialize
void c_opengl_image::init()
{
    // Select image format
    switch (m_type) {
        case e_type::rgb24:
            m_stride = m_width * 3;
            m_gl_format = GL_RGB;
            m_gl_type = GL_UNSIGNED_BYTE;
            break;

        case e_type::bgr24:
            m_stride = m_width * 3;
            m_gl_format = GL_BGR;
            m_gl_type = GL_UNSIGNED_BYTE;
            break;

        case e_type::rgba32:
            m_stride = m_width * 4;
            m_gl_format = GL_RGBA;
            m_gl_type = GL_UNSIGNED_BYTE;
            break;

        case e_type::bgra32:
            m_stride = m_width * 4;
            m_gl_format = GL_BGRA;
            m_gl_type = GL_UNSIGNED_BYTE;
            break;

        default:
            BOOST_ASSERT(false);
    }

    // Allocate data if necessary
    if (!m_data)
        m_data = std::unique_ptr<uint8_t[]>(new uint8_t[m_stride * m_height]);
}
