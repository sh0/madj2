/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Int inc
#include "mj_config.h"
#include "vo_shader.h"

// C++
#include <fstream>

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// Typemap
const std::map<e_shader_type, GLenum> c_shader_object::m_typemap = {
    { e_shader_type::vertex, GL_VERTEX_SHADER },
    { e_shader_type::tess_control, GL_TESS_CONTROL_SHADER },
    { e_shader_type::tess_evaluation, GL_TESS_EVALUATION_SHADER },
    { e_shader_type::geometry, GL_GEOMETRY_SHADER },
    { e_shader_type::fragment, GL_FRAGMENT_SHADER }
};

// Constructor and destructor
c_shader_object::c_shader_object(std::string name, e_shader_type type, std::string fn) :
    m_name(name), m_type(type)
{
    // Shader source
    std::ifstream fs(fn);
    if (!fs.is_open())
        throw c_exception("Shader: Failed to open file!", { throw_format("name", m_name), throw_format("path", fn) });
    std::string src((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    fs.close();

    // Type
    auto type_it = m_typemap.find(m_type);
    assert(type_it != m_typemap.end());

    // Create shader
    m_shader = glCreateShader(type_it->second);
    if (!m_shader)
        throw c_exception("Shader: Failed to create shader!", { throw_format("name", m_name), throw_format("path", fn) });

    // Shader compilation
    const GLchar* src_c = src.c_str();
    glShaderSource(m_shader, 1, &src_c, NULL);
    glCompileShader(m_shader);
    GLint ret = GL_FALSE;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &ret);
    if (ret != GL_TRUE) {
        std::cout << boost::format("Shader: Compiling error! name=%s") % m_name << std::endl;
        gl_info_shader(m_shader);
        throw c_exception("Shader: Failed to compile shader object!", { throw_format("name", m_name), throw_format("path", fn) });
    }
}

c_shader_object::~c_shader_object()
{
    glDeleteShader(m_shader);
}

// Info log
void c_shader_object::gl_info_shader(GLuint obj)
{
    GLint len = 0;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        GLchar* log = new GLchar[len];
        GLsizei nread = 0;
        glGetShaderInfoLog(obj, len, &nread, log);
        if (nread > 0) {
            std::string line(log);
            std::vector<std::string> split;
            boost::split(split, line, boost::is_any_of("\n"));
            for (std::string& str : split) {
                if (!str.empty())
                    std::cout << "Shader: " << str << std::endl;
            }
        }
        delete[] log;
    }
}

// Constructor and destructor
c_shader_program::c_shader_program(std::string name, std::vector<std::shared_ptr<c_shader_object>> shader) :
    m_name(name)
{
    // Create program
    m_program = glCreateProgram();
    if (!m_program)
        throw c_exception("Shader: Failed to create program!", { throw_format("name", m_name) });

    // Attach shaders
    for (auto& s : shader)
        glAttachShader(m_program, s->object());

    // Program linking
    glLinkProgram(m_program);
    GLint ret = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &ret);
    if (ret != GL_TRUE) {
        std::cout << boost::format("Shader: Linking error! name=%s") % m_name << std::endl;
        gl_info_program(m_program);
        throw c_exception("Shader: Failed to link shader program!", { throw_format("name", m_name) });
    }

    // Detach shaders
    for (auto& s : shader)
        glDetachShader(m_program, s->object());

    // Get uniform list
    int uni_active = 0;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uni_active);
    int uni_maxlen = 0;
    glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uni_maxlen);
    uni_maxlen++;

    // Parse uniforms
    for (int i = 0; i < uni_active; i++) {
        // Parameters
        GLint uni_size = 0;
        GLenum uni_type;
        GLsizei uni_len = 0;
        std::shared_ptr<GLchar> uni_str(new GLchar[uni_maxlen], [](GLchar* str){ delete[] str; });
        *uni_str = '\0';

        // Get general info
        glGetActiveUniform(
            m_program, i,
            uni_maxlen, &uni_len,
            &uni_size,
            &uni_type,
            uni_str.get()
        );
        if (glGetError() != GL_NO_ERROR) {
            throw c_exception("Shader: Failed to get uniform info!", { throw_format("name", m_name), throw_format("id", i) });
        } else if (uni_len > 0) {
            // Get address
            //std::cout << boost::format("Shader (%s): Uniform: name=%2%, size=%3%") % m_name % uni_str.get() % uni_size << std::endl;
            GLint uni_addr = glGetUniformLocation(m_program, uni_str.get());
            if (uni_addr != -1) {
                m_uniform.insert({
                    uni_str.get(),
                    c_uniform{
                        std::string(uni_str.get()), uni_size,
                        uni_type, uni_addr
                    }
                });
            }
        }
    }


    // Get uniform list
    int attr_active = 0;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &attr_active);
    int attr_maxlen = 0;
    glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attr_maxlen);
    attr_maxlen++;

    // Parse attributes
    for (int i = 0; i < attr_active; i++) {
        // Parameters
        GLint attr_size = 0;
        GLenum attr_type;
        GLsizei attr_len = 0;
        std::shared_ptr<GLchar> attr_str(new GLchar[attr_maxlen], [](GLchar* str){ delete[] str; });
        *attr_str = '\0';

        // Get general info
        glGetActiveAttrib(
            m_program, i,
            attr_maxlen, &attr_len,
            &attr_size,
            &attr_type,
            attr_str.get()
        );
        if (glGetError() != GL_NO_ERROR) {
            throw c_exception("Shader: Failed to get attribute info!", { throw_format("name", m_name), throw_format("id", i) });
        } else if (attr_len > 0) {
            m_attribute.insert({ attr_str.get(), i });
        }
    }

    // Debug
    std::cout <<
        boost::format("Shader (%1%): uniforms=%2%, attributes=%3%") %
        m_name % uni_active % attr_active << std::endl;
}

c_shader_program::~c_shader_program()
{
    // Clear uniforms and attributes
    m_uniform.clear();
    m_attribute.clear();

    // Program
    glDeleteProgram(m_program);
}

// Program use
void c_shader_program::use_begin()
{
    // Consistency check
    #ifndef NDEBUG
        GLint prog_cur = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog_cur);
        assert(prog_cur == 0);
    #endif

    // Enable shader program
    glUseProgram(m_program);
}

void c_shader_program::use_end()
{
    // Consistency check
    #ifndef NDEBUG
        GLuint prog_cur = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&prog_cur));
        assert(prog_cur == m_program);
    #endif

    // Disable shader program
    glUseProgram(0);
}

// Uniforms
c_uniform* c_shader_program::uniform(std::string name)
{
    // Find
    auto it = m_uniform.find(name);
    if (it == m_uniform.end())
        return nullptr;
    return &(it->second);
}

// Attributes
GLint c_shader_program::attribute(std::string name)
{
    // Find
    auto it = m_attribute.find(name);
    if (it == m_attribute.end())
        return -1;
    return it->second;
}

// Info log
void c_shader_program::gl_info_program(GLuint obj)
{
    GLint len = 0;
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        GLchar* log = new GLchar[len];
        GLsizei nread = 0;
        glGetProgramInfoLog(obj, len, &nread, log);
        if (nread > 0) {
            std::string line(log);
            std::vector<std::string> split;
            boost::split(split, line, boost::is_any_of("\n"));
            for (std::string& str : split) {
                if (!str.empty())
                    std::cout << "Shader: " << str << std::endl;
            }
        }
        delete[] log;
    }
}

// Constructor
c_shader::c_shader()
{
    // Path
    boost::filesystem::path path(MJ_DATA_PATH);
    path /= "shader";
    if (!boost::filesystem::is_directory(path))
        throw c_exception("Shader: Data folder does not exist!", { throw_format("path", path.native()) });

    // Iterate files
    for (auto it = boost::filesystem::directory_iterator(path); it != boost::filesystem::directory_iterator(); ++it) {
        // Check type
        auto fn = it->path();
        if (fn.extension() != ".glsl")
            continue;

        // Load
        std::string name = fn.stem().native();
        if (boost::starts_with(name, "vertex-")) {
            m_objects[name] = std::make_shared<c_shader_object>(name, e_shader_type::vertex, fn.native());
        } else if (boost::starts_with(name, "tess_control-"))
            m_objects[name] = std::make_shared<c_shader_object>(name, e_shader_type::tess_control, fn.native());
        else if (boost::starts_with(name, "tess_evaluation-"))
            m_objects[name] = std::make_shared<c_shader_object>(name, e_shader_type::tess_evaluation, fn.native());
        else if (boost::starts_with(name, "geometry-"))
            m_objects[name] = std::make_shared<c_shader_object>(name, e_shader_type::geometry, fn.native());
        else if (boost::starts_with(name, "fragment-"))
            m_objects[name] = std::make_shared<c_shader_object>(name, e_shader_type::fragment, fn.native());
        else
            std::cout << "Shader: Unknown type of shader! name=" << name << ", path=" << fn << std::endl;
    }

    // Program list
    path /= "program.list";
    if (!boost::filesystem::exists(path))
        throw c_exception("Shader: Program list file does not exist!", { "path", path.native() });
    std::ifstream program_fs(path.native());
    if (!program_fs.is_open())
        throw c_exception("Shader: Failed to open program list file!", { "path", path.native() });
    for (std::string line; std::getline(program_fs, line);) {
        // Trim
        boost::trim(line);

        // Check for comments
        if (boost::starts_with(line, "#"))
            continue;

        // Header
        std::vector<std::string> header;
        boost::split(header, line, boost::is_any_of(":"));
        if (header.size() < 2)
            continue;
        std::string name = header[0];
        boost::trim(name);

        // Objects
        std::vector<std::string> objects_str;
        boost::split(objects_str, header[1], boost::is_any_of(","));
        if (objects_str.size() < 2)
            continue;
        for (auto& str : objects_str)
            boost::trim(str);
        std::vector<std::shared_ptr<c_shader_object>> objects_obj;
        for (auto& str : objects_str) {
            auto it = m_objects.find(str);
            if (it != m_objects.end())
                objects_obj.push_back(it->second);
            else
                std::cout << boost::format("Shader: Could not find object! object=%s, program=%s") % str % name << std::endl;
        }

        // Program
        m_programs[name] = std::make_shared<c_shader_program>(name, objects_obj);
    }
}

c_shader::~c_shader()
{
    // Programs
    m_programs.clear();

    // Objects
    m_objects.clear();
}
