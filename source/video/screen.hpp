/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_SCREEN
#define H_VIDEO_SCREEN

// Internal
#include "config.hpp"
#include "opengl/opengl.hpp"
#include "video/context.hpp"
#include "video/view.hpp"

// Screen class
class c_video_screen : boost::noncopyable
{
    public:
        // Constructor and deconstructor
        c_video_screen(
            uint id, const std::string& name, const std::string& color,
            int view_cols, int view_rows,
            int pos_x, int pos_y, int width, int height, bool fullscreen
        );
        ~c_video_screen();

        // Dispatch
        void dispatch();

        // Info
        const std::string& name() { return m_name; }
        const std::string& color() { return m_color; }

        // View
        std::shared_ptr<c_video_view> view(uint id) {
            assert(m_view_list.size() > id);
            return m_view_list[id];
        }

        // Window
        int32_t width() { return m_window_width; }
        int32_t height() { return m_window_height; }
        double aspect() {
            return (
                static_cast<double>(m_window_width) /
                static_cast<double>(m_window_height)
            );
        }

    private:
        // Info
        uint m_id;
        std::string m_name;
        std::string m_color;

        // View
        int m_view_cols;
        int m_view_rows;
        std::vector<std::shared_ptr<c_video_view>> m_view_list;

        // Window
        int m_window_pos_x;
        int m_window_pos_y;
        int m_window_width;
        int m_window_height;
        bool m_window_fullscreen;
        SDL_Window* m_window;

        // Timing
        float m_time;

        // Context
        std::shared_ptr<c_video_context> m_context;

        // CEGUI
        class c_cegui {
            public:
                // Constructor and destructor
                c_cegui(std::shared_ptr<c_video_context> context) :
                    m_system(context->cegui_system()),
                    m_target(context->cegui_renderer().getDefaultRenderTarget()),
                    m_context(context->cegui_system().createGUIContext(m_target))
                { }

                // Gets
                CEGUI::RenderTarget& target() { return m_target; }
                CEGUI::GUIContext& context() { return m_context; }

            private:
                // Objects
                CEGUI::System& m_system;
                CEGUI::RenderTarget& m_target;
                CEGUI::GUIContext& m_context;
        };
        std::unique_ptr<c_cegui> m_cegui;
        CEGUI::Window* m_cegui_root;
        CEGUI::GridLayoutContainer* m_cegui_glc;

        // GL functions
        bool gl_init();
};

#endif

