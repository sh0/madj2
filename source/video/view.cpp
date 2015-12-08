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
c_video_view::c_video_view(std::shared_ptr<c_video_context> context, std::string name, int width, int height) :
    // Info
    m_name(name), m_width(width), m_height(height),
    // Context
    m_context(context),
    // Video
    m_video_texture(context->cegui_renderer().createTexture(name + "-texture")),
    m_video_opengl(static_cast<CEGUI::OpenGLTexture&>(m_video_texture)),
    m_video_image(CEGUI::ImageManager::getSingleton().create("BasicImage", "video/" + name)),
    m_video_basic(static_cast<CEGUI::BasicImage&>(m_video_image))
{
    // Debug
    std::cout << boost::format("View (%s): width=%d, height=%d") % m_name % m_width % m_height << std::endl;

    // Temporary video
    m_video_temp = std::make_shared<c_opengl_texture_2d>();
    m_video_temp->upload(640, 480, true);

    m_video_opengl.setOpenGLTexture(m_video_temp->object(), CEGUI::Sizef(m_video_temp->width(), m_video_temp->height()));
    if (context->cegui_renderer().isTexCoordSystemFlipped()) {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, m_video_temp->height(), m_video_temp->width(), 0.0f));
    } else {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, 0.0f, m_video_temp->width(), m_video_temp->height()));
    }
    m_video_basic.setAutoScaled(CEGUI::ASM_Disabled);
    m_video_basic.setTexture(&m_video_opengl);

    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-video.layout");
    m_window->setName(m_name);
    m_window_image = m_window->getChild("Video");
    m_window_image->setProperty("Image", m_video_image.getName());

    // Events
    m_window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&c_video_view::event_window_resize, this));
}

c_video_view::~c_video_view()
{
    // Window
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);

    // Video
    CEGUI::ImageManager::getSingleton().destroy(m_video_image);
    m_context->cegui_renderer().destroyTexture(m_video_texture);
}

// Dispatch
void c_video_view::dispatch()
{

}

// Events
bool c_video_view::event_window_resize(const CEGUI::EventArgs& event)
{
    // Update
    m_window->notifyScreenAreaChanged(true);

    //auto area = m_window_image->getInnerRectClipper();
    //std::cout << "resize width = " << area.getWidth() << ", height = " << area.getHeight() << std::endl;

    /*
    if (m_context->cegui_renderer().isTexCoordSystemFlipped()) {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, m_video_temp->height(), m_video_temp->width(), 0.0f));
    } else {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, 0.0f, m_video_temp->width(), m_video_temp->height()));
    }
    */

    // Propagate
    return false;
}
