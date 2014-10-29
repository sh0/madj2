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
};

#endif
