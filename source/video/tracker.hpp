/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_TRACKER
#define H_VIDEO_TRACKER

// Internal
#include "config.hpp"
#include "video/context.hpp"
#include "video/screen.hpp"
#include "opengl/texture.hpp"
#include "media/media.hpp"
#include "media/work.hpp"
#include "video/tracker_tempo.hpp"
#include "video/tracker_story.hpp"
#include "video/tracker_file.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/widgets/Menubar.h>
#include <CEGUI/widgets/MenuItem.h>

// Tracker class
class c_video_tracker : public c_video_view, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_tracker(std::shared_ptr<c_video_screen> screen, std::string name, int pos_x, int pos_y, int pos_w, int pos_h);
        virtual ~c_video_tracker();

        // Info
        std::string name() { return m_name; }

        // Dispatch
        virtual void dispatch(c_time_cyclic& timer);

        // Window
        virtual CEGUI::Window* window() { return m_window; }

        // Position
        virtual int pos_x() { return m_pos_x; };
        virtual int pos_y() { return m_pos_y; };
        virtual int pos_w() { return m_pos_w; };
        virtual int pos_h() { return m_pos_h; };

        // Events
        bool event_action(std::string action);

    private:
        // Info
        std::string m_name;
        int m_pos_x, m_pos_y, m_pos_w, m_pos_h;

        // Context
        std::shared_ptr<c_video_context> m_context;

        // Window
        CEGUI::Window* m_window;
        CEGUI::Window* m_window_client;
        CEGUI::Window* m_window_image;

        // Menu
        CEGUI::MenuItem* m_menu_file;

        // Widgets
        std::unique_ptr<c_video_tracker_tempo> m_widget_tempo;
        std::unique_ptr<c_video_tracker_story> m_widget_story;
        std::unique_ptr<c_video_tracker_file> m_widget_file;

        // Video
        CEGUI::Texture& m_video_texture;
        CEGUI::OpenGLTexture& m_video_opengl;
        CEGUI::Image& m_video_image;
        CEGUI::BasicImage& m_video_basic;

        // Media
        std::shared_ptr<c_media_work> m_media_work;
        std::shared_ptr<c_opengl_texture_2d> m_media_texture;

        // Events
        bool event_window_resize(const CEGUI::EventArgs& event);
        bool event_menu_file(const CEGUI::EventArgs& event);
};

#endif
