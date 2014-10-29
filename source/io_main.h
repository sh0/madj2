/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_IO_MAIN
#define H_IO_MAIN

// Internal
#include "mj_config.h"
#include "io_midi.h"

// Main I/O class
class c_io : c_noncopiable
{
    public:
        // MIDI
        void midi_add(std::string name, std::string color, std::string device) {
            m_midi.push_back(std::make_shared<c_io_midi>(device));
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
        std::vector<std::shared_ptr<c_io_midi>> m_midi;
};

#endif
