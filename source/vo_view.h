/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_VIEW
#define H_VO_VIEW

// Internal
#include "mj_config.h"
#include "vo_context.h"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>

// View class
class c_video_view : c_noncopiable
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
