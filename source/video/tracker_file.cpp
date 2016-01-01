/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/tracker_file.hpp"

// Constructor and destructor
c_video_tracker_file::c_video_tracker_file(CEGUI::Window* root, std::string name, std::shared_ptr<c_media_work> work) :
    // Window
    m_root(root),
    m_window(nullptr),
    // Widgets
    m_editbox(nullptr),
    m_listbox(nullptr),
    // Work
    m_work(work)
{
    // Window
    m_window = dynamic_cast<CEGUI::FrameWindow*>(CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-file.layout"));
    m_window->setName(name + "_File");
    m_window->setText("File (" + name + ")");
    m_window->setVisible(false);
    m_window->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&c_video_tracker_file::event_window_key_down, this));
    m_window->subscribeEvent(CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&c_video_tracker_file::event_window_deactivated, this));
    m_window->getCloseButton()->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&c_video_tracker_file::event_window_close_button, this));
    root->addChild(m_window);

    // Widgets
    m_editbox = dynamic_cast<CEGUI::Editbox*>(m_window->getChildRecursive("Editbox"));
    m_editbox->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&c_video_tracker_file::event_editbox_text_changed, this));
    m_editbox->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&c_video_tracker_file::event_window_key_down, this));
    m_listbox = dynamic_cast<CEGUI::Listbox*>(m_window->getChildRecursive("Listbox"));
    m_listbox->setMultiselectEnabled(false);
    m_listbox->subscribeEvent(CEGUI::Listbox::EventSelectionChanged, CEGUI::Event::Subscriber(&c_video_tracker_file::event_listbox_selection_changed, this));
    //m_listbox->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&c_video_tracker_file::event_window_key_down, this));

    // Files
    #if 0
    const auto& files = c_global::media->media_files();
    std::cout << "Files: " << files.size() << std::endl;
    for (size_t i = 0; i < files.size(); i++) {
        CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(files[i].native(), i);
        item->setAutoDeleted(true);
        m_listbox->addItem(item);
    }
    #else
    for (size_t i = 0; i < 100; i++) {
        CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem("blablabal balbablasdfa sdfasf", i);
        //item->setTextColours(CEGUI::Colour(0, 0, 0));
        item->setSelectionColours(CEGUI::Colour(1, 0, 0));
        m_listbox->addItem(item);
        m_listbox->ensureItemIsVisible(item);
    }
    #endif

    // Events
    //m_slider->subscribeEvent(CEGUI::Slider::EventThumbTrackStarted, CEGUI::Event::Subscriber(&c_video_tracker_file::event_thumb_track_started, this));
    //m_slider->subscribeEvent(CEGUI::Slider::EventThumbTrackEnded, CEGUI::Event::Subscriber(&c_video_tracker_file::event_thumb_track_ended, this));
}

c_video_tracker_file::~c_video_tracker_file()
{
    // Window
    //m_root->destroyChild(m_window);
    m_root = nullptr;

    m_window->removeAllEvents();
    m_window = nullptr;
}

// Dispatch
void c_video_tracker_file::dispatch(c_time_cyclic& timer)
{

}

// Operations
void c_video_tracker_file::show()
{
    // Window
    m_window->setVisible(true);
    m_window->moveToFront();
    m_editbox->activate();
}

// Events
bool c_video_tracker_file::event_window_key_down(const CEGUI::EventArgs& event)
{
    const CEGUI::KeyEventArgs& keys = dynamic_cast<const CEGUI::KeyEventArgs&>(event);
    if (keys.scancode == CEGUI::Key::Scan::Return) {
        // Select file
        m_window->setVisible(false);
        return true;

    } else if (keys.scancode == CEGUI::Key::Scan::ArrowDown && m_listbox->getItemCount() > 0) {
        // Next item
        CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
        if (item) {
            size_t index = std::min<size_t>(m_listbox->getItemIndex(item) + 1, m_listbox->getItemCount() - 1);
            m_listbox->clearAllSelections();
            m_listbox->setItemSelectState(index, true);
        } else {
            m_listbox->setItemSelectState(static_cast<size_t>(0), true);
        }
        return true;

    } else if (keys.scancode == CEGUI::Key::Scan::ArrowUp && m_listbox->getItemCount() > 0) {
        // Previous item
        CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
        if (item) {
            int index = std::max<int>(m_listbox->getItemIndex(item) - 1, 0);
            m_listbox->clearAllSelections();
            m_listbox->setItemSelectState(index, true);
        } else {
            m_listbox->setItemSelectState(static_cast<size_t>(0), true);
        }
        return true;

    } else if (keys.scancode == CEGUI::Key::Scan::PageDown && m_listbox->getItemCount() > 0) {
        // Next page
        CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
        if (item) {
            size_t index = std::min<size_t>(m_listbox->getItemIndex(item) + 10, m_listbox->getItemCount() - 1);
            m_listbox->clearAllSelections();
            m_listbox->setItemSelectState(index, true);
        } else {
            m_listbox->setItemSelectState(static_cast<size_t>(0), true);
        }
        return true;

    } else if (keys.scancode == CEGUI::Key::Scan::PageUp && m_listbox->getItemCount() > 0) {
        // Previous page
        CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
        if (item) {
            int index = std::max<int>(m_listbox->getItemIndex(item) - 10, 0);
            m_listbox->clearAllSelections();
            m_listbox->setItemSelectState(index, true);
        } else {
            m_listbox->setItemSelectState(static_cast<size_t>(0), true);
        }
        return true;
    }

    // Not handled
    return false;
}

bool c_video_tracker_file::event_window_deactivated(const CEGUI::EventArgs& event)
{
    m_window->setVisible(false);
    return true;
}

bool c_video_tracker_file::event_window_close_button(const CEGUI::EventArgs& event)
{
    m_window->setVisible(false);
    return true;
}

bool c_video_tracker_file::event_editbox_text_changed(const CEGUI::EventArgs& event)
{
    std::cout << "text: " << m_editbox->getText() << std::endl;
    return true;
}

bool c_video_tracker_file::event_listbox_selection_changed(const CEGUI::EventArgs& event)
{
    CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
    std::cout << "changed: " << (item ? static_cast<int>(item->getID()) : -1) << std::endl;
    return true;
}
