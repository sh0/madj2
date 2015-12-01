/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_VIEW
#define H_VIDEO_VIEW

// Internal
#include "config.hpp"
#include "video/context.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>

// View class
class c_video_view : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_view(std::string name, int width, int height);
        ~c_video_view();

        // Dispatch
        void dispatch();

        // Window
        CEGUI::Window* window() { return m_window; }

    private:
        // Info
        std::string m_name;
        int m_width;
        int m_height;

        // Window
        CEGUI::Window* m_window;
};

#endif
