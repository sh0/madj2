/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_SCREEN_GLFW
#define H_VIDEO_SCREEN_GLFW

// Internal
#include "config.hpp"
#include "opengl/opengl.hpp"
#include "video/context.hpp"
#include "video/view.hpp"

// Screen class
class c_video_screen_glfw : boost::noncopyable
{
    public:
        // Constructor and deconstructor
        c_video_screen_glfw(
            uint id, const std::string& name, const std::string& color,
            int view_cols, int view_rows,
            int pos_x, int pos_y, int width, int height, bool fullscreen
        );
        ~c_video_screen_glfw();

        // Dispatch
        void dispatch_input();
        void dispatch_render();

    private:
        // Window
        int m_window_width;
        int m_window_height;
        GLFWwindow* m_window;
};

#endif
