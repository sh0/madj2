/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "vo_texture.h"

// Constructor and destructor
c_texture_2d::c_texture_2d(e_format format, e_filter filter) :
    // Object
    m_object(0),
    // Bind
    m_bind_active(false), m_bind_id(0),
    // Parameters
    m_width(0), m_height(0)
{
    // Format
    switch (format) {
        case e_format::r8: m_format = GL_R8; break;
        case e_format::rgb24: m_format = GL_RGB8; break;
        case e_format::rgba32: m_format = GL_RGBA8; break;
        default: assert(false);
    }

    // Generate
    glGenTextures(1, &m_object);
    if (!m_object)
        throw c_exception("Texture: Failed to generate texture!");

    // Bind
    glBindTexture(GL_TEXTURE_2D, m_object);

    // Set allowed mipmap levels to only level 0
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Priority
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1.0);

    // Interpolation
    GLint filter_gl;
    switch (filter) {
        case e_filter::linear: filter_gl = GL_LINEAR; break;
        case e_filter::nearest: filter_gl = GL_NEAREST; break;
        default: assert(false);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_gl);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_gl);

    // Wrapping (GL_CLAMP_TO_BORDER or GL_CLAMP_TO_EDGE)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Border texels color
    GLfloat border[4] = { 0.0, 0.0, 0.0, 0.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

    // Fragment texturing mode (GL_MODULATE = frag * tex; GL_REPLACE = tex)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // OpenGL check
    dbg_gl_check();

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    // Extra check
    assert(glIsTexture(m_object) == GL_TRUE);
}

c_texture_2d::~c_texture_2d()
{
    // Delete
    glDeleteTextures(1, &m_object);
}

// Binding
void c_texture_2d::bind(uint32_t id)
{
    // Check
    assert(!m_bind_active);

    // Active texture
    assert(id < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    glActiveTexture(GL_TEXTURE0 + id);
    m_bind_id = id;

    // Check integrity by verifying that no previous texture is bound
    #ifndef NDEBUG
        GLuint obj = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&obj));
        assert(!obj);
    #endif

    // Bind
    glBindTexture(GL_TEXTURE_2D, m_object);
    m_bind_active = true;
}

void c_texture_2d::unbind()
{
    // Check
    assert(m_bind_active);

    // Active texture
    glActiveTexture(GL_TEXTURE0 + m_bind_id);

    // Check integrity
    #ifndef NDEBUG
        GLuint obj = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&obj));
        assert(obj == m_object);
    #endif

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    m_bind_active = false;
}

// Upload
void c_texture_2d::upload(uint32_t width, uint32_t height, bool clear_texture)
{
    // Check if already correct size
    if ((m_width == width) && (m_height == height))
        return;

    // Parameters
    m_width = width;
    m_height = height;

    // Bind
    bool was_bound = m_bind_active;
    if (!was_bound)
        bind(0);

    // Alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_width);

    // Temporary data
    std::shared_ptr<uint8_t> data;
    if (clear_texture) {
        uint32_t size = m_width * m_height * 3;
        data = std::shared_ptr<uint8_t>(new uint8_t[size], [](uint8_t* data){ delete[](data); });
        memset(data.get(), 0, size);
    }

    // Upload image
    glTexImage2D(
        GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        GL_RGB, GL_UNSIGNED_BYTE, data.get()
    );

    // OpenGL check
    dbg_gl_check();

    // Unbind
    if (!was_bound)
        unbind();
}

void c_texture_2d::upload(std::shared_ptr<c_image> image)
{
    // Parameters
    m_width = image->width();
    m_height = image->height();

    // Bind
    bool was_bound = m_bind_active;
    if (!was_bound)
        bind(0);

    // Alignment
    uint stride = 1;
    if ((image->stride() % 8) == 0)
        stride = 8;
    else if ((image->stride() % 4) == 0)
        stride = 4;
    else if ((image->stride() % 2) == 0)
        stride = 2;
    glPixelStorei(GL_UNPACK_ALIGNMENT, stride);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_width);

    // Upload image
    glTexImage2D(
        GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        image->gl_format(), image->gl_type(), image->data()
    );

    // OpenGL check
    dbg_gl_check();

    // Unbind
    if (!was_bound)
        unbind();
}

void c_texture_2d::upload(
    uint32_t width, uint32_t height, uint32_t stride,
    GLenum gl_format, GLenum gl_type,
    uint8_t* data
) {
    // Parameters
    m_width = width;
    m_height = height;

    // Bind
    bool was_bound = m_bind_active;
    if (!was_bound)
        bind(0);

    // Alignment
    uint32_t align = 1;
    if ((stride % 8) == 0)
        align = 8;
    else if ((stride % 4) == 0)
        align = 4;
    else if ((stride % 2) == 0)
        align = 2;
    glPixelStorei(GL_UNPACK_ALIGNMENT, align);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, m_width);

    // Upload image
    glTexImage2D(
        GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        gl_format, gl_type, data
    );

    // OpenGL check
    dbg_gl_check();

    // Unbind
    if (!was_bound)
        unbind();
}

void c_texture_2d::upload_sub(
    uint32_t offset_x, uint32_t offset_y,
    uint32_t width, uint32_t height, uint32_t stride,
    GLenum gl_format, GLenum gl_type,
    uint8_t* data
) {
    // Check bounds
    assert(offset_x + width <= m_width);
    assert(offset_y + height <= m_height);

    // Bind
    bool was_bound = m_bind_active;
    if (!was_bound)
        bind(0);

    // Alignment
    uint32_t align = 1;
    if ((stride % 8) == 0)
        align = 8;
    else if ((stride % 4) == 0)
        align = 4;
    else if ((stride % 2) == 0)
        align = 2;
    glPixelStorei(GL_UNPACK_ALIGNMENT, align);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

    // Upload image
    glTexSubImage2D(
        GL_TEXTURE_2D, 0,
        offset_x, offset_y,
        width, height,
        gl_format, gl_type, data
    );

    // OpenGL check
    dbg_gl_check();

    // Unbind
    if (!was_bound)
        unbind();
}
