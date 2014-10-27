/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_MAIN
#define H_VO_MAIN

// Internal
#include "mj_config.h"
#include "vo_opengl.h"
#include "vo_context.h"
#include "vo_screen.h"

// Main video class
class c_video : c_noncopiable
{
    public:
        // Context
        std::shared_ptr<c_video_context> context(SDL_Window* window) {
            if (auto ctx = m_context.lock()) {
                ctx->make_current(window);
                return ctx;
            } else {
                ctx = std::make_shared<c_video_context>(window);
                m_context = ctx;
                return ctx;
            }
        }

        // Screens
        std::vector<std::shared_ptr<c_video_screen>>& screen_list() { return m_screens; }
        void screen_add(
            const std::string& name, const std::string& color,
            int view_cols, int view_rows,
            int pos_x, int pos_y, int width, int height, bool fullscreen
        ) {
            m_screens.push_back(std::make_shared<c_video_screen>(
                m_screens.size(), name, color,
                view_cols, view_rows,
                pos_x, pos_y, width, height, fullscreen
            ));
        }

        // Dispatch
        void dispatch() {
            for (auto& screen : m_screens)
                screen->dispatch();
        }

    private:
        // Context
        std::weak_ptr<c_video_context> m_context;

        // Screens
        std::vector<std::shared_ptr<c_video_screen>> m_screens;
};

#endif
