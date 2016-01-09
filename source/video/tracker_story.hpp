/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_TRACKER_STORY
#define H_VIDEO_TRACKER_STORY

// Internal
#include "config.hpp"
#include "media/work.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/Slider.h>
#include <CEGUI/widgets/Thumb.h>
#include <CEGUI/widgets/DefaultWindow.h>

// Tracker class
class c_video_tracker_story : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_tracker_story(CEGUI::Window* root, std::shared_ptr<c_media_work> work);
        ~c_video_tracker_story();

        // Dispatch
        void dispatch(c_time_cyclic& timer);

    private:
        // Window
        CEGUI::Window* m_window;

        // Widgets
        CEGUI::Slider* m_slider;
        bool m_tracking;
        std::array<CEGUI::DefaultWindow*, 3> m_text;

        // Work
        std::shared_ptr<c_media_work> m_work;

        // Events
        bool event_thumb_track_started(const CEGUI::EventArgs& event);
        bool event_thumb_track_ended(const CEGUI::EventArgs& event);
};

#endif
