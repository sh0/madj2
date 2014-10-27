/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_SCREEN
#define H_VO_SCREEN

// Internal
#include "mj_config.h"
#include "vo_opengl.h"
#include "vo_context.h"

// Screen class
class c_video_screen : c_noncopiable
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
        /*
        std::shared_ptr<c_video_view> view(uint id) {
            assert(m_view_list.size() > id);
            return m_view_list[id];
        }
        */

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
        //std::vector<std::shared_ptr<c_video_view>> m_view_list;

        // Window
        int m_window_pos_x;
        int m_window_pos_y;
        int m_window_width;
        int m_window_height;
        bool m_window_fullscreen;
        SDL_Window* m_window;

        // Context
        std::shared_ptr<c_video_context> m_context;

        // GL functions
        bool gl_init();
};

#endif

