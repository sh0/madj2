/*
 * Prismriver project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Int inc
#include "opengl/uniform.hpp"

// Uniform typemap
std::map<GLenum, c_opengl_uniform::e_type> c_opengl_uniform::m_typemap =
{
    // Floats
    { GL_FLOAT, e_type::f1 }, // float
    { GL_FLOAT_VEC2, e_type::f2 }, // vec2
    { GL_FLOAT_VEC3, e_type::f3 }, // vec3
    { GL_FLOAT_VEC4, e_type::f4 }, // vec4

    // Doubles
    #ifdef GLEW_VERSION_4_1
    { GL_DOUBLE, e_type::d1 }, // double
    { GL_DOUBLE_VEC2, e_type::d2 }, // dvec2
    { GL_DOUBLE_VEC3, e_type::d3 }, // dvec3
    { GL_DOUBLE_VEC4, e_type::d4 }, // dvec4
    #endif

    // Ints
    { GL_INT, e_type::i1 }, // int
    { GL_INT_VEC2, e_type::i2 }, // ivec2
    { GL_INT_VEC3, e_type::i3 }, // ivec3
    { GL_INT_VEC4, e_type::i4 }, // ivec4

    // Uints
    { GL_UNSIGNED_INT, e_type::u1 }, // unsigned int
    { GL_UNSIGNED_INT_VEC2, e_type::u2 }, // uvec2
    { GL_UNSIGNED_INT_VEC3, e_type::u3 }, // uvec3
    { GL_UNSIGNED_INT_VEC4, e_type::u4 }, // uvec4

    // Bools
    { GL_BOOL, e_type::b1 }, // bool
    { GL_BOOL_VEC2, e_type::b2 }, // bvec2
    { GL_BOOL_VEC3, e_type::b3 }, // bvec3
    { GL_BOOL_VEC4, e_type::b4 }, // bvec4

    // Float matrices
    { GL_FLOAT_MAT2, e_type::f2x2 }, // mat2
    { GL_FLOAT_MAT3, e_type::f3x3 }, // mat3
    { GL_FLOAT_MAT4, e_type::f4x4 }, // mat4
    { GL_FLOAT_MAT2x3, e_type::f2x3 }, // mat2x3
    { GL_FLOAT_MAT2x4, e_type::f2x4 }, // mat2x4
    { GL_FLOAT_MAT3x2, e_type::f3x2 }, // mat3x2
    { GL_FLOAT_MAT3x4, e_type::f3x4 }, // mat3x4
    { GL_FLOAT_MAT4x2, e_type::f4x2 }, // mat4x2
    { GL_FLOAT_MAT4x3, e_type::f4x3 }, // mat4x3

    // Double matrices
    #if 0
    #ifdef GLEW_VERSION_4_1
    { GL_DOUBLE_MAT2, e_type::d2x2 }, // dmat2
    { GL_DOUBLE_MAT3, e_type::d3x3 }, // dmat3
    { GL_DOUBLE_MAT4, e_type::d4x4 }, // dmat4
    { GL_DOUBLE_MAT2x3, e_type::d2x3 }, // dmat2x3
    { GL_DOUBLE_MAT2x4, e_type::d2x4 }, // dmat2x4
    { GL_DOUBLE_MAT3x2, e_type::d3x2 }, // dmat3x2
    { GL_DOUBLE_MAT3x4, e_type::d3x4 }, // dmat3x4
    { GL_DOUBLE_MAT4x2, e_type::d4x2 }, // dmat4x2
    { GL_DOUBLE_MAT4x3, e_type::d4x3 }, // dmat4x3
    #endif
    #endif

    // Sampler
    { GL_SAMPLER_1D, e_type::s }, // sampler1D
    { GL_SAMPLER_2D, e_type::s }, // sampler2D
    { GL_SAMPLER_3D, e_type::s }, // sampler3D
    { GL_SAMPLER_CUBE, e_type::s }, // samplerCube
    { GL_SAMPLER_1D_SHADOW, e_type::s }, // sampler1DShadow
    { GL_SAMPLER_2D_SHADOW, e_type::s }, // sampler2DShadow
    { GL_SAMPLER_1D_ARRAY, e_type::s }, // sampler1DArray
    { GL_SAMPLER_2D_ARRAY, e_type::s }, // sampler2DArray
    { GL_SAMPLER_1D_ARRAY_SHADOW, e_type::s }, // sampler1DArrayShadow
    { GL_SAMPLER_2D_ARRAY_SHADOW, e_type::s }, // sampler2DArrayShadow
    { GL_SAMPLER_2D_MULTISAMPLE, e_type::s }, // sampler2DMS
    { GL_SAMPLER_2D_MULTISAMPLE_ARRAY, e_type::s }, // sampler2DMSArray
    { GL_SAMPLER_CUBE_SHADOW, e_type::s }, // samplerCubeShadow
    { GL_SAMPLER_BUFFER, e_type::s }, // samplerBuffer
    { GL_SAMPLER_2D_RECT, e_type::s }, // sampler2DRect
    { GL_SAMPLER_2D_RECT_SHADOW, e_type::s }, // sampler2DRectShadow

    { GL_INT_SAMPLER_1D, e_type::si }, // isampler1D
    { GL_INT_SAMPLER_2D, e_type::si }, // isampler2D
    { GL_INT_SAMPLER_3D, e_type::si }, // isampler3D
    { GL_INT_SAMPLER_CUBE, e_type::si }, // isamplerCube
    { GL_INT_SAMPLER_1D_ARRAY, e_type::si }, // isampler1DArray
    { GL_INT_SAMPLER_2D_ARRAY, e_type::si }, // isampler2DArray
    { GL_INT_SAMPLER_2D_MULTISAMPLE, e_type::si }, // isampler2DMS
    { GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, e_type::si }, // isampler2DMSArray
    { GL_INT_SAMPLER_BUFFER, e_type::si }, // isamplerBuffer
    { GL_INT_SAMPLER_2D_RECT, e_type::si }, // isampler2DRect

    { GL_UNSIGNED_INT_SAMPLER_1D, e_type::su }, // usampler1D
    { GL_UNSIGNED_INT_SAMPLER_2D, e_type::su }, // usampler2D
    { GL_UNSIGNED_INT_SAMPLER_3D, e_type::su }, // usampler3D
    { GL_UNSIGNED_INT_SAMPLER_CUBE, e_type::su }, // usamplerCube
    { GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, e_type::su }, // usampler2DArray
    { GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, e_type::su }, // usampler2DArray
    { GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, e_type::su }, // usampler2DMS
    { GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, e_type::su }, // usampler2DMSArray
    { GL_UNSIGNED_INT_SAMPLER_BUFFER, e_type::su }, // usamplerBuffer
    { GL_UNSIGNED_INT_SAMPLER_2D_RECT, e_type::su }, // usampler2DRect

    // Image
    #if 0
    #ifdef GLEW_VERSION_4_2
    { GL_INT_IMAGE_1D, e_type::mi }, // iimage1D
    { GL_INT_IMAGE_2D, e_type::mi }, // iimage2D
    { GL_INT_IMAGE_3D, e_type::mi }, // iimage3D
    { GL_INT_IMAGE_2D_RECT, e_type::mi }, // iimage2DRect
    { GL_INT_IMAGE_CUBE, e_type::mi }, // iimageCube
    { GL_INT_IMAGE_BUFFER, e_type::mi }, // iimageBuffer
    { GL_INT_IMAGE_1D_ARRAY, e_type::mi }, // iimage1DArray
    { GL_INT_IMAGE_2D_ARRAY, e_type::mi }, // iimage2DArray
    { GL_INT_IMAGE_2D_MULTISAMPLE, e_type::mi }, // iimage2DMS
    { GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, e_type::mi }, // iimage2DMSArray

    { GL_UNSIGNED_INT_IMAGE_1D, e_type::mu }, // uimage1D
    { GL_UNSIGNED_INT_IMAGE_2D, e_type::mu }, // uimage2D
    { GL_UNSIGNED_INT_IMAGE_3D, e_type::mu }, // uimage3D
    { GL_UNSIGNED_INT_IMAGE_2D_RECT, e_type::mu }, // uimage2DRect
    { GL_UNSIGNED_INT_IMAGE_CUBE, e_type::mu }, // uimageCube
    { GL_UNSIGNED_INT_IMAGE_BUFFER, e_type::mu }, // uimageBuffer
    { GL_UNSIGNED_INT_IMAGE_1D_ARRAY, e_type::mu }, // uimage1DArray
    { GL_UNSIGNED_INT_IMAGE_2D_ARRAY, e_type::mu }, // uimage2DArray
    { GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, e_type::mu }, // uimage2DMS
    { GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, e_type::mu }, // uimage2DMSArray
    #endif

    // Atomic counter
    #ifdef GLEW_VERSION_4_2
    { GL_UNSIGNED_INT_ATOMIC_COUNTER, e_type::mu }, // atomic_uint
    #endif
    #endif

    // Count
    { 0, e_type::count }
};
