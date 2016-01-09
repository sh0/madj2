/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_LEAPMOTION
#define H_CONTROLLER_LEAPMOTION

// Internal
#include "config.hpp"
#include "controller/device.hpp"

// Checks
#ifdef LEAPMOTION_FOUND

// Leapmotion
#include "Leap.h"

class c_controller_leapmotion : public c_controller_device, protected Leap::Listener, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_controller_leapmotion();
        ~c_controller_leapmotion();

        // Devices
        static std::vector<std::shared_ptr<c_controller_device>> devices();

        // Dispatch
        virtual void dispatch_input(c_time_cyclic& timer);

    private:
        // Controller
        Leap::Controller m_controller;

        // Temporary
        double m_temp_videopos;
        float m_temp_handpos;
};

#endif
#endif
