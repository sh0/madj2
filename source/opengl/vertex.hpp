/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_VERTEX
#define H_OPENGL_VERTEX

// Internal
#include "opengl/opengl.hpp"
#include "opengl/buffer.hpp"

// C++
#include <array>

// Vertex array object class
class c_opengl_vertex : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_opengl_vertex();
        ~c_opengl_vertex();

        // Object
        GLuint object() { return m_object; }

        // Binding
        void bind();
        void unbind();

    private:
        // Object
        GLuint m_object;

        // Bind
        bool m_active;
};

// Rectangle VAO class
class c_opengl_vertex_rectangle : boost::noncopyable
{
    public:
        // Upload
        void upload(GLint index, float x1, float y1, float x2, float y2);

        // Draw
        void draw();

    private:
        c_opengl_vertex m_vertex;
        c_opengl_buffer_array m_buffer;
};

#endif
