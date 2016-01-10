/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_EFFECTS_EFFECTS
#define H_EFFECTS_EFFECTS

// Internal
#include "config.hpp"
#include "opengl/texture.hpp"

// C++
#include <array>

// Effect base class
class c_effects_effect
{
    public:
        // Destructor
        virtual ~c_effects_effect() { }

        // Execute
        virtual void process(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst) = 0;
};

class c_effects : boost::noncopyable
{
    public:
        // Constructor
        c_effects(int render_width, int render_height);

        // Renderer
        int render_width() { return m_render_width; }
        int render_height() { return m_render_height; }

    private:
        // Renderer
        int m_render_width;
        int m_render_height;
};

#endif
