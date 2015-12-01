/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_SHADER
#define H_OPENGL_SHADER

// Internal
#include "opengl/opengl.hpp"
#include "opengl/uniform.hpp"

// C++
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

// Boost
#include <boost/noncopyable.hpp>
#include <boost/format.hpp>

// Shader types
enum class e_opengl_shader_type {
    vertex,
    tess_control,
    tess_evaluation,
    geometry,
    fragment
};

// Shader object class
class c_opengl_shader_object : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_opengl_shader_object(std::string name, e_opengl_shader_type type, std::string fn);
        ~c_opengl_shader_object();

        // Info
        const std::string& name() { return m_name; }
        e_opengl_shader_type type() { return m_type; }

        // Object
        GLuint object() { return m_shader; }

    private:
        // Info
        std::string m_name;
        e_opengl_shader_type m_type;

        // Shader
        GLuint m_shader;

        // Info log
        void gl_info_shader(GLuint obj);

        // Typemap
        static const std::map<e_opengl_shader_type, GLenum> m_typemap;
};

// Shader program class
class c_opengl_shader_program : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_opengl_shader_program(std::string name, std::vector<std::shared_ptr<c_opengl_shader_object>> shader);
        ~c_opengl_shader_program();

        // Info
        const std::string& name() { return m_name; }

        // Program use
        void use_begin();
        void use_end();

        // Uniforms
        c_opengl_uniform* uniform(std::string name);

        // Attributes
        GLint attribute(std::string name);

    private:
        // Info
        std::string m_name;

        // Program
        GLuint m_program;

        // Uniforms and attributes
        std::map<std::string, c_opengl_uniform> m_uniform;
        std::map<std::string, GLint> m_attribute;

        // Info log
        void gl_info_program(GLuint obj);
};

// Shader class
class c_opengl_shader : boost::noncopyable
{
    public:
        // Constructor
        c_opengl_shader();

        // Programs
        std::shared_ptr<c_opengl_shader_program> program(std::string name);

    private:
        // Program objects
        std::map<std::string, std::vector<std::string>> m_programs;
};

#endif
