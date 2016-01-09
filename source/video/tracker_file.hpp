/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_TRACKER_FILE
#define H_VIDEO_TRACKER_FILE

// Internal
#include "config.hpp"
#include "media/media.hpp"
#include "media/work.hpp"

// CEGUI
#include <CEGUI/GUIContext.h>
#include <CEGUI/Window.h>
#include <CEGUI/WindowManager.h>
#include <CEGUI/widgets/FrameWindow.h>
#include <CEGUI/widgets/Editbox.h>
#include <CEGUI/widgets/Listbox.h>
#include <CEGUI/widgets/ListboxTextItem.h>
#include <CEGUI/widgets/PushButton.h>

// Tracker class
class c_video_tracker_file : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_video_tracker_file(CEGUI::Window* root, std::string name, std::shared_ptr<c_media_work> work);
        ~c_video_tracker_file();

        // Dispatch
        void dispatch(c_time_cyclic& timer);

        // Operations
        void show();
        void load(boost::filesystem::path path);
        void list(std::string query);

    private:
        // Window
        CEGUI::Window* m_root;
        CEGUI::FrameWindow* m_window;

        // Widgets
        CEGUI::Editbox* m_editbox;
        CEGUI::Listbox* m_listbox;

        // Work
        std::shared_ptr<c_media_work> m_work;

        // Files
        const std::vector<boost::filesystem::path>& m_files;
        std::vector<CEGUI::ListboxTextItem*> m_items;

        // Events
        bool event_window_key_down(const CEGUI::EventArgs& event);
        bool event_window_deactivated(const CEGUI::EventArgs& event);
        bool event_window_close_button(const CEGUI::EventArgs& event);
        bool event_editbox_text_changed(const CEGUI::EventArgs& event);
        bool event_listbox_selection_changed(const CEGUI::EventArgs& event);
        bool event_listbox_double_click(const CEGUI::EventArgs& event);
};

#endif
