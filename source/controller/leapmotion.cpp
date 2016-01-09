/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/video.hpp"
#include "video/tracker.hpp"
#include "controller/leapmotion.hpp"

// Checks
#ifdef LEAPMOTION_FOUND

// Constructor and destructor
c_controller_leapmotion::c_controller_leapmotion() :
    m_temp_videopos(-1),
    m_temp_handpos(-1)
{
    // Listener
    m_controller.addListener(*this);
}

c_controller_leapmotion::~c_controller_leapmotion()
{
    // Listener
    m_controller.removeListener(*this);
}

// Devices
std::vector<std::shared_ptr<c_controller_device>> c_controller_leapmotion::devices()
{
    std::vector<std::shared_ptr<c_controller_device>> devices;
    devices.push_back(std::make_shared<c_controller_leapmotion>());
    return devices;
}

// Dispatch
void c_controller_leapmotion::dispatch_input(c_time_cyclic& timer)
{
    // Check
    if (!m_controller.isConnected())
        return;

    // Tracker
    if (c_global::video->tracker_list().empty())
        return;
    auto tracker = c_global::video->tracker_list()[0];

    // Work
    auto work = tracker->work();
    if (!work)
        return;

    // Frame
    auto frame = m_controller.frame();
    auto hands = frame.hands();

    if (!hands.isEmpty()) {
        auto hand = hands.frontmost();
        auto center = hand.palmPosition();
        float distance = center.y;

        //std::cout << "hand! " << distance << std::endl;
        if (work->playback_mode() != c_media_work::PLAYBACK_STOP || m_temp_videopos < 0 || m_temp_handpos < 0) {
            work->playback_stop();
            m_temp_videopos = work->playback_time();
            m_temp_handpos = distance;
        }
        work->playback_stop(m_temp_videopos + ((distance - 250.0) / 100.0));

    } else if (m_temp_videopos >= 0) {
        work->playback_play(work->playback_play_speed());
        m_temp_videopos = -1;
        m_temp_handpos = -1;
    }
}

#endif
