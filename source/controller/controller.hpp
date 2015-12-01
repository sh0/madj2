/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_CONTROLLER
#define H_CONTROLLER_CONTROLLER

// Internal
#include "config.hpp"
#include "controller/midi.hpp"

// Main controller class
class c_controller : boost::noncopyable
{
    public:
        // Constructor
        c_controller();

        // MIDI
        void midi_add(std::string name, std::string color, std::string device) {
            m_midi.push_back(std::make_shared<c_controller_midi>(device));
        }

        // Input
        void input_keyboard(std::string key, bool value, bool mod_ctrl, bool mod_shift, bool mod_alt, bool mod_gui);
        void input_midi_key(std::string key, bool value);
        void input_midi_pot(std::string key, float value);

        // Dispatch
        void dispatch() {
            for (auto& midi : m_midi)
                midi->dispatch();
        }

    private:
        // MIDI
        std::vector<std::shared_ptr<c_controller_midi>> m_midi;
};

#endif
