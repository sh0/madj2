/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_SHADER
#define H_VO_SHADER

// Int inc
#include "mj_config.h"
#include "vo_opengl.h"
#include "vo_uniform.h"

// C++
#include <map>

// Shader types
enum class e_shader_type {
    vertex,
    tess_control,
    tess_evaluation,
    geometry,
    fragment
};

// Shader object class
class c_shader_object : c_noncopiable
{
    public:
        // Constructor and destructor
        c_shader_object(std::string name, e_shader_type type, std::string fn);
        ~c_shader_object();

        // Info
        const std::string& name() { return m_name; }
        e_shader_type type() { return m_type; }

        // Object
        GLuint object() { return m_shader; }

    private:
        // Info
        std::string m_name;
        e_shader_type m_type;

        // Shader
        GLuint m_shader;

        // Info log
        void gl_info_shader(GLuint obj);

        // Typemap
        static const std::map<e_shader_type, GLenum> m_typemap;
};

// Shader program class
class c_shader_program : c_noncopiable
{
    public:
        // Constructor and destructor
        c_shader_program(std::string name, std::vector<std::shared_ptr<c_shader_object>> shader);
        ~c_shader_program();

        // Info
        const std::string& name() { return m_name; }

        // Program use
        void use_begin();
        void use_end();

        // Uniforms
        c_uniform* uniform(std::string name);

        // Attributes
        GLint attribute(std::string name);

    private:
        // Info
        std::string m_name;

        // Program
        GLuint m_program;

        // Uniforms and attributes
        std::map<std::string, c_uniform> m_uniform;
        std::map<std::string, GLint> m_attribute;

        // Info log
        void gl_info_program(GLuint obj);
};

// Shader class
class c_shader : c_noncopiable
{
    public:
        // Constructor and destructor
        c_shader();
        ~c_shader();

        // Programs
        std::shared_ptr<c_shader_program> program(std::string name) {
            auto program = m_programs.find(name);
            if (program == m_programs.end())
                return nullptr;
            return program->second;
        }

    private:
        // Objects and programs
        std::map<std::string, std::shared_ptr<c_shader_object>> m_objects;
        std::map<std::string, std::shared_ptr<c_shader_program>> m_programs;
};

#endif
