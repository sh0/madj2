/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "video/tracker_tempo.hpp"

// Constructor and destructor
c_video_tracker_tempo::c_video_tracker_tempo(CEGUI::Window* root)
{
    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-tempo.layout");
    root->addChild(m_window);
}

c_video_tracker_tempo::~c_video_tracker_tempo()
{
    // Window
    //CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);
}
