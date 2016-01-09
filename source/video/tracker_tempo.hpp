/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_TRACKER_TEMPO
#define H_VIDEO_TRACKER_TEMPO

// Internal
#include "config.hpp"
#include "controller/controller.hpp"

// C++
#include <array>

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/ProgressBar.h>

// Tracker class
class c_video_tracker_tempo : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_tracker_tempo(CEGUI::Window* root);
        ~c_video_tracker_tempo();

        // Dispatch
        void dispatch(c_time_cyclic& timer);

    private:
        // Window
        CEGUI::Window* m_window;

        std::array<CEGUI::Window*, 2> m_button;
        std::array<CEGUI::ProgressBar*, 2> m_progress;
};

#endif
