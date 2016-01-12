/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_AUDIO_SOURCE
#define H_AUDIO_SOURCE

// Internal
#include "config.hpp"
#include "timer.hpp"

// C++
#include <mutex>

// PortAudio
#include <portaudio.h>

// Audio source class
class c_audio_source : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_audio_source();
        ~c_audio_source();

        // Parameters
        uint32_t samplerate() { return m_samplerate; }
        uint32_t buffersize() { return m_buffersize; }

        // Read
        bool read(void* buffer, uint32_t frames, int64_t& timestamp);

    private:
        // Initialization
        static std::once_flag m_pa_initialize_once;

        // Stream
        PaStream* m_stream;

        // Parameters
        uint32_t m_samplerate;
        uint32_t m_buffersize;
        int64_t m_latency;
};

#endif
