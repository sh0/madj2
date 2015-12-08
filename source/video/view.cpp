/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "video/video.hpp"
#include "video/view.hpp"

// Constructor and destructor
c_video_view::c_video_view(std::string name, int width, int height) :
    // Info
    m_name(name), m_width(width), m_height(height)
{
    // Debug
    std::cout << boost::format("View (%s): width=%d, height=%d") % m_name % m_width % m_height << std::endl;

    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-video2.layout");
    m_window->setName(m_name);
}

c_video_view::~c_video_view()
{

}

// Dispatch
void c_video_view::dispatch()
{

}
