/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "vo_image.h"

// Initialize
void c_image::init()
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
            assert(false);
    }

    // Allocate data if necessary
    if (!m_data)
        m_data = std::unique_ptr<uint8_t[]>(new uint8_t[m_stride * m_height]);
}
