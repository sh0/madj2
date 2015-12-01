/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_OPENGL_UNIFORM
#define H_OPENGL_UNIFORM

// Internal
#include "opengl/opengl.hpp"

// C++
#include <map>

// OpenCV
#include <opencv2/core.hpp>

// Shader uniform class
class c_opengl_uniform
{
    public:
        // Constructor
        c_opengl_uniform(
            std::string name, GLint size, GLenum type_gl, GLint addr
        ) :
            m_name(name),
            m_size(size),
            m_type(m_typemap[type_gl]),
            m_addr(addr)
        { }

        // Types
        enum class e_type {
            // Unknown
            unknown = 0,

            // Floats - float, double
            f1, f2, f3, f4,
            #ifdef GLEW_VERSION_4_1
            d1, d2, d3, d4,
            #endif

            // Integers - int, uint
            i1, i2, i3, i4,
            u1, u2, u3, u4,

            // Booleans - bool
            b1, b2, b3, b4,

            // Matrixes - float, double
            f2x2, f3x3, f4x4, f2x3, f2x4, f3x2, f3x4, f4x2, f4x3,
            #ifdef GLEW_VERSION_4_1
            d2x2, d3x3, d4x4, d2x3, d2x4, d3x2, d3x4, d4x2, d4x3,
            #endif

            // Samplers
            s, si, su,

            // Count
            count
        };

        // Properties
        std::string& name() { return m_name; }
        GLint size() { return m_size; }
        e_type type() { return m_type; }

        // Floats
        void set_f1(GLfloat v0) {
            assert(m_type == e_type::f1 && m_size == 1);
            glUniform1f(m_addr, v0);
        }
        void set_f2(GLfloat v0, GLfloat v1) {
            assert(m_type == e_type::f2 && m_size == 1);
            glUniform2f(m_addr, v0, v1);
        }
        void set_f3(GLfloat v0, GLfloat v1, GLfloat v2) {
            assert(m_type == e_type::f3 && m_size == 1);
            glUniform3f(m_addr, v0, v1, v2);
        }
        void set_f4(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
            assert(m_type == e_type::f4 && m_size == 1);
            glUniform4f(m_addr, v0, v1, v2, v3);
        }
        void set_f1v(GLsizei count, GLfloat* value) {
            assert(m_type == e_type::f1 && count <= m_size);
            glUniform1fv(m_addr, count, value);
        }
        void set_f2v(GLsizei count, GLfloat* value) {
            assert(m_type == e_type::f2 && count <= m_size);
            glUniform2fv(m_addr, count, value);
        }
        void set_f3v(GLsizei count, GLfloat* value) {
            assert(m_type == e_type::f3 && count <= m_size);
            glUniform3fv(m_addr, count, value);
        }
        void set_f4v(GLsizei count, GLfloat* value) {
            assert(m_type == e_type::f4 && count <= m_size);
            glUniform4fv(m_addr, count, value);
        }

        // Doubles
        #ifdef GLEW_VERSION_4_1
        void set_d1(double v0) {
            assert(m_type == e_type::d1 && m_size == 1);
            glUniform1d(m_addr, v0);
        }
        void set_d2(double v0, double v1) {
            assert(m_type == e_type::d2 && m_size == 1);
            glUniform2d(m_addr, v0, v1);
        }
        void set_d3(double v0, double v1, double v2) {
            assert(m_type == e_type::d3 && m_size == 1);
            glUniform3d(m_addr, v0, v1, v2);
        }
        void set_d4(double v0, double v1, double v2, double v3) {
            assert(m_type == e_type::d4 && m_size == 1);
            glUniform4d(m_addr, v0, v1, v2, v3);
        }
        void set_d1v(GLsizei count, double* value) {
            assert(m_type == e_type::d1 && count <= m_size);
            glUniform1dv(m_addr, count, value);
        }
        void set_d2v(GLsizei count, double* value) {
            assert(m_type == e_type::d2 && count <= m_size);
            glUniform2dv(m_addr, count, value);
        }
        void set_d3v(GLsizei count, double* value) {
            assert(m_type == e_type::d3 && count <= m_size);
            glUniform3dv(m_addr, count, value);
        }
        void set_d4v(GLsizei count, double* value) {
            assert(m_type == e_type::d4 && count <= m_size);
            glUniform4dv(m_addr, count, value);
        }
        #endif

        // Ints
        void set_i1(GLint v0) {
            assert(m_type == e_type::i1 && m_size == 1);
            glUniform1i(m_addr, v0);
        }
        void set_i2(GLint v0, GLint v1) {
            assert(m_type == e_type::i2 && m_size == 1);
            glUniform2i(m_addr, v0, v1);
        }
        void set_i3(GLint v0, GLint v1, GLint v2) {
            assert(m_type == e_type::i3 && m_size == 1);
            glUniform3i(m_addr, v0, v1, v2);
        }
        void set_i4(GLint v0, GLint v1, GLint v2, GLint v3) {
            assert(m_type == e_type::i4 && m_size == 1);
            glUniform4i(m_addr, v0, v1, v2, v3);
        }
        void set_i1v(GLsizei count, GLint* value) {
            assert(m_type == e_type::i1 && count <= m_size);
            glUniform1iv(m_addr, count, value);
        }
        void set_i2v(GLsizei count, GLint* value) {
            assert(m_type == e_type::i2 && count <= m_size);
            glUniform2iv(m_addr, count, value);
        }
        void set_i3v(GLsizei count, GLint* value) {
            assert(m_type == e_type::i3 && count <= m_size);
            glUniform3iv(m_addr, count, value);
        }
        void set_i4v(GLsizei count, GLint* value) {
            assert(m_type == e_type::i4 && count <= m_size);
            glUniform4iv(m_addr, count, value);
        }

        // Uints
        void set_u1(GLuint v0) {
            assert(m_type == e_type::u1 && m_size == 1);
            glUniform1ui(m_addr, v0);
        }
        void set_u2(GLuint v0, GLuint v1) {
            assert(m_type == e_type::u2 && m_size == 1);
            glUniform2ui(m_addr, v0, v1);
        }
        void set_u3(GLuint v0, GLuint v1, GLuint v2) {
            assert(m_type == e_type::u3 && m_size == 1);
            glUniform3ui(m_addr, v0, v1, v2);
        }
        void set_u4(GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
            assert(m_type == e_type::u4 && m_size == 1);
            glUniform4ui(m_addr, v0, v1, v2, v3);
        }
        void set_u1v(GLsizei count, GLuint* value) {
            assert(m_type == e_type::u1 && count <= m_size);
            glUniform1uiv(m_addr, count, value);
        }
        void set_u2v(GLsizei count, GLuint* value) {
            assert(m_type == e_type::u2 && count <= m_size);
            glUniform2uiv(m_addr, count, value);
        }
        void set_u3v(GLsizei count, GLuint* value) {
            assert(m_type == e_type::u3 && count <= m_size);
            glUniform3uiv(m_addr, count, value);
        }
        void set_u4v(GLsizei count, GLuint* value) {
            assert(m_type == e_type::u4 && count <= m_size);
            glUniform4uiv(m_addr, count, value);
        }

        // Bools
        void set_b1(bool v0) {
            assert(m_type == e_type::b1 && m_size == 1);
            glUniform1i(m_addr, v0);
        }
        void set_b2(bool v0, bool v1) {
            assert(m_type == e_type::b2 && m_size == 1);
            glUniform2i(m_addr, v0, v1);
        }
        void set_b3(bool v0, bool v1, bool v2) {
            assert(m_type == e_type::b3 && m_size == 1);
            glUniform3i(m_addr, v0, v1, v2);
        }
        void set_b4(bool v0, bool v1, bool v2, bool v3) {
            assert(m_type == e_type::b4 && m_size == 1);
            glUniform4i(m_addr, v0, v1, v2, v3);
        }
        void set_b1v(GLsizei count, GLint* value) {
            assert(m_type == e_type::b1 && count <= m_size);
            glUniform1iv(m_addr, count, value);
        }
        void set_b2v(GLsizei count, GLint* value) {
            assert(m_type == e_type::b2 && count <= m_size);
            glUniform2iv(m_addr, count, value);
        }
        void set_b3v(GLsizei count, GLint* value) {
            assert(m_type == e_type::b3 && count <= m_size);
            glUniform3iv(m_addr, count, value);
        }
        void set_b4v(GLsizei count, GLint* value) {
            assert(m_type == e_type::b4 && count <= m_size);
            glUniform4iv(m_addr, count, value);
        }

        // Float matrices
        void set_f3x3(cv::Matx33f& m) {
            assert(m_type == e_type::f3x3);
            float r[9];
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    r[i * 3 + j] = m(j, i); //m(i, j);
            glUniformMatrix3fv(m_addr, 1, GL_FALSE, r);
        }

        void set_f4x4(float* r) {
            assert(m_type == e_type::f4x4);
            glUniformMatrix4fv(m_addr, 1, GL_FALSE, r);
        }
        #if 0
        void set_f2x2(glm::mat2x2& m) {
            assert(m_type == e_type::f2x2);
            glUniformMatrix2fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f4x4(glm::mat4x4& m) {
            assert(m_type == e_type::f4x4);
            glUniformMatrix4fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f2x3(glm::mat2x3& m) {
            assert(m_type == e_type::f2x3);
            glUniformMatrix2x3fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f2x4(glm::mat2x4& m) {
            assert(m_type == e_type::f2x4);
            glUniformMatrix2x4fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f3x2(glm::mat3x2& m) {
            assert(m_type == e_type::f3x2);
            glUniformMatrix3x2fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f3x4(glm::mat3x4& m) {
            assert(m_type == e_type::f3x4);
            glUniformMatrix3x4fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f4x2(glm::mat4x2& m) {
            assert(m_type == e_type::f4x2);
            glUniformMatrix4x2fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_f4x3(glm::mat4x3& m) {
            assert(m_type == e_type::f4x3);
            glUniformMatrix4x3fv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        #endif

        // Double matrices
        #if 0
        #ifdef GLEW_VERSION_4_1
        void set_d2x2(glm::dmat2x2& m) {
            assert(m_type == e_type::d2x2);
            glUniformMatrix2dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d3x3(glm::dmat3x3& m) {
            assert(m_type == e_type::d3x3);
            glUniformMatrix3dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d4x4(glm::dmat4x4& m) {
            assert(m_type == e_type::d4x4);
            glUniformMatrix4dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d2x3(glm::dmat2x3& m) {
            assert(m_type == e_type::d2x3);
            glUniformMatrix2x3dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d2x4(glm::dmat2x4& m) {
            assert(m_type == e_type::d2x4);
            glUniformMatrix2x4dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d3x2(glm::dmat3x2& m) {
            assert(m_type == e_type::d3x2);
            glUniformMatrix3x2dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d3x4(glm::dmat3x4& m) {
            assert(m_type == e_type::d3x4);
            glUniformMatrix3x4dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d4x2(glm::dmat4x2& m) {
            assert(m_type == e_type::d4x2);
            glUniformMatrix4x2dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        void set_d4x3(glm::dmat4x3& m) {
            assert(m_type == e_type::d4x3);
            glUniformMatrix4x3dv(m_addr, 1, GL_FALSE, glm::value_ptr(m));
        }
        #endif
        #endif

        // Sampler
        void set_s(GLint id) {
            assert(m_type == e_type::s);
            glUniform1i(m_addr, id);
        }
        void set_si(GLint id) {
            assert(m_type == e_type::si);
            glUniform1i(m_addr, id);
        }
        void set_su(GLint id) {
            assert(m_type == e_type::su);
            glUniform1i(m_addr, id);
        }

    private:
        // Data
        std::string m_name;
        GLint m_size;
        e_type m_type;
        GLint m_addr;

        // Typemap
        static std::map<GLenum, e_type> m_typemap;
};

#endif
