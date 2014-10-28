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
c_video_view::c_video_view(
    std::shared_ptr<c_video_context> context,
    int width, int height,
    int view_l, int view_t, int view_r, int view_b
) :
    // Video context
    m_video_context(context),
    // Screen
    m_screen_width(width), m_screen_height(height),
    // View
    m_view_l(view_l), m_view_t(view_t), m_view_r(view_r), m_view_b(view_b),
    // CEGUI
    m_cegui_target(context->cegui_renderer().getDefaultRenderTarget()),
    m_cegui_context(context->cegui_system().createGUIContext(m_cegui_target))
{
    // Debug
    std::cout << boost::format("View: l=%d, t=%d, r=%d, b=%d") % m_view_l % m_view_t % m_view_r % m_view_b << std::endl;

    CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    CEGUI::Window* window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-video.xml", "madj");
    //m_cegui_context.getRootWindow()->addChild(window);
    m_cegui_context.setRootWindow(window);
}

c_video_view::~c_video_view()
{
    // CEGUI
    m_video_context->cegui_system().destroyGUIContext(m_cegui_context);
}

// Dispatch
void c_video_view::dispatch()
{
    /*
    CEGUI::Rectf area(
        static_cast<float>(m_view_l) / static_cast<float>(m_screen_width), // left
        static_cast<float>(m_view_t) / static_cast<float>(m_screen_height), // top
        static_cast<float>(m_view_r) / static_cast<float>(m_screen_width), // right
        static_cast<float>(m_view_b) / static_cast<float>(m_screen_height) // bottom
    );
    */
    //CEGUI::Rectf area(0.0f, 0.0f, 1.0f, 1.0f);
    //m_cegui_target.setArea(area);
    m_cegui_target.activate();
    m_video_context->cegui_renderer().beginRendering();
    m_cegui_context.draw();
    m_video_context->cegui_renderer().endRendering();
    m_cegui_target.deactivate();
}
