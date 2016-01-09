/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_AUDIO_TEMPODETECT
#define H_AUDIO_TEMPODETECT

// Internal
#include "config.hpp"
#include "timer.hpp"
#include "audio/source.hpp"

// Aubio
#include <aubio/aubio.h>

// Tempo detection class
class c_audio_tempodetect : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_audio_tempodetect();
        ~c_audio_tempodetect();

    private:
        // Detector
        std::shared_ptr<aubio_tempo_t> m_tempo;

        // Buffers
        std::shared_ptr<fvec_t> m_buffer_src;
        std::shared_ptr<fvec_t> m_buffer_dst;

        // Source
        std::shared_ptr<c_audio_source> m_source;

        // Thread
        std::atomic_bool m_run;
        std::thread m_thread;
        std::mutex m_mutex;
        void thread();
};

#endif
