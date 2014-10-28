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
};

#endif
