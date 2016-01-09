/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/tracker_file.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_video_tracker_file::c_video_tracker_file(CEGUI::Window* root, std::string name, std::shared_ptr<c_media_work> work) :
    // Window
    m_root(root),
    m_window(nullptr),
    // Widgets
    m_editbox(nullptr),
    m_listbox(nullptr),
    // Work
    m_work(work),
    // Files
    m_files(c_global::media->media_files())
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
    m_listbox->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&c_video_tracker_file::event_listbox_double_click, this));

    // Files
    auto replace = [](std::string subject, const std::string& search, const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    };
    for (size_t i = 0; i < m_files.size(); i++) {
        std::string file = replace(m_files[i].native(), "[", "\\[");
        CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(file, i);
        item->setAutoDeleted(false);
        item->setSelectionBrushImage("GWEN/Input.ListBox.EvenLineSelected");
        m_items.push_back(item);
    }
    list(m_editbox->getText().c_str());
}

c_video_tracker_file::~c_video_tracker_file()
{
    // Listbox
    m_listbox->resetList();
    for (auto item : m_items)
        delete item;

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

    // Select first entry
    if (!m_listbox->getFirstSelectedItem() && m_listbox->getItemCount() > 0)
        m_listbox->setItemSelectState(static_cast<size_t>(0), true);
}

void c_video_tracker_file::load(boost::filesystem::path path)
{
    //std::cout << "Loading " << path << std::endl;
    m_work->open(path);
}

void c_video_tracker_file::list(std::string query)
{
    // Break query to filter strings
    std::vector<std::string> filters;
    boost::split(filters, query, boost::is_any_of(" \t"));
    for (auto& filter : filters) {
        boost::trim(filter);
        boost::algorithm::to_lower(filter);
    }

    // Build list
    m_listbox->resetList();
    for (size_t i = 0; i < m_items.size(); i++) {
        std::string file = m_files[i].native();
        boost::algorithm::to_lower(file);

        bool valid = true;
        for (auto& filter : filters) {
            if (file.find(filter) == std::string::npos)
                valid = false;
        }
        if (valid)
            m_listbox->addItem(m_items[i]);
    }

    // Select first entry
    m_listbox->clearAllSelections();
    if (m_listbox->getItemCount() > 0)
        m_listbox->setItemSelectState(static_cast<size_t>(0), true);
}

// Events
bool c_video_tracker_file::event_window_key_down(const CEGUI::EventArgs& event)
{
    const CEGUI::KeyEventArgs& keys = dynamic_cast<const CEGUI::KeyEventArgs&>(event);
    if (keys.scancode == CEGUI::Key::Scan::Return) {
        // Select file
        m_window->setVisible(false);
        CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
        if (item)
            load(m_files[item->getID()]);
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
    //std::cout << "text: " << m_editbox->getText() << std::endl;
    list(m_editbox->getText().c_str());
    return true;
}

bool c_video_tracker_file::event_listbox_selection_changed(const CEGUI::EventArgs& event)
{
    //CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
    //std::cout << "changed: " << (item ? static_cast<int>(item->getID()) : -1) << std::endl;
    return true;
}

bool c_video_tracker_file::event_listbox_double_click(const CEGUI::EventArgs& event)
{
    CEGUI::ListboxItem* item = m_listbox->getFirstSelectedItem();
    if (item) {
        m_window->setVisible(false);
        load(m_files[item->getID()]);
    }
    return true;
}
