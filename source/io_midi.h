/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_IO_MIDI
#define H_IO_MIDI

// Internal
#include "mj_config.h"

// C++
#include <array>

// ALSA
#include <alsa/asoundlib.h>

// MIDI I/O class
class c_io_midi : c_noncopiable
{
    public:
        // Constructor and destructor
        c_io_midi(std::string device);
        ~c_io_midi();

        // Dispatch
        void dispatch();

    private:
        // Info
        std::string m_device;

        // Devices
        snd_rawmidi_t* m_midi_input;
        snd_rawmidi_t* m_midi_output;

        // Opcodes
        uint8_t m_last_opcode;

        // Buffer
        class c_buffer : c_noncopiable
        {
            public:
                // Reading
                uint8_t* read_data() { return &m_buffer[0]; }
                size_t read_size() { return m_size; }
                void read_pop(size_t size) {
                    assert(size <= m_size);
                    memcpy(&m_buffer[0], &m_buffer[size], m_size - size);
                    m_size -= size;
                }

                // Writing
                uint8_t* write_data() { return &m_buffer[m_size]; }
                size_t write_size() { return m_buffer.size() - m_size; }
                void write_push(size_t size) {
                    assert(m_size + size <= m_buffer.size());
                    m_size += size;
                }

            private:
                std::array<uint8_t, 4096> m_buffer;
                size_t m_size;
        };
        c_buffer m_buffer;

        // Data
        struct s_channel {
            std::array<bool, 128> note;
            std::array<uint8_t, 128> velocity;
            std::array<uint8_t, 128> pressure;
            std::array<uint8_t, 128> control;
            int ch_program;
            int ch_pressure;
            int ch_bender;
        };
        std::array<s_channel, 16> m_channel;

        // Read message opcodes
        int read_op0(uint8_t* data, size_t size);
        int read_op1(uint8_t* data, size_t size);
        int read_op2(uint8_t* data, size_t size);

        // Read channel messages
        void read_note_off(int channel, int key, int velocity);
        void read_note_on(int channel, int key, int velocity);
        void read_note_pressure(int channel, int key, int pressure);
        void read_control(int channel, int id, int value);
        void read_program_change(int channel, int program);
        void read_channel_pressure(int channel, int pressure);
        void read_bender(int channel, int value);

        // Read system exclusive messages
        void read_sysex(uint8_t* data, size_t size);
        void read_sysex_ohmrgb(int op, int device, uint8_t* data, size_t size);

        // Read system common messages
        void read_common_mtc_quarter(int id, int value);
        void read_common_song_pos(int id);
        void read_common_song_select(int id);
        void read_common_tune_request();

        // Read system real-time messages
        void read_realtime_clock();
        void read_realtime_start();
        void read_realtime_continue();
        void read_realtime_stop();
        void read_realtime_sensing();
        void read_realtime_reset();

        // Write system exclusive messages
        void write_sysex(std::vector<uint8_t> msg);
        void write_sysex_ohmrgb(std::vector<uint8_t> msg);
        void write_sysex_ohmrgb_save_settings() { write_sysex_ohmrgb({ 0x02 }); }
        void write_sysex_ohmrgb_set_leds(std::vector<uint8_t> leds);
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
