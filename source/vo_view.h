/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VO_VIEW
#define H_VO_VIEW

// Internal
#include "mj_config.h"
#include "vo_context.h"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>

// View class
class c_video_view : c_noncopiable
{
    public:
        // Constructor and destructor
        c_video_view(
            std::shared_ptr<c_video_context> context,
            int width, int height,
            int view_l, int view_t, int view_r, int view_b
        );
        ~c_video_view();

        // Dispatch
        void dispatch();

    private:
        // Video context
        std::shared_ptr<c_video_context> m_video_context;

        // Screen
        int m_screen_width;
        int m_screen_height;

        // View
        int m_view_l, m_view_t;
        int m_view_r, m_view_b;

        // CEGUI
        CEGUI::RenderTarget& m_cegui_target;
        CEGUI::GUIContext& m_cegui_context;
};

#endif
