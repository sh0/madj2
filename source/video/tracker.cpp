/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "video/video.hpp"
#include "video/tracker.hpp"

// Constructor and destructor
c_video_tracker::c_video_tracker(
    std::shared_ptr<c_video_screen> screen,
    std::string name, int pos_x, int pos_y, int pos_w, int pos_h
) :
    // Info
    m_name(name),
    m_pos_x(pos_x), m_pos_y(pos_y), m_pos_w(pos_w), m_pos_h(pos_h),
    // Context
    m_context(screen->context()),
    // Video
    m_video_texture(m_context->cegui_renderer().createTexture(name + "-texture")),
    m_video_opengl(static_cast<CEGUI::OpenGLTexture&>(m_video_texture)),
    m_video_image(CEGUI::ImageManager::getSingleton().create("BasicImage", "video/" + name)),
    m_video_basic(static_cast<CEGUI::BasicImage&>(m_video_image)),
    // Media
    m_media_work(std::make_shared<c_media_work>()),
    m_media_texture(nullptr) //(std::make_shared<c_opengl_texture_2d>())
{
    // Debug
    //std::cout << boost::format("View (%s)") % m_name << std::endl;

    // Temporary video
    #if 0
    m_video_temp = std::make_shared<c_opengl_texture_2d>();
    m_video_temp->upload(640, 480, true);

    m_video_opengl.setOpenGLTexture(m_video_temp->object(), CEGUI::Sizef(m_video_temp->width(), m_video_temp->height()));
    if (m_context->cegui_renderer().isTexCoordSystemFlipped()) {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, m_video_temp->height(), m_video_temp->width(), 0.0f));
    } else {
        m_video_basic.setArea(CEGUI::Rectf(0.0f, 0.0f, m_video_temp->width(), m_video_temp->height()));
    }
    #endif
    m_video_basic.setAutoScaled(CEGUI::ASM_Disabled);
    m_video_basic.setTexture(&m_video_opengl);

    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-video.layout");
    BOOST_ASSERT(m_window);
    m_window->setName(m_name);
    m_window->setText(m_name);
    m_window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&c_video_tracker::event_window_resize, this));
    m_window->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&c_video_tracker::event_window_key_down, this));

    // Client area
    m_window_client = m_window->getChildRecursive("Client");
    BOOST_ASSERT(m_window_client);

    // Video
    m_window_image = m_window->getChildRecursive("Video");
    BOOST_ASSERT(m_window_image);
    m_window_image->setProperty("Image", m_video_image.getName());

    // Menu
    m_menu_file = dynamic_cast<CEGUI::MenuItem*>(m_window->getChildRecursive("MenuFile"));
    BOOST_ASSERT(m_menu_file);
    m_menu_file->subscribeEvent(CEGUI::MenuItem::EventClicked, CEGUI::Event::Subscriber(&c_video_tracker::event_menu_file, this));

    // Widgets
    m_widget_tempo = std::unique_ptr<c_video_tracker_tempo>(new c_video_tracker_tempo(m_window_client));
    m_widget_story = std::unique_ptr<c_video_tracker_story>(new c_video_tracker_story(m_window_client, m_media_work));
    m_widget_file = std::unique_ptr<c_video_tracker_file>(new c_video_tracker_file(screen->root(), name, m_media_work));

    // Update
    m_window->invalidate(true);
}

c_video_tracker::~c_video_tracker()
{
    // Widgets
    m_widget_tempo.reset();
    m_widget_story.reset();
    m_widget_file.reset();

    // Work
    m_media_work.reset();

    // Window
    m_window->removeAllEvents();
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);

    // Video
    CEGUI::ImageManager::getSingleton().destroy(m_video_image);
    m_context->cegui_renderer().destroyTexture(m_video_texture);
}

// Dispatch
void c_video_tracker::dispatch(c_time_cyclic& timer)
{
    // Widgets
    if (m_widget_tempo)
        m_widget_tempo->dispatch(timer);
    if (m_widget_story)
        m_widget_story->dispatch(timer);
    if (m_widget_file)
        m_widget_file->dispatch(timer);

    // Media
    m_media_work->dispatch(timer);
    if (m_media_work->state_loaded()) {
        //m_window->setText(m_name + " - " + path.stem().native());
        auto frame = m_media_work->frame_play();
        if (frame) {
            bool update = (m_media_texture != frame);
            if (!update && m_media_texture && frame)
                update = (frame->width() != m_media_texture->width() || frame->height() != m_media_texture->height());
            m_media_texture = frame;
            if (update) {
                m_video_opengl.setOpenGLTexture(m_media_texture->object(), CEGUI::Sizef(m_media_texture->width(), m_media_texture->height()));
                m_video_basic.setArea(CEGUI::Rectf(0.0f, 0.0f, m_media_texture->width(), m_media_texture->height()));
                //m_video_basic.setArea(CEGUI::Rectf(0.0f, 0.0f, 200, 200));
                m_video_basic.setAutoScaled(CEGUI::ASM_Disabled);
                m_video_basic.setTexture(&m_video_opengl);

                m_window->invalidate(true);
                m_window->notifyScreenAreaChanged(true);
            }
        }
    }
}

// Events
bool c_video_tracker::event_action(std::string action, bool active)
{
    // Actions
    if (active && action == "browser_open") {
        m_widget_file->show();
        return true;
    }

    // Unhandled
    return false;
}

bool c_video_tracker::event_window_resize(const CEGUI::EventArgs& event)
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

bool c_video_tracker::event_window_key_down(const CEGUI::EventArgs& event)
{
    const CEGUI::KeyEventArgs& keys = dynamic_cast<const CEGUI::KeyEventArgs&>(event);
    if (keys.scancode == CEGUI::Key::Scan::Return) {
        m_widget_file->show();
        return true;
    } else if (keys.scancode == CEGUI::Key::Scan::Space) {
        m_media_work->event_pause();
        return true;
    } else if (keys.scancode == CEGUI::Key::Scan::ArrowRight) {
        m_media_work->event_skip_forward();
        return true;
    } else if (keys.scancode == CEGUI::Key::Scan::ArrowLeft) {
        m_media_work->event_skip_backward();
        return true;
    } else if (keys.scancode == CEGUI::Key::Scan::ArrowUp) {
        m_media_work->event_play_forward();
        return true;
    } else if (keys.scancode == CEGUI::Key::Scan::ArrowDown) {
        m_media_work->event_play_backward();
        return true;
    }

    // Not handled
    return false;
}

bool c_video_tracker::event_menu_file(const CEGUI::EventArgs& event)
{
    m_widget_file->show();
    return false;
}
