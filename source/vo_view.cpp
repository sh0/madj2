/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_global.h"
#include "vo_main.h"
#include "vo_view.h"

// Constructor and destructor
c_video_view::c_video_view(std::string name, int width, int height) :
    // Info
    m_name(name), m_width(width), m_height(height)
{
    // Debug
    std::cout << boost::format("View (%s): width=%d, height=%d") % m_name % m_width % m_height << std::endl;

    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-video.layout");
    m_window->setName(m_name);
}

c_video_view::~c_video_view()
{

}

// Dispatch
void c_video_view::dispatch()
{

}
