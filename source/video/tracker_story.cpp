/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/tracker_story.hpp"

// Constructor and destructor
c_video_tracker_story::c_video_tracker_story(CEGUI::Window* root, std::shared_ptr<c_media_work> work) :
    // Widgets
    m_slider(nullptr),
    m_tracking(false),
    m_text({{nullptr, nullptr, nullptr}}),
    // Work
    m_work(work)
{
    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-story.layout");
    root->addChild(m_window);

    // Widgets
    m_slider = dynamic_cast<CEGUI::Slider*>(m_window->getChildRecursive("Slider"));
    //m_slider->getThumb()->setHotTracked(true);
    m_text[0] = dynamic_cast<CEGUI::DefaultWindow*>(m_window->getChildRecursive("TimeL"));
    m_text[1] = dynamic_cast<CEGUI::DefaultWindow*>(m_window->getChildRecursive("TimeC"));
    m_text[2] = dynamic_cast<CEGUI::DefaultWindow*>(m_window->getChildRecursive("TimeR"));

    // Events
    m_slider->subscribeEvent(CEGUI::Slider::EventThumbTrackStarted, CEGUI::Event::Subscriber(&c_video_tracker_story::event_thumb_track_started, this));
    m_slider->subscribeEvent(CEGUI::Slider::EventThumbTrackEnded, CEGUI::Event::Subscriber(&c_video_tracker_story::event_thumb_track_ended, this));
}

c_video_tracker_story::~c_video_tracker_story()
{
    // Window
    m_window->removeAllEvents();
    //CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);
}

// Dispatch
void c_video_tracker_story::dispatch(c_time_cyclic& timer)
{
    // Time postition and duration
    double t = m_work->playback_time();
    double d = m_work->playback_length();

    // Text
    auto formatter = [](double r) {
        int rq = std::min<int>(999, std::max<int>(0, std::fmod(r, 1.0) * 1000.0));
        int rs = std::min<int>(59, std::max<int>(0, std::fmod(r, 60.0)));
        int rm = std::min<int>(59, std::max<int>(0, std::fmod(r, 3600.0) / 60.0));
        int rh = std::max<int>(0, std::fmod(r, 216000.0) / 3600.0);
        return boost::str(boost::format("%02d:%02d:%02d.%03d") % rh % rm % rs % rq);
    };
    m_text[0]->setText(formatter(t));
    m_text[1]->setText(formatter(d));
    m_text[2]->setText(std::string("-") + formatter(d - t));

    // Slider
    if (!m_tracking)
        m_slider->setCurrentValue(t / d);
}

// Events
bool c_video_tracker_story::event_thumb_track_started(const CEGUI::EventArgs& event)
{
    m_tracking = true;
    return false;
}

bool c_video_tracker_story::event_thumb_track_ended(const CEGUI::EventArgs& event)
{
    m_work->playback_play(m_work->playback_play_speed(), m_work->playback_length() * m_slider->getCurrentValue());

    m_tracking = false;
    return false;
}
