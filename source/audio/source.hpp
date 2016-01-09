/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_AUDIO_SOURCE
#define H_AUDIO_SOURCE

// Internal
#include "config.hpp"

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
        int samplerate() { return 44100; }

    private:
        // Initialization
        static std::once_flag m_pa_initialize_once;
};

#endif
