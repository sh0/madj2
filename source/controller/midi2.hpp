/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_MIDI
#define H_CONTROLLER_MIDI

// Internal
#include "config.hpp"

// C++
#include <array>

// Portmidi
#include <portmidi.h>

// MIDI controller class
class c_controller_midi : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_controller_midi(PmDeviceID input, PmDeviceID output);
        ~c_controller_midi();

        // Dispatch
        void dispatch();

    private:
        // Devices
        PmDeviceID m_input_device;
        PortMidiStream* m_input_stream;
        PmDeviceID m_output_device;
        PortMidiStream* m_output_stream;

        // Buffers
        std::array<PmEvent, 1024> m_input_buffer;

        /*
        // Opcodes
        uint8_t m_last_opcode;

        // Receive buffer
        class c_buffer_recv : boost::noncopyable
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
        c_buffer_recv m_buffer_recv;

        // Send buffer
        class c_buffer_send : boost::noncopyable
        {
            public:
                // Reading
                uint8_t* read_data() { return m_data.data(); }
                size_t read_size() { return m_data.size(); }
                void read_pop(size_t size) {
                    assert(size <= m_data.size());
                    memcpy(&m_data[0], &m_data[size], m_data.size() - size);
                    m_data.resize(m_data.size() - size);
                }

                // Writing
                void write(std::vector<uint8_t>& data) {
                    m_data.insert(m_data.end(), data.begin(), data.end());
                }
            private:
                std::vector<uint8_t> m_data;
        };
        c_buffer_send m_buffer_send;
        */

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
};

#endif
