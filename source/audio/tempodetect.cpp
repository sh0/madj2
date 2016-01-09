/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "audio/tempodetect.hpp"

// Constructor and destructor
c_audio_tempodetect::c_audio_tempodetect()
{
    // Device

    // Detector
    uint32_t samplerate = 44100;
    uint32_t win_size = 1024;
    uint32_t hop_size = win_size / 4;
    char_t* method = const_cast<char_t*>("default");
    m_tempo = std::shared_ptr<aubio_tempo_t>(new_aubio_tempo(method, win_size, hop_size, samplerate), del_aubio_tempo);

    // Buffers
    uint32_t buffer_size = 4096;
    m_buffer_src = std::shared_ptr<fvec_t>(new_fvec(buffer_size), del_fvec);
    m_buffer_dst = std::shared_ptr<fvec_t>(new_fvec(2), del_fvec);
}

c_audio_tempodetect::~c_audio_tempodetect()
{
    // Detector
    m_tempo.reset();
}

// thread
void c_audio_tempodetect::thread()
{
    while (m_run) {
        // Detector
        aubio_tempo_do(m_tempo.get(), m_buffer_src.get(), m_buffer_dst.get());
        if (m_buffer_dst->data[0] != 0) {
            int64_t frame = aubio_tempo_get_last(m_tempo.get());
        }
    }
}
