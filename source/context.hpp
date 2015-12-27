/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTEXT
#define H_CONTEXT

// Internal
#include "config.hpp"
#include "timer.hpp"
#include "global.hpp"

// C++
#include <atomic>

// Context class
class c_context : boost::noncopyable
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
