/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "audio/tempodetect.hpp"

// Constructor and destructor
c_audio_tempodetect::c_audio_tempodetect()
{
    // Source
    m_source = std::make_shared<c_audio_source>();

    // Detector
    uint32_t samplerate = m_source->samplerate();
    uint32_t win_size = 1024;
    uint32_t hop_size = win_size / 4;
    char_t* method = const_cast<char_t*>("default");
    m_tempo = std::shared_ptr<aubio_tempo_t>(new_aubio_tempo(method, win_size, hop_size, samplerate), del_aubio_tempo);

    // Thresholds
    auto threshold_silence = aubio_tempo_get_silence(m_tempo.get());
    auto threshold_peak = aubio_tempo_get_threshold(m_tempo.get());
    //std::cout << boost::format("Audio: threshold_silence = %f, threshold_peak = %f") % threshold_silence % threshold_peak << std::endl;
    //aubio_tempo_set_threshold(m_tempo.get(), 0.15f);

    // Buffers
    uint32_t buffer_size = m_source->buffersize();
    m_buffer_src = std::shared_ptr<fvec_t>(new_fvec(buffer_size), del_fvec);
    m_buffer_dst = std::shared_ptr<fvec_t>(new_fvec(2), del_fvec);

    // Thread
    m_run = true;
    m_thread = std::thread(&c_audio_tempodetect::thread, this);
}

c_audio_tempodetect::~c_audio_tempodetect()
{
    // Thread
    m_run = false;
    m_thread.join();

    // Source
    m_source.reset();

    // Detector
    m_tempo.reset();
}

// Thread
void c_audio_tempodetect::thread()
{
    while (m_run) {
        // Source
        int64_t timestamp = 0;
        if (!m_source->read(m_buffer_src->data, m_buffer_src->length, timestamp))
            return;

        // Detector
        //std::cout << "." << std::flush;
        aubio_tempo_do(m_tempo.get(), m_buffer_src.get(), m_buffer_dst.get());
        if (m_buffer_dst->data[0] != 0) {
            int64_t frame = aubio_tempo_get_last(m_tempo.get());
            int64_t offset = timestamp + (frame * 1000000 / m_source->samplerate());

            //std::cout << "Beat at " << offset << std::endl;
            //std::cout << "x" << std::flush;
        }
    }
}
