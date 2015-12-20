/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_OHMRGB
#define H_CONTROLLER_OHMRGB

// Internal
#include "config.hpp"
#include "controller/midi.hpp"

// C++
#include <map>

// Livid OhmRGB controller
class c_controller_ohmrgb : public c_controller_midi //, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_controller_ohmrgb(PmDeviceID input, PmDeviceID output);
        virtual ~c_controller_ohmrgb() { }

        // Dispatch
        virtual void dispatch_render();

    protected:
        // States
        bool m_state_needs_update;
        std::array<uint8_t, 82> m_state_leds;
        std::map<std::string, uint8_t> m_state_control;

        // Leds
        void leds_set(std::string id, uint8_t value);
        void leds_update() { write_sysex_ohmrgb_set_leds(m_state_leds); }

        // Controls
        void control_set(std::string id, uint8_t value);
        uint8_t control_get(std::string id);

        // Read control messages
        virtual void read_note_on(int channel, int key, int velocity);
        virtual void read_control(int channel, int id, int value);

        // Read system exclusive messages
        virtual void read_sysex(std::vector<uint8_t> msg);
        void read_sysex_ohmrgb(int op, int device, std::vector<uint8_t> msg);
        void read_sysex_ohmrgb_request_leds(std::vector<uint8_t> msg);

        // Write control messages
        void write_backlight_dimmer(int value) { write_control(0, 0x76, value); }
        void write_logo_dimmer(int value) { write_control(0, 0x77, value); }

        // Write system exclusive messages
        void write_sysex_ohmrgb(std::vector<uint8_t> msg);
        void write_sysex_ohmrgb_save_settings() { write_sysex_ohmrgb({ 0x02 }); }
        void write_sysex_ohmrgb_set_leds(std::array<uint8_t, 82>& leds);
        void write_sysex_ohmrgb_factory_reset() { write_sysex_ohmrgb({ 0x06 }); }
        void write_sysex_ohmrgb_request_leds() { write_sysex_ohmrgb({ 0x07, 0x04 }); }
        void write_sysex_ohmrgb_request_controls() { write_sysex_ohmrgb({ 0x07, 0x06 }); }
        void write_sysex_ohmrgb_request_midi_state() { write_sysex_ohmrgb({ 0x07, 0x08 }); }
        void write_sysex_ohmrgb_request_midi_mapping(uint8_t ll, uint8_t hh) { write_sysex_ohmrgb({ 0x07, 0x09, ll, hh }); }
        void write_sysex_ohmrgb_request_analog_mapping() { write_sysex_ohmrgb({ 0x07, 0x0a }); }
        void write_sysex_ohmrgb_request_button_mapping() { write_sysex_ohmrgb({ 0x07, 0x0b }); }
        void write_sysex_ohmrgb_request_basic_channel() { write_sysex_ohmrgb({ 0x07, 0x0c }); }
        void write_sysex_ohmrgb_request_output_merge() { write_sysex_ohmrgb({ 0x07, 0x0d }); }
        void write_sysex_ohmrgb_request_crossfader() { write_sysex_ohmrgb({ 0x07, 0x0f }); }
        void write_sysex_ohmrgb_request_bank_channel() { write_sysex_ohmrgb({ 0x07, 0x16 }); }
        void write_sysex_ohmrgb_request_bank_channels() { write_sysex_ohmrgb({ 0x07, 0x17 }); }
        void write_sysex_ohmrgb_request_current_bank() { write_sysex_ohmrgb({ 0x07, 0x1a }); }
        void write_sysex_ohmrgb_request_expansion_jack_map() { write_sysex_ohmrgb({ 0x07, 0x22 }); }
        void write_sysex_ohmrgb_request_color_map() { write_sysex_ohmrgb({ 0x07, 0x23 }); }
        void write_sysex_ohmrgb_map_single_led(uint8_t ll, uint8_t hh, uint8_t cr) { write_sysex_ohmrgb({ 0x09, ll, hh, cr }); }
        //void write_sysex_ohmrgb_map_analog_inputs();
        //void write_sysex_ohmrgb_map_buttons();
        void write_sysex_ohmrgb_set_basic_channel(uint8_t channel) { write_sysex_ohmrgb({ 0x0c, channel }); }
        void write_sysex_ohmrgb_set_midi_output_merge(bool state) { write_sysex_ohmrgb({ 0x0d, static_cast<uint8_t>(state ? 0x01 : 0x00) }); }
        void write_sysex_ohmrgb_erase_input_map() { write_sysex_ohmrgb({ 0x0e }); }
        void write_sysex_ohmrgb_set_crossfader_flip(bool state) { write_sysex_ohmrgb({ 0x0f, static_cast<uint8_t>(state ? 0x01 : 0x00) }); }
        void write_sysex_ohmrgb_set_bank_channel(uint8_t channel) { write_sysex_ohmrgb({ 0x16, channel }); }
        void write_sysex_ohmrgb_set_bank_channels(uint8_t channel[4]) { write_sysex_ohmrgb({ 0x17, channel[0], channel[1], channel[2], channel[3] }); }
        void write_sysex_ohmrgb_save_current_bank_settings() { write_sysex_ohmrgb({ 0x18 }); }
        void write_sysex_ohmrgb_save_all_bank_settings() { write_sysex_ohmrgb({ 0x19 }); }
        //void write_sysex_ohmrgb_map_expansion_analog_inputs();
        //void write_sysex_ohmrgb_color_map();
        //void write_sysex_ohmrgb_map_led_all_notes();
        //void write_sysex_ohmrgb_map_led_all_ccs();
        //void write_sysex_ohmrgb_set_leds_for_all_banks();
};

#endif
