/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_TEXTURE
#define H_OPENGL_TEXTURE

// Internal
#include "opengl/opengl.hpp"
#include "opengl/image.hpp"

// C++
#include <memory>
#include <stdexcept>

// Boost
#include <boost/noncopyable.hpp>

// 2D texture class
class c_opengl_texture_2d : boost::noncopyable
{
    public:
        // Internal format representation
        enum class e_format {
            r8,
            rgb24,
            rgba32,
            depth32f
        };

        // Interpolation enum
        enum class e_filter {
            nearest,
            linear
        };

        // Constructor and desturctor
        c_opengl_texture_2d(e_format format = e_format::rgb24, e_filter filter = e_filter::linear);
        ~c_opengl_texture_2d();

        // Binding
        void bind(uint32_t id);
        void unbind();

        // Type
        static GLenum type_target() { return GL_TEXTURE_2D; }
        static GLenum type_get() { return GL_TEXTURE_BINDING_2D; }

        // Object
        GLuint object() { return m_object; }

        // Properties
        int width() { return m_width; }
        int height() { return m_height; }

        // Upload
        void upload(uint32_t width, uint32_t height, bool clear_texture = false);
        void upload(std::shared_ptr<c_opengl_image> image);
        void upload(
            uint32_t width, uint32_t height, uint32_t stride,
            GLenum gl_format, GLenum gl_type,
            uint8_t* data
        );
        void upload_sub(
            uint32_t offset_x, uint32_t offset_y,
            uint32_t width, uint32_t height, uint32_t stride,
            GLenum gl_format, GLenum gl_type,
            uint8_t* data
        );

    private:
        // Object
        GLuint m_object;

        // Bind
        bool m_bind_active;
        GLenum m_bind_id;

        // Properties
        uint32_t m_width;
        uint32_t m_height;
        GLenum m_format;
};

#endif
