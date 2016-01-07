/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_DEVICE
#define H_CONTROLLER_DEVICE

// Internal
#include "config.hpp"
#include "timer.hpp"

// Device interface
class c_controller_device
{
    public:
        // Destructor
        virtual ~c_controller_device() { }

        // Dispatch
        virtual void dispatch_input(c_time_cyclic& timer) { }
        virtual void dispatch_render(c_time_cyclic& timer) { }
};

#endif
