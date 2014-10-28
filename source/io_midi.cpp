/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "io_midi.h"

// Constructor and destructor
c_io_midi::c_io_midi(std::string device) :
    // Info
    m_device(device),
    // Devices
    m_midi_input(nullptr), m_midi_output(nullptr)
{
    // Open device
    int status = snd_rawmidi_open(&m_midi_input, nullptr, m_device.c_str(), SND_RAWMIDI_NONBLOCK);
    if (status < 0)
        throw c_exception("Midi: Unable to open input device!", { throw_format("device", device), throw_format("error", snd_strerror(status)) });
    status = snd_rawmidi_open(nullptr, &m_midi_output, m_device.c_str(), SND_RAWMIDI_APPEND);
    if (status < 0)
        throw c_exception("Midi: Unable to open output device!", { throw_format("device", device), throw_format("error", snd_strerror(status)) });
}

c_io_midi::~c_io_midi()
{
    // Close devices
    snd_rawmidi_drain(m_midi_input);
    snd_rawmidi_close(m_midi_input);
    snd_rawmidi_drain(m_midi_output);
    snd_rawmidi_close(m_midi_output);
}

// Dispatch
void c_io_midi::dispatch()
{

}
