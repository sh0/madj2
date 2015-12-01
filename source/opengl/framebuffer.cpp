/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/framebuffer.hpp"

// Constructor and destructor
c_opengl_framebuffer::c_opengl_framebuffer(
    int width, int height,
    c_opengl_texture_2d::e_format format_color,
    c_opengl_texture_2d::e_format format_depth
) :
    m_object(0), m_width(width), m_height(height), m_active(false)
{
    // Framebuffer
    glGenFramebuffers(1, &m_object);
    glBindFramebuffer(GL_FRAMEBUFFER, m_object);

    // Textures
    m_color = std::make_shared<c_opengl_texture_2d>(format_color);
    m_color->upload(m_width, m_height);
    m_depth = std::make_shared<c_opengl_texture_2d>(format_depth);
    m_depth->upload(m_width, m_height);

    // Attach
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color->object(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->object(), 0);

    // Check
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to complete framebuffer!");

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Setup
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glShadeModel(GL_SMOOTH);

    //glEnable(GL_DEPTH_CLAMP);
    //glDepthRange(-1.0f, 1.0f);
    //glDepthRange(0.1f, 100.0f);
    // f - n = 2 -> f = 2 + n
    // f + n = 0 -> 2 + 2n = 0 -> n = -1, f = 1
    //glDepthRange(0, 10000.0);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
    //glEnable(GL_CULL_FACE);

    //glEnable(GL_MULTISAMPLE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_opengl_check();
}

c_opengl_framebuffer::~c_opengl_framebuffer()
{
    // Check
    BOOST_ASSERT(!m_active);

    // Textures
    m_color.reset();
    m_depth.reset();

    // Framebuffer
    glDeleteFramebuffers(1, &m_object);
}

// Binding
void c_opengl_framebuffer::bind(int width, int height)
{
    // Check
    BOOST_ASSERT(!m_active);

    // Check size
    if (width > 0 && height > 0 && width != m_width && height != m_height) {
        m_width = width;
        m_height = height;
        m_color->upload(m_width, m_height);
        m_depth->upload(m_width, m_height);
    }

    // Bind
    m_active = true;
    glBindFramebuffer(GL_FRAMEBUFFER, m_object);
    g_opengl_check();

    // Setup
    /*
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(100000.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    */

    // Clear screen
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(100000.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    g_opengl_check();
}

void c_opengl_framebuffer::unbind()
{
    // Check
    BOOST_ASSERT(m_active);

    // Unbind
    m_active = false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    g_opengl_check();
}
