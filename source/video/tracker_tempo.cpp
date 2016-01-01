/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "global.hpp"
#include "video/tracker_tempo.hpp"

// Constructor and destructor
c_video_tracker_tempo::c_video_tracker_tempo(CEGUI::Window* root)
{
    // Window
    m_window = CEGUI::WindowManager::getSingletonPtr()->loadLayoutFromFile("tracker-tempo.layout");
    root->addChild(m_window);

    // Widgets
    m_button[0] = m_window->getChildRecursive("Button1");
    m_button[1] = m_window->getChildRecursive("Button2");
    m_progress[0] = dynamic_cast<CEGUI::ProgressBar*>(m_window->getChildRecursive("Progress1"));
    m_progress[1] = dynamic_cast<CEGUI::ProgressBar*>(m_window->getChildRecursive("Progress2"));
}

c_video_tracker_tempo::~c_video_tracker_tempo()
{
    // Window
    m_window->removeAllEvents();
    //CEGUI::WindowManager::getSingletonPtr()->destroyWindow(m_window);
}

// Dispatch
void c_video_tracker_tempo::dispatch(c_time_cyclic& timer)
{
    // Tempos
    auto tempos = c_global::controller->tempos();
    for (int i = 0; i < std::min<int>(2, tempos.size()); i++) {
        auto& tempo = tempos[i];

        // State
        if (tempo->define_state())
            m_button[i]->setText("D");
        else if (tempo->freeform_state())
            m_button[i]->setText("F");
        else if (tempo->tempo_state())
            m_button[i]->setText("T");
        else
            m_button[i]->setText("C");

        // Tempo
        if (tempo->tempo_state())
            m_progress[i]->setProgress(tempo->tempo_value_at_timepoint(timer.time_this()));
        else
            m_progress[i]->setProgress(0);
    }
}
