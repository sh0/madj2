/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_CONTEXT
#define H_VO_CONTEXT

// Internal
#include "mj_config.h"
#include "vo_opengl.h"

// Video context class
class c_video_context : c_noncopiable
{
    public:
        // Constructor and destructor
        c_video_context(SDL_Window* window) {
            m_context = SDL_GL_CreateContext(window);
        }

        ~c_video_context() {
            SDL_GL_DeleteContext(m_context);
        }

        // Operations
        bool make_current(SDL_Window* window) {
            return SDL_GL_MakeCurrent(window, m_context) == 0;
        }

    private:
        // Context
        SDL_GLContext m_context;
};

#endif
