/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/controller.hpp"
#include "controller/midi.hpp"
#include "controller/leapmotion.hpp"
#include "global.hpp"
#include "video/video.hpp"

// Constructor
c_controller::c_controller()
{
    // Tempo
    for (int i = 0; i < 2; i++)
        m_tempos.push_back(std::make_shared<c_controller_tempo>());

    // MIDI
    auto midi_devices = c_controller_midi::devices();
    for (auto& device : midi_devices)
        m_devices.push_back(device);

    // Leap Motion
    #ifdef LEAPMOTION_FOUND
    auto leapmotion_devices = c_controller_leapmotion::devices();
    for (auto& device : leapmotion_devices)
        m_devices.push_back(device);
    #endif

    // Tempo detector
    m_tempodetector = std::make_shared<c_audio_tempodetect>();
}

// Input
bool c_controller::input_keyboard(std::string key, bool value)
{
    // Check if we have valid key
    if (key.empty())
        return false;

    // Find key and set new state
    bool current = m_state_keyboard.get(key);
    m_state_keyboard.set(key, value);

    // Check for state changes
    if (current == value)
        return false;

    // Find mapping
    bool handled = false;
    for (auto& map : m_mappings) {
        // Filter mappings that are relevant
        if (map.device != "key")
            continue;
        if (!map.has_key(key))
            continue;

        // Mapping state
        bool active = true;
        for (auto& mk : map.keys)
            active = (active && m_state_keyboard.get(mk));

        // Check if state has changed
        if (active != map.active) {
            // New state
            map.active = active;

            // Exectute
            if (map.target == "tempo_a" && m_tempos.size() >= 1) {
                handled = (m_tempos[0]->event_action(map.action, active) ? true : handled);
            } else if (map.target == "tempo_b" && m_tempos.size() >= 2) {
                handled = (m_tempos[1]->event_action(map.action, active) ? true : handled);
            } else {
                for (auto& tracker : c_global::video->tracker_list()) {
                    if (tracker->name() == map.target)
                        handled = (tracker->event_action(map.action, active) ? true : handled);
                }
            }
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
