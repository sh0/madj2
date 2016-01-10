/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_EFFECTS_FRAMING
#define H_EFFECTS_FRAMING

// Internal
#include "config.hpp"
#include "effects/effects.hpp"

// Zoombox framing
class c_effects_zoombox : public c_effects_effect, boost::noncopyable
{
    public:
        // Constructor
        c_effects_zoombox();

        // Execute
        virtual void process(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst);

    private:
        // Shader
        std::shared_ptr<c_opengl_shader_program> m_program;
};

#endif
