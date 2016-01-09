/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_BUFFER
#define H_OPENGL_BUFFER

// Internal
#include "opengl/opengl.hpp"

// C++
#include <map>
#include <stdexcept>

// Boost
#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

// Usage enum
enum class e_opengl_buffer_usage {
    stream_draw,
    stream_read,
    stream_copy,
    static_draw,
    static_read,
    static_copy,
    dynamic_draw,
    dynamic_read,
    dynamic_copy
};

// Mode enum
enum class e_opengl_buffer_mode {
    write_only,
    read_only,
    read_write
};

// Typemaps
extern const std::map<e_opengl_buffer_usage, GLenum> g_opengl_buffer_usage_map;
extern const std::map<e_opengl_buffer_mode, GLenum> g_opengl_buffer_mode_map;

// Buffer object template
template <GLenum t_type_target, GLenum t_type_get>
class c_opengl_buffer : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_opengl_buffer() :
            m_object(0), m_active(false)
        {
            // Generate
            glGenBuffers(1, &m_object);
            if (!m_object) {
                g_opengl_check();
                throw std::runtime_error("Vertex buffer: Failed to generate buffer!");
            }

            // Temporary bind
            glBindBuffer(t_type_target, m_object);
            glBindBuffer(t_type_target, 0);
            g_opengl_check();

            // Extra check
            BOOST_ASSERT(glIsBuffer(m_object) == GL_TRUE);
        }

        ~c_opengl_buffer()
        {
            // Delete
            glDeleteBuffers(1, &m_object);
        }

        // Type
        GLenum type_target() { return t_type_target; }
        GLenum type_get() { return t_type_get; }

        // Object
        GLuint object() { return m_object; }

        // Binding
        void bind()
        {
            // Check integrity
            BOOST_ASSERT(!m_active);
            #ifndef NDEBUG
                GLuint object = 0;
                glGetIntegerv(t_type_get, reinterpret_cast<GLint*>(&object));
                BOOST_ASSERT(!object);
            #endif

            // Bind
            glBindBuffer(t_type_target, m_object);
            m_active = true;
        }

        void unbind()
        {
            // Check integrity
            BOOST_ASSERT(m_active);
            /*
            #ifndef NDEBUG
                GLuint object = 0;
                glGetIntegerv(t_type_get, reinterpret_cast<GLint*>(&object));
                BOOST_ASSERT(object == m_object);
            #endif
            */

            // Unbind
            glBindBuffer(t_type_target, 0);
            m_active = false;
        }

        // Upload
        void upload(uint8_t* data, uint32_t size, const e_opengl_buffer_usage usage)
        {
            // Bind
            bool is_bound = m_active;
            if (!is_bound)
                bind();

            // Upload
            glBufferData(t_type_target, size, data, g_opengl_buffer_usage_map.at(usage));

            // Unbind
            if (!is_bound)
                unbind();

            // Debug
            g_opengl_check();
        }

    private:
        // Object
        GLuint m_object;

        // Bind
        bool m_active;
};

// Typedefs
#ifdef GL_ARRAY_BUFFER_BINDING
    typedef c_opengl_buffer<GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING> c_opengl_buffer_array;
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER_BINDING
    typedef c_opengl_buffer<GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING> c_opengl_buffer_element_array;
#endif
#ifdef GL_COPY_READ_BUFFER_BINDING
    typedef c_opengl_buffer<GL_COPY_READ_BUFFER, GL_COPY_READ_BUFFER_BINDING> c_opengl_buffer_copy_read;
#endif
#ifdef GL_COPY_WRITE_BUFFER_BINDING
    typedef c_opengl_buffer<GL_COPY_WRITE_BUFFER, GL_COPY_WRITE_BUFFER_BINDING> c_opengl_buffer_copy_write;
#endif
#ifdef GL_PIXEL_PACK_BUFFER_BINDING
    typedef c_opengl_buffer<GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER_BINDING> c_opengl_buffer_pixel_pack;
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER_BINDING
    typedef c_opengl_buffer<GL_PIXEL_UNPACK_BUFFER, GL_PIXEL_UNPACK_BUFFER_BINDING> c_opengl_buffer_pixel_unpack;
#endif
#ifdef GL_TEXTURE_BUFFER_BINDING
    typedef c_opengl_buffer<GL_TEXTURE_BUFFER, GL_TEXTURE_BUFFER_BINDING> c_opengl_buffer_texture;
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    typedef c_opengl_buffer<GL_TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING> c_opengl_buffer_transform_feedback;
#endif
#ifdef GL_UNIFORM_BUFFER_BINDING
    typedef c_opengl_buffer<GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING> c_opengl_buffer_uniform;
#endif
#if defined(GLEW_VERSION_4_0) || defined(GLEW_ARB_draw_indirect)
    typedef c_opengl_buffer<GL_DRAW_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER_BINDING> c_opengl_buffer_draw_indirect;
#endif
#if defined(GLEW_VERSION_4_2) || defined(GLEW_ARB_shader_atomic_counters)
    typedef c_opengl_buffer<GL_ATOMIC_COUNTER_BUFFER, GL_ATOMIC_COUNTER_BUFFER_BINDING> c_opengl_buffer_atomic_counter;
#endif
#if defined(GLEW_VERSION_4_3) || defined(GLEW_ARB_shader_storage_buffer_object)
    typedef c_opengl_buffer<GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING> c_opengl_buffer_shader_storage;
#endif

#endif
