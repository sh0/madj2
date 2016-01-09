/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_EFFECTS_EFFECTS
#define H_EFFECTS_EFFECTS

// Internal
#include "config.hpp"
#include "opengl/texture.hpp"

class c_effect
{
    public:
        // Destructor
        virtual ~c_effect() { }

        // Execute
        virtual void process(std::shared_ptr<c_opengl_texture_2d> src, std::shared_ptr<c_opengl_texture_2d> dst) = 0;
};

#endif
