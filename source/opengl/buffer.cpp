/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "opengl/buffer.hpp"

// Usage map
const std::map<e_opengl_buffer_usage, GLenum> g_opengl_buffer_usage_map = {
    { e_opengl_buffer_usage::stream_draw, GL_STREAM_DRAW },
    { e_opengl_buffer_usage::stream_read, GL_STREAM_READ },
    { e_opengl_buffer_usage::stream_copy, GL_STATIC_DRAW },
    { e_opengl_buffer_usage::static_draw, GL_STATIC_READ },
    { e_opengl_buffer_usage::static_read, GL_STATIC_READ },
    { e_opengl_buffer_usage::static_copy, GL_STATIC_COPY },
    { e_opengl_buffer_usage::dynamic_draw, GL_DYNAMIC_DRAW },
    { e_opengl_buffer_usage::dynamic_read, GL_DYNAMIC_READ },
    { e_opengl_buffer_usage::dynamic_copy, GL_DYNAMIC_COPY }
};

// Mode map
const std::map<e_opengl_buffer_mode, GLenum> g_opengl_buffer_mode_map = {
    { e_opengl_buffer_mode::write_only, GL_WRITE_ONLY },
    { e_opengl_buffer_mode::read_only, GL_READ_ONLY },
    { e_opengl_buffer_mode::read_write, GL_READ_WRITE }
};
