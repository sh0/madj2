/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_VIEW
#define H_VIDEO_VIEW

// Internal
#include "config.hpp"
#include "video/context.hpp"
#include "opengl/texture.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>

// View class
class c_video_view : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_view(std::shared_ptr<c_video_context> context, std::string name, int width, int height);
        ~c_video_view();

        // Dispatch
        void dispatch();

        // Window
        CEGUI::Window* window() { return m_window; }

    private:
        // Info
        std::string m_name;
        int m_width;
        int m_height;

        // Context
        std::shared_ptr<c_video_context> m_context;

        // Window
        CEGUI::Window* m_window;
        CEGUI::Window* m_window_image;

        // Video
        CEGUI::Texture& m_video_texture;
        CEGUI::OpenGLTexture& m_video_opengl;
        CEGUI::Image& m_video_image;
        CEGUI::BasicImage& m_video_basic;
        std::shared_ptr<c_opengl_texture_2d> m_video_temp;

        // Events
        bool event_window_resize(const CEGUI::EventArgs& event);
};

#endif
