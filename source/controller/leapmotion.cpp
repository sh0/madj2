/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/leapmotion.hpp"

// Checks
#ifdef LEAPMOTION_FOUND

// Constructor and destructor
c_controller_leapmotion::c_controller_leapmotion()
{
    // Listener
    m_controller.addListener(*this);
}

c_controller_leapmotion::~c_controller_leapmotion()
{
    // Listener
    m_controller.removeListener(*this);
}

// Devices
std::vector<std::shared_ptr<c_controller_device>> c_controller_leapmotion::devices()
{
    std::vector<std::shared_ptr<c_controller_device>> devices;
    devices.push_back(std::make_shared<c_controller_leapmotion>());
    return devices;
}

// Dispatch
void c_controller_leapmotion::dispatch_input(c_time_cyclic& timer)
{
    // Check
    if (!m_controller.isConnected())
        return;

    // Frame
    auto frame = m_controller.frame();
    auto hands = frame.hands();
}

#endif
