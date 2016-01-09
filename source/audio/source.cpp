/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "audio/source.hpp"

// Initialization
std::once_flag c_audio_source::m_pa_initialize_once;

// Constructor and destructor
c_audio_source::c_audio_source() :
    // Stream
    m_stream(nullptr)
{
    // Initialize
    std::call_once(m_pa_initialize_once, [](){
        // Library initialization
        PaError err = Pa_Initialize();
        if (err != paNoError)
            throw c_exception("Audio: Failed to initialize PortAudio!", { throw_format("error", Pa_GetErrorText(err)) });

        // Terminating
        std::atexit([](){
            PaError err = Pa_Terminate();
            if (err != paNoError)
                std::cout << boost::format("Audio: Failed to terminate PortAudio! error = \"%s\"") % Pa_GetErrorText(err) << std::endl;
        });
    });

    // Stream parameters
    PaStreamParameters parameters;
    parameters.device = Pa_GetDefaultInputDevice();
    if (parameters.device == paNoDevice)
        throw c_exception("Audio: No input device found!");
    parameters.channelCount = 1;
    parameters.sampleFormat = paFloat32;
    parameters.suggestedLatency = Pa_GetDeviceInfo(parameters.device)->defaultLowInputLatency;
    parameters.hostApiSpecificStreamInfo = nullptr;

    // Open stream
    m_samplerate = 44100;
    m_buffersize = 1024;
    PaError err = Pa_OpenStream(&m_stream, &parameters, nullptr, m_samplerate, m_buffersize, paClipOff, nullptr, this);
    if (err != paNoError)
        throw c_exception("Audio: Failed to open stream!", { throw_format("error", Pa_GetErrorText(err)) });

    // Stream info
    auto info = Pa_GetStreamInfo(m_stream);
    //std::cout << "Latency: " << info->inputLatency << std::endl;
    m_latency = g_time_sec2us(info->inputLatency);

    // Start stream
    err = Pa_StartStream(m_stream);
    if (err != paNoError)
        throw c_exception("Audio: Failed to start stream!", { throw_format("error", Pa_GetErrorText(err)) });

    // Stream time
    //Pa_GetStreamTime(m_stream);
}

c_audio_source::~c_audio_source()
{
    // Stream
    if (m_stream)
        Pa_CloseStream(m_stream);
}

// Read
bool c_audio_source::read(void* buffer, uint32_t frames, int64_t& timestamp)
{
    // Read
    PaError err = Pa_ReadStream(m_stream, buffer, frames);
    if (err != paNoError && err != paInputOverflowed) {
        std::cout << boost::format("Audio: Read error! %s") % Pa_GetErrorText(err) << std::endl;
        return false;
    }

    // Timestamp
    timestamp = g_time_now_us() - m_latency;

    // Success
    return true;
}
