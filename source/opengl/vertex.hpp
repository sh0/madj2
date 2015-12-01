/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_VERTEX
#define H_OPENGL_VERTEX

// Internal
#include "opengl/opengl.hpp"

// C++
#include <stdexcept>

// Boost
#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

// Vertex class
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

#endif
