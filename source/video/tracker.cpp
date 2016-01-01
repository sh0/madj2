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
    std::shared_ptr<c_video_context> context,
    std::string name, int pos_x, int pos_y, int pos_w, int pos_h
) :
    // Info
    m_name(name),
    m_pos_x(pos_x), m_pos_y(pos_y), m_pos_w(pos_w), m_pos_h(pos_h),
    // Context
    m_context(context),
    // Video
    m_video_texture(context->cegui_renderer().createTexture(name + "-texture")),
    m_video_opengl(static_cast<CEGUI::OpenGLTexture&>(m_video_texture)),
    m_video_image(CEGUI::ImageManager::getSingleton().create("BasicImage", "video/" + name)),
    m_video_basic(static_cast<CEGUI::BasicImage&>(m_video_image)),
    // Media
    m_media_temp(0),
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
    if (context->cegui_renderer().isTexCoordSystemFlipped()) {
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

    // Client area
    m_window_client = m_window->getChildRecursive("Client");
    BOOST_ASSERT(m_window_client);

    // Video
    m_window_image = m_window->getChildRecursive("Video");
    BOOST_ASSERT(m_window_image);
    m_window_image->setProperty("Image", m_video_image.getName());

    // Tempo
    m_tempo = std::make_shared<c_video_tracker_tempo>(m_window_client);

    // Story
    m_story = std::make_shared<c_video_tracker_story>(m_window_client, m_media_work);

    // Events
    m_window->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&c_video_tracker::event_window_resize, this));

    // Update
    m_window->invalidate(true);
}

c_video_tracker::~c_video_tracker()
{
    // Tempo
    m_tempo.reset();

    // Story
    m_story.reset();

    // Work
    m_media_work.reset();

    // Window
    CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);

    // Video
    CEGUI::ImageManager::getSingleton().destroy(m_video_image);
    m_context->cegui_renderer().destroyTexture(m_video_texture);
}

// Dispatch
void c_video_tracker::dispatch(c_time_cyclic& timer)
{
    // Tempo
    if (m_tempo)
        m_tempo->dispatch(timer);

    // Story
    if (m_story)
        m_story->dispatch(timer);

    // Media
    m_media_work->dispatch(timer);
    if (m_media_work->state_loaded()) {
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
void c_video_tracker::event_action(std::string action)
{
    if (action == "browser_open") {
        const auto& files = c_global::media->media_files();
        if (files.empty()) {
            std::cout << "Tracker: No media files to play!" << std::endl;
            return;
        }

        if (m_media_temp >= static_cast<int>(files.size()))
            m_media_temp = 0;
        auto path = files[m_media_temp++];

        m_media_work->open(path);

        m_window->setText(m_name + " - " + path.stem().native());
    }
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
