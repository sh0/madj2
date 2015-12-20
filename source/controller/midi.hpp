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
#include <cstring>

// Portmidi
#include <portmidi.h>

// MIDI controller class
class c_controller_midi : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_controller_midi(PmDeviceID input, PmDeviceID output);
        virtual ~c_controller_midi();

        // Dispatch
        virtual void dispatch_input();
        virtual void dispatch_render();

    protected:
        // Devices
        PmDeviceID m_input_device;
        PortMidiStream* m_input_stream;
        PmDeviceID m_output_device;
        PortMidiStream* m_output_stream;

        // Buffers
        uint32_t m_input_size;
        std::array<PmEvent, 1024> m_input_buffer;

        // Opcodes
        uint8_t m_last_opcode;

        // Receive buffer
        class c_buffer_recv : boost::noncopyable
        {
            public:
                // Reading
                PmEvent* read_data() { return &m_buffer[0]; }
                size_t read_size() { return m_size; }
                void read_pop(size_t size) {
                    assert(size <= m_size);
                    for (size_t i = 0; i < m_size - size; i++)
                        m_buffer[i] = m_buffer[size + i];
                    m_size -= size;
                }

                // Writing
                PmEvent* write_data() { return &m_buffer[m_size]; }
                size_t write_size() { return m_buffer.size() - m_size; }
                void write_push(size_t size) {
                    assert(m_size + size <= m_buffer.size());
                    m_size += size;
                }

            private:
                std::array<PmEvent, 1024> m_buffer;
                size_t m_size;
        };
        c_buffer_recv m_buffer_recv;

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
        int read_msg(PmEvent* events, size_t size);
        virtual void read_short(PmMessage msg);
        virtual void read_sysex(std::vector<uint8_t> msg);

        // Read channel messages
        virtual void read_note_off(int channel, int key, int velocity);
        virtual void read_note_on(int channel, int key, int velocity);
        virtual void read_note_pressure(int channel, int key, int pressure);
        virtual void read_control(int channel, int id, int value);
        virtual void read_program_change(int channel, int program);
        virtual void read_channel_pressure(int channel, int pressure);
        virtual void read_bender(int channel, int value);

        // Read system common messages
        virtual void read_common_mtc_quarter(int id, int value);
        virtual void read_common_song_pos(int id);
        virtual void read_common_song_select(int id);
        virtual void read_common_tune_request();

        // Read system real-time messages
        virtual void read_realtime_clock();
        virtual void read_realtime_start();
        virtual void read_realtime_continue();
        virtual void read_realtime_stop();
        virtual void read_realtime_sensing();
        virtual void read_realtime_reset();

        // Write messages
        void write_short(PmMessage msg);
        void write_sysex(std::vector<uint8_t> msg);

        // Write channel messages
        void write_note_off(int channel, int key, int velocity);
        void write_note_on(int channel, int key, int velocity);
        void write_note_pressure(int channel, int key, int pressure);
        void write_control(int channel, int id, int value);
        void write_program_change(int channel, int program);
        void write_channel_pressure(int channel, int pressure);
        void write_bender(int channel, int value);

        // Write system common messages
        void write_common_mtc_quarter(int id, int value);
        void write_common_song_pos(int id);
        void write_common_song_select(int id);
        void write_common_tune_request();

        // Read system real-time messages
        void write_realtime_clock();
        void write_realtime_start();
        void write_realtime_continue();
        void write_realtime_stop();
        void write_realtime_sensing();
        void write_realtime_reset();
};

#endif
