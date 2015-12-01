/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_VIDEO
#define H_VIDEO_VIDEO

// Internal
#include "config.hpp"
#include "opengl/opengl.hpp"
#include "opengl/shader.hpp"
#include "video/context.hpp"
#include "video/screen.hpp"

// Main video class
class c_video : boost::noncopyable
{
    public:
        // Destructor
        ~c_video() {
            m_screens.clear();
            m_shader.reset();
            m_context.reset();
        }

        // Context
        std::shared_ptr<c_video_context> context(SDL_Window* window) {
            if (m_context) {
                m_context->make_current(window);
            } else {
                m_context = std::make_shared<c_video_context>(window);
                m_shader = std::make_shared<c_opengl_shader>();
            }
            return m_context;
        }

        // Shaders
        std::shared_ptr<c_opengl_shader> shader() { return m_shader; }

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
        // OpenGL
        std::shared_ptr<c_video_context> m_context;
        std::shared_ptr<c_opengl_shader> m_shader;

        // Screens
        std::vector<std::shared_ptr<c_video_screen>> m_screens;
};

#endif