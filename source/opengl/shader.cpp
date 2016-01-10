/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/shader.hpp"

// C++
#include <fstream>

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// Typemap
const std::map<e_opengl_shader_type, GLenum> c_opengl_shader_object::m_typemap = {
    { e_opengl_shader_type::vertex, GL_VERTEX_SHADER },
    { e_opengl_shader_type::tess_control, GL_TESS_CONTROL_SHADER },
    { e_opengl_shader_type::tess_evaluation, GL_TESS_EVALUATION_SHADER },
    { e_opengl_shader_type::geometry, GL_GEOMETRY_SHADER },
    { e_opengl_shader_type::fragment, GL_FRAGMENT_SHADER }
};

// Constructor and destructor
c_opengl_shader_object::c_opengl_shader_object(std::string name, e_opengl_shader_type type, std::string fn) :
    m_name(name), m_type(type)
{
    // Shader source
    std::ifstream fs(fn);
    if (!fs.is_open())
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to open file! name = \"%s\", path = \"%s\"") % m_name % fn));
    std::string src((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    fs.close();

    // Type
    auto type_it = m_typemap.find(m_type);
    assert(type_it != m_typemap.end());

    // Create shader
    m_shader = glCreateShader(type_it->second);
    if (!m_shader)
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to create shader! name = \"%s\", path = \"%s\"") % m_name % fn));

    // Shader compilation
    const GLchar* src_c = src.c_str();
    glShaderSource(m_shader, 1, &src_c, NULL);
    glCompileShader(m_shader);
    GLint ret = GL_FALSE;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &ret);
    if (ret != GL_TRUE) {
        std::cout << boost::format("Shader: Compiling error! name = \"%s\"") % m_name << std::endl;
        gl_info_shader(m_shader);
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to compile shader object! name = \"%s\", path = \"%s\"") % m_name % fn));
    }
}

c_opengl_shader_object::~c_opengl_shader_object()
{
    glDeleteShader(m_shader);
}

// Info log
void c_opengl_shader_object::gl_info_shader(GLuint obj)
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
c_opengl_shader_program::c_opengl_shader_program(std::string name, std::vector<std::shared_ptr<c_opengl_shader_object>> shader) :
    m_name(name)
{
    // Create program
    m_program = glCreateProgram();
    if (!m_program)
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to create program! name = \"%s\"") % m_name));

    // Attach shaders
    for (auto& s : shader)
        glAttachShader(m_program, s->object());

    // Program linking
    glLinkProgram(m_program);
    GLint ret = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &ret);
    if (ret != GL_TRUE) {
        std::cout << boost::format("Shader: Linking error! name = \"%s\"") % m_name << std::endl;
        gl_info_program(m_program);
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to link shader program! name = \"%s\"") % m_name));
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
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            throw std::runtime_error(boost::str(boost::format("Shader: Failed to get uniform info! name = \"%s\", id = %d, error = \"%s\"") % m_name % i % g_opengl_error(err)));
        } else if (uni_len > 0) {
            // Get address
            std::string uni_name = uni_str.get();
            GLint uni_addr = glGetUniformLocation(m_program, uni_str.get());
            if (uni_addr != -1) {
                c_opengl_uniform uniform{uni_name, uni_size, uni_type, uni_addr};
                std::cout << boost::format("Shader (%s): Uniform: name = \"%s\", type = \"%s\", size = %d, address = %d")
                    % m_name % uniform.name() % uniform.desc() % uniform.size() % uniform.addr() << std::endl;
                m_uniform.insert({ uni_name, uniform });
            }
        }
    }

    // Get attribute list
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
            throw std::runtime_error(boost::str(boost::format("Shader: Failed to get attribute info! name = \"%s\", id = %d") % m_name % i));
        } else if (attr_len > 0) {
            std::string attr_name = attr_str.get();
            GLint attr_addr = glGetAttribLocation(m_program, attr_str.get());
            if (attr_addr != -1) {
                c_opengl_uniform attribute{attr_name, attr_size, attr_type, attr_addr};
                std::cout << boost::format("Shader (%s): Attribute: name = \"%s\", type = \"%s\", size = %d, address = %d")
                    % m_name % attribute.name() % attribute.desc() % attribute.size() % attribute.addr() << std::endl;
                m_attribute.insert({ attr_name, attribute });
            }
        }
    }

    // Debug
    /*
    std::cout <<
        boost::format("Shader (%1%): Parameters: uniforms = %2%, attributes = %3%") %
        m_name % uni_active % attr_active << std::endl;
    */
}

c_opengl_shader_program::~c_opengl_shader_program()
{
    // Clear uniforms and attributes
    m_uniform.clear();
    m_attribute.clear();

    // Program
    glDeleteProgram(m_program);
}

// Program use
void c_opengl_shader_program::use_begin()
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

void c_opengl_shader_program::use_end()
{
    // Consistency check
    /*
    #ifndef NDEBUG
        GLuint prog_cur = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&prog_cur));
        assert(prog_cur == m_program);
    #endif
    */

    // Disable shader program
    glUseProgram(0);
}

// Uniforms
c_opengl_uniform* c_opengl_shader_program::uniform(std::string name)
{
    // Find
    auto it = m_uniform.find(name);
    if (it == m_uniform.end())
        return nullptr;
    return &(it->second);
}

// Attributes
c_opengl_uniform* c_opengl_shader_program::attribute(std::string name)
{
    // Find
    auto it = m_attribute.find(name);
    if (it == m_attribute.end())
        return nullptr;
    return &(it->second);
}

// Info log
void c_opengl_shader_program::gl_info_program(GLuint obj)
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
c_opengl_shader::c_opengl_shader()
{
    // Path
    boost::filesystem::path path(MJ_DATA_PATH);
    path /= "shader";
    if (!boost::filesystem::is_directory(path))
        throw std::runtime_error(boost::str(boost::format("Shader: Data folder does not exist! path = \"%s\"") % path.native()));

    // Program list
    boost::filesystem::path path_programs = path;
    path_programs /= "program.list";
    if (!boost::filesystem::exists(path_programs))
        throw std::runtime_error(boost::str(boost::format("Shader: Program list file does not exist! path = \"%s\"") % path_programs.native()));

    // Load list
    std::ifstream program_fs(path_programs.native());
    if (!program_fs.is_open())
        throw std::runtime_error(boost::str(boost::format("Shader: Failed to open program list file! path = \"%s\"") % path_programs.native()));
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
        std::vector<std::string> list;
        boost::split(list, header[1], boost::is_any_of(","));
        if (list.size() < 2)
            continue;
        for (auto& str : list)
            boost::trim(str);

        // Gather objects
        std::vector<std::shared_ptr<c_opengl_shader_object>> objects;
        for (auto& item : list) {
            // Path
            boost::filesystem::path fn = path;
            fn /= item + ".glsl";

            // Object
            std::shared_ptr<c_opengl_shader_object> object;
            if (boost::ends_with(item, "-vertex")) {
                object = std::make_shared<c_opengl_shader_object>(item, e_opengl_shader_type::vertex, fn.native());
            } else if (boost::ends_with(item, "-tess_control")) {
                object = std::make_shared<c_opengl_shader_object>(item, e_opengl_shader_type::tess_control, fn.native());
            } else if (boost::ends_with(item, "-tess_evaluation")) {
                object = std::make_shared<c_opengl_shader_object>(item, e_opengl_shader_type::tess_evaluation, fn.native());
            } else if (boost::ends_with(item, "-geometry")) {
                object = std::make_shared<c_opengl_shader_object>(item, e_opengl_shader_type::geometry, fn.native());
            } else if (boost::ends_with(item, "-fragment")) {
                object = std::make_shared<c_opengl_shader_object>(item, e_opengl_shader_type::fragment, fn.native());
            } else {
                throw std::runtime_error(boost::str(boost::format("Shader: Unknown type of shader! item = \"%s\", name = \"%s\"") % item % name));
            }
            objects.push_back(object);
        }
        g_opengl_check();

        // Link
        if (objects.empty())
            continue;
        auto program = std::make_shared<c_opengl_shader_program>(name, objects);
        g_opengl_check();

        // Program
        m_programs[name] = program;
    }
    g_opengl_check();
}

std::shared_ptr<c_opengl_shader_program> c_opengl_shader::program(std::string name)
{
    // Find
    auto it = m_programs.find(name);
    if (it == m_programs.end())
        return nullptr;
    return it->second;
}
