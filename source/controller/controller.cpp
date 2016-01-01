/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/controller.hpp"
#include "controller/ohmrgb.hpp"
#include "global.hpp"
#include "video/video.hpp"

// Portmidi
#include <portmidi.h>

// Constructor
c_controller::c_controller()
{
    // Tempo
    for (int i = 0; i < 2; i++)
        m_tempos.push_back(std::make_shared<c_controller_tempo>());

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
bool c_controller::input_keyboard(std::string key, bool value, bool mod_ctrl, bool mod_shift, bool mod_alt, bool mod_gui)
{
    // Only press events
    if (!value)
        return false;

    // Find mapping
    bool handled = false;
    for (auto& map : m_mappings) {
        // Filter
        if (map.device != "key")
            continue;
        if (!map.has_key(key))
            continue;
        if (mod_ctrl && !map.has_key("ctrl"))
            continue;
        if (mod_shift && !map.has_key("shift"))
            continue;
        if (mod_alt && !map.has_key("alt"))
            continue;
        if (mod_gui && !map.has_key("gui"))
            continue;

        // Exectute
        for (auto& tracker : c_global::video->tracker_list()) {
            if (tracker->name() == map.target)
                handled = (tracker->event_action(map.action) ? true : handled);
        }
    }
    return handled;
}

void c_controller::input_midi_key(std::string key, bool value)
{

}

void c_controller::input_midi_pot(std::string key, float value)
{

}

// Mappings
void c_controller::mapping_add(std::string device, std::vector<std::string> keys, std::string target, std::string action)
{
    s_mapping map;
    map.device = device;
    map.keys = keys;
    map.target = target;
    map.action = action;
    m_mappings.push_back(map);
}
