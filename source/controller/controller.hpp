/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_CONTROLLER
#define H_CONTROLLER_CONTROLLER

// Internal
#include "config.hpp"
#include "timer.hpp"
#include "controller/tempo.hpp"
#include "controller/device.hpp"

// C++
#include <map>

// Main controller class
class c_controller : boost::noncopyable
{
    public:
        // Constructor
        c_controller();

        // Input
        bool input_keyboard(std::string key, bool value);
        void input_midi_key(std::string key, bool value);
        void input_midi_pot(std::string key, float value);

        // Mappings
        void mapping_add(std::string device, std::vector<std::string> keys, std::string target, std::string action);

        // Tempos
        std::vector<std::shared_ptr<c_controller_tempo>> tempos() { return m_tempos; }

        // Dispatch
        void dispatch_input(c_time_cyclic& timer) {
            for (auto& device : m_devices)
                device->dispatch_input(timer);
        }

        void dispatch_render(c_time_cyclic& timer) {
            for (auto& device : m_devices)
                device->dispatch_render(timer);
        }

    private:
        // Devices
        std::vector<std::shared_ptr<c_controller_device>> m_devices;

        // Mappings
        struct s_mapping {
            // Mapping
            std::string device;
            std::vector<std::string> keys;
            std::string target;
            std::string action;

            // State
            bool active;

            // Helpers
            bool has_key(std::string key) {
                return (std::find(keys.begin(), keys.end(), key) != keys.end());
            }

            // Constructor
            s_mapping() : active(false) { }
        };
        std::vector<s_mapping> m_mappings;

        // State
        class c_state_keyboard {
            public:
                bool get(std::string key) {
                    auto it = m_state.find(key);
                    if (it != m_state.end())
                        return it->second;
                    return false;
                }

                void set(std::string key, bool value) {
                    m_state[key] = value;
                }

            private:
                std::map<std::string, bool> m_state;
        };
        c_state_keyboard m_state_keyboard;

        // Tempo
        std::vector<std::shared_ptr<c_controller_tempo>> m_tempos;
};

#endif
