/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "audio/source.hpp"

// Initialization
std::once_flag c_audio_source::m_pa_initialize_once;

// Constructor and destructor
c_audio_source::c_audio_source()
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


}

c_audio_source::~c_audio_source()
{

}
