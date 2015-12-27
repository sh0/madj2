/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_VIDEO
#define H_VIDEO_VIDEO

// Internal
#include "config.hpp"
#include "timer.hpp"
#include "opengl/opengl.hpp"
#include "opengl/shader.hpp"
#include "video/context.hpp"
#include "video/screen.hpp"
#include "video/tracker.hpp"

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

        // Trackers
        std::vector<std::shared_ptr<c_video_tracker>>& tracker_list() { return m_trackers; }
        void tracker_add(
            std::string name, std::string screen, int pos_x, int pos_y, int pos_w, int pos_h
        ) {
            for (auto& s : m_screens) {
                if (s->name() != screen)
                    continue;
                auto tracker = std::make_shared<c_video_tracker>(s->context(), name, pos_x, pos_y, pos_w, pos_h);
                m_trackers.push_back(tracker);
                s->view_add(tracker);
            }
        }

        // Dispatch
        void dispatch_input(c_time_cyclic& timer) {
            for (auto& screen : m_screens)
                screen->dispatch_input(timer);
        }

        void dispatch_render(c_time_cyclic& timer) {
            for (auto& screen : m_screens)
                screen->dispatch_render(timer);
        }

    private:
        // OpenGL
        std::shared_ptr<c_video_context> m_context;
        std::shared_ptr<c_opengl_shader> m_shader;

        // Screens
        std::vector<std::shared_ptr<c_video_screen>> m_screens;

        // Trackers
        std::vector<std::shared_ptr<c_video_tracker>> m_trackers;
};

#endif
