/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_TRACKER_TEMPO
#define H_VIDEO_TRACKER_TEMPO

// Internal
#include "config.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>

// Tracker class
class c_video_tracker_tempo : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_tracker_tempo(CEGUI::Window* root);
        ~c_video_tracker_tempo();

    private:
        // Window
        CEGUI::Window* m_window;
};

#endif
