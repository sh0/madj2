/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/controller.hpp"
#include "controller/ohmrgb.hpp"

// Portmidi
#include <portmidi.h>

// Constructor
c_controller::c_controller()
{
    // MIDI initialization
    PmError midi_error = Pm_Initialize();
    if (midi_error != pmNoError)
        throw c_exception("Controller: Failed to initialize MIDI library!", { throw_format("error", Pm_GetErrorText(midi_error)) });

    // Enumerate MIDI devices
    int midi_count = Pm_CountDevices();
    std::array<int, 2> midi_ohmrgb = {{ -1, -1 }};
    for (int id = 0; id < midi_count; id++) {
        // Get MIDI device info
        const PmDeviceInfo* info = Pm_GetDeviceInfo(id);
        if (!info)
            continue;

        // Check for supported device
        std::string name = info->name;
        //std::cout << boost::format("Controller: MIDI device! name = %s, input = %d, output = %d") % name % info->input % info->output << std::endl;
        if (name == "OhmRGB MIDI 1") {
            if (info->input)
                midi_ohmrgb[0] = id;
            if (info->output)
                midi_ohmrgb[1] = id;
        }
    }

    // Create devices
    if (midi_ohmrgb[0] >= 0 || midi_ohmrgb[1] >= 0)
        m_midi.push_back(std::make_shared<c_controller_ohmrgb>(midi_ohmrgb[0], midi_ohmrgb[1]));
}

// Input
void c_controller::input_keyboard(std::string key, bool value, bool mod_ctrl, bool mod_shift, bool mod_alt, bool mod_gui)
{

}

void c_controller::input_midi_key(std::string key, bool value)
{

}

void c_controller::input_midi_pot(std::string key, float value)
{

}
