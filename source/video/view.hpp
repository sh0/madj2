/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_VIEW
#define H_VIDEO_VIEW

// Internal
#include "config.hpp"
#include "timer.hpp"

// CEGUI
#include <CEGUI/Window.h>

// View base class
class c_video_view
{
    public:
        // Destructor
        virtual ~c_video_view() { }

        // Dispatch
        virtual void dispatch(c_time_cyclic& timer) { };

        // Window
        virtual CEGUI::Window* window() = 0;

        // Position
        virtual int pos_x() = 0;
        virtual int pos_y() = 0;
        virtual int pos_w() = 0;
        virtual int pos_h() = 0;
};

#endif
