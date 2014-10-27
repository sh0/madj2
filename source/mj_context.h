/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MJ_CONTEXT
#define H_MJ_CONTEXT

// Internal
#include "mj_config.h"
#include "mj_time.h"
#include "mj_global.h"

// C++
#include <atomic>

// Context class
class c_context
{
    public:
        // Constructor and destructor
        c_context(std::string config_fn);
        ~c_context();

        // Running
        void run();
        void kill() { m_run = false; }

    private:
        // Main thread
        std::atomic_bool m_run;

        // Config functions
        bool config_media(
            std::vector<std::string> path_list
        );
        bool config_soundcard(
            std::string driver,
            std::string device,
            std::vector<std::string> channels
        );
        bool config_screen(
            std::string name,
            int32_t screen,
            int32_t view_cols,
            int32_t view_rows,
            int32_t width,
            int32_t height,
            bool fullscreen,
            std::string color
        );
        bool config_workspace_tracker(
            std::string name,
            std::string screen,
            int32_t slot,
            std::vector<std::string> audio
        );
        bool config_workspace_twitter(
            std::string name,
            std::string screen,
            int32_t slot
        );
        bool config_joystick(
            std::string name,
            std::string dev,
            std::string color,
            std::vector<std::string> buttons,
            std::vector<std::string> axes
        );
        bool config_key(
            std::vector<std::string> key
        );
};

#endif
