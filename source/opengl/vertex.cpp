/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/vertex.hpp"

// Constructor and destructor
c_opengl_vertex::c_opengl_vertex() :
    m_object(0), m_active(false)
{
    // Generate
    glGenVertexArrays(1, &m_object);
    if (!m_object)
        throw std::runtime_error("Failed to generate new VAO!");

    // Temporary bind
    glBindVertexArray(m_object);
    glBindVertexArray(0);
    g_opengl_check();

    // Extra check
    BOOST_ASSERT(glIsVertexArray(m_object) == GL_TRUE);
}

c_opengl_vertex::~c_opengl_vertex()
{
    // Delete
    glDeleteVertexArrays(1, &m_object);
}

// Binding
void c_opengl_vertex::bind()
{
    // Check integrity
    BOOST_ASSERT(!m_active);
    #ifndef NDEBUG
        GLuint object = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&object));
        BOOST_ASSERT(!object);
    #endif

    // Bind
    glBindVertexArray(m_object);
    m_active = true;
}

void c_opengl_vertex::unbind()
{
    // Check integrity
    BOOST_ASSERT(m_active);
    /*
    #ifndef NDEBUG
        GLuint object = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&object));
        BOOST_ASSERT(object == m_object);
    #endif
    */

    // Unbind
    glBindVertexArray(0);
    m_active = false;
}

// Upload
void c_opengl_vertex_rectangle::upload(GLint index, float x1, float y1, float x2, float y2)
{
    // Buffer
    std::array<float, 2 * 4> data = {{
        x1, y1,
        x2, y1,
        x2, y2,
        x1, y2
    }};
    m_buffer.upload(data.data(), data.size() * sizeof(float), e_opengl_buffer_usage::dynamic_draw);

    // Array
    m_vertex.bind();
    m_buffer.bind();
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(index),
    m_buffer.unbind();
    m_vertex.unbind();
}

// Draw
void c_opengl_vertex_rectangle::draw()
{
    m_vertex.bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    m_vertex.unbind();
}
