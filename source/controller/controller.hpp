/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_CONTROLLER
#define H_CONTROLLER_CONTROLLER

// Internal
#include "config.hpp"
#include "controller/midi.hpp"
#include "controller/tempo.hpp"

// Main controller class
class c_controller : boost::noncopyable
{
    public:
        // Constructor
        c_controller();

        // Input
        void input_keyboard(std::string key, bool value, bool mod_ctrl, bool mod_shift, bool mod_alt, bool mod_gui);
        void input_midi_key(std::string key, bool value);
        void input_midi_pot(std::string key, float value);

        // Mappings
        void mapping_add(std::string device, std::vector<std::string> keys, std::string target, std::string action);

        // Dispatch
        void dispatch_input() {
            for (auto& midi : m_midi)
                midi->dispatch_input();
        }

        void dispatch_render() {
            for (auto& midi : m_midi)
                midi->dispatch_render();
        }

    private:
        // MIDI
        std::vector<std::shared_ptr<c_controller_midi>> m_midi;

        // Mappings
        struct s_mapping {
            std::string device;
            std::vector<std::string> keys;
            std::string target;
            std::string action;

            bool has_key(std::string key) {
                return (std::find(keys.begin(), keys.end(), key) != keys.end());
            }
        };
        std::vector<s_mapping> m_mappings;

        // Tempo
        std::vector<std::shared_ptr<c_controller_tempo>> m_tempo;
};

#endif
