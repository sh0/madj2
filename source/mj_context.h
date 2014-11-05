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
};

#endif
