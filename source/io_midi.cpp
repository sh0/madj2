/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_global.h"
#include "io_main.h"
#include "io_midi.h"

// Constructor and destructor
c_io_midi::c_io_midi(std::string device) :
    // Info
    m_device(device),
    // Devices
    m_midi_input(nullptr), m_midi_output(nullptr)
{
    // Open device
    int ret = snd_rawmidi_open(&m_midi_input, nullptr, m_device.c_str(), SND_RAWMIDI_NONBLOCK);
    if (ret < 0)
        throw c_exception("Midi: Unable to open input device!", { throw_format("device", device), throw_format("error", snd_strerror(ret)) });
    ret = snd_rawmidi_open(nullptr, &m_midi_output, m_device.c_str(), SND_RAWMIDI_APPEND);
    if (ret < 0)
        throw c_exception("Midi: Unable to open output device!", { throw_format("device", device), throw_format("error", snd_strerror(ret)) });
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
    // Fill buffer
    int ret = snd_rawmidi_read(m_midi_input, m_buffer.write_data(), m_buffer.write_size());
    if (ret > 0)
        m_buffer.write_push(ret);

    // Process
    while (true) {
        int size = read_op1(m_buffer.read_data(), m_buffer.read_size());
        if (size > 0) {
            m_buffer.read_pop(size);
        } else {
            if (size < 0)
                m_buffer.read_pop(m_buffer.read_size());
            return;
        }
    }
}

// Read message opcodes
int c_io_midi::read_op1(uint8_t* data, size_t size)
{
    // Check
    if (size == 0)
        return 0;

    // Find operation
    switch (data[0] & 0xf0) {
        case MIDI_CMD_NOTE_OFF:
            // Note off
            if (size < 3)
                return 0;
            read_note_off(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_NOTE_ON:
            // Note on
            if (size < 3)
                return 0;
            read_note_on(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_NOTE_PRESSURE:
            // Key pressure
            if (size < 3)
                return 0;
            read_note_pressure(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_CONTROL:
            // Control change
            if (size < 3)
                return 0;
            read_control(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_PGM_CHANGE:
            // Program change
            if (size < 2)
                return 0;
            read_program_change(data[0] & 0x0f, data[1]);
            return 2;
        case MIDI_CMD_CHANNEL_PRESSURE:
            // Channel pressure
            if (size < 2)
                return 0;
            read_channel_pressure(data[0] & 0x0f, data[1]);
            return 2;
        case MIDI_CMD_BENDER:
            // Pitch bender
            if (size < 3)
                return 0;
            read_bender(data[0] & 0x0f, static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 7));
            return 3;
        case MIDI_CMD_COMMON_SYSEX:
            // System messages
            return read_op2(data, size);
        default:
            // Unknown opcode
            return -1;
    }
}

int c_io_midi::read_op2(uint8_t* data, size_t size)
{
    // Check
    if (size == 0)
        return 0;

    // Find operation
    switch (data[0]) {
        case MIDI_CMD_COMMON_SYSEX:
            // System-exclusive message
            {
                size_t offset = 0;
                while (offset < size && data[offset] != 0xf7)
                    offset++;
                if (offset >= size)
                    return 0;
                read_sysex(data + 1, offset - 1);
                return offset + 1;
            }
        case MIDI_CMD_COMMON_MTC_QUARTER:
            // MTC quarter frame
            if (size < 2)
                return 0;
            read_common_mtc_quarter(data[1] >> 4, data[1] & 0x0f);
            return 1;
        case MIDI_CMD_COMMON_SONG_POS:
            // Song position
            if (size < 3)
                return 0;
            read_common_song_pos(static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 7));
            return 3;
        case MIDI_CMD_COMMON_SONG_SELECT:
            // Song selection
            if (size < 2)
                return 0;
            read_common_song_select(data[1]);
            return 2;
        case MIDI_CMD_COMMON_TUNE_REQUEST:
            // Tune request
            read_common_tune_request();
            return 1;
        case MIDI_CMD_COMMON_CLOCK:
            // Start clock
            read_realtime_clock();
            return 1;
        case MIDI_CMD_COMMON_START:
            // Start
            read_realtime_start();
            return 1;
        case MIDI_CMD_COMMON_CONTINUE:
            // Continue
            read_realtime_continue();
            return 1;
        case MIDI_CMD_COMMON_STOP:
            // Stop
            read_realtime_stop();
            return 1;
        case MIDI_CMD_COMMON_SENSING:
            // Active sensing
            read_realtime_sensing();
            return 1;
        case MIDI_CMD_COMMON_RESET:
            // Reset
            read_realtime_reset();
            return 1;
        default:
            // Unknown opcode
            return -1;
    }
}

// Read channel messages
void c_io_midi::read_note_off(int channel, int key, int velocity)
{
    if (m_channel[channel].note[key] != false) {
        m_channel[channel].note[key] = false;
        c_global::io->input_midi_key(boost::str(boost::format("c%dn%d") % channel % key), false);
    }
    if (m_channel[channel].velocity[key] != velocity) {
        m_channel[channel].velocity[key] = velocity;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dv%d") % channel % key), static_cast<float>(velocity) / 127.0f);
    }
}

void c_io_midi::read_note_on(int channel, int key, int velocity)
{
    if (m_channel[channel].note[key] != true) {
        m_channel[channel].note[key] = true;
        c_global::io->input_midi_key(boost::str(boost::format("c%dn%d") % channel % key), true);
    }
    if (m_channel[channel].velocity[key] != velocity) {
        m_channel[channel].velocity[key] = velocity;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dv%d") % channel % key), static_cast<float>(velocity) / 127.0f);
    }
}

void c_io_midi::read_note_pressure(int channel, int key, int pressure)
{
    if (m_channel[channel].pressure[key] != pressure) {
        m_channel[channel].pressure[key] = pressure;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dp%d") % channel % key), static_cast<float>(pressure) / 127.0f);
    }
}

void c_io_midi::read_control(int channel, int id, int value)
{
    if (m_channel[channel].control[id] != value) {
        m_channel[channel].control[id] = value;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dc%d") % channel % id), static_cast<float>(value) / 127.0f);
    }
}

void c_io_midi::read_program_change(int channel, int program)
{
    if (m_channel[channel].ch_program != program) {
        m_channel[channel].ch_program = program;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dprogram") % channel), static_cast<float>(program) / 127.0f);
    }
}

void c_io_midi::read_channel_pressure(int channel, int pressure)
{
    if (m_channel[channel].ch_pressure != pressure) {
        m_channel[channel].ch_pressure = pressure;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dpressure") % channel), static_cast<float>(pressure) / 127.0f);
    }
}

void c_io_midi::read_bender(int channel, int value)
{
    if (m_channel[channel].ch_bender != value) {
        m_channel[channel].ch_bender = value;
        c_global::io->input_midi_pot(boost::str(boost::format("c%dbender") % channel), static_cast<float>(value) / 127.0f);
    }
}

// Read system exclusive messages
void c_io_midi::read_sysex(uint8_t* data, size_t size)
{
    if (size >= 5 && data[0] == 0x00 && data[1] == 0x01 && data[2] == 0x61)
        read_sysex_ohmrgb(data[4], data[3], data + 5, size - 5);
}

void c_io_midi::read_sysex_ohmrgb(int op, int device, uint8_t* data, size_t size)
{
    if (op == 0x7e) {
        // NAK - Negative acknowledge

    } else if (op == 0x7f) {
        // ACK - Positive acknowledge

    }
}

// Read system common messages
void c_io_midi::read_common_mtc_quarter(int id, int value)
{

}

void c_io_midi::read_common_song_pos(int id)
{

}

void c_io_midi::read_common_song_select(int id)
{

}

void c_io_midi::read_common_tune_request()
{

}

// Read system real-time messages
void c_io_midi::read_realtime_clock()
{

}

void c_io_midi::read_realtime_start()
{

}

void c_io_midi::read_realtime_continue()
{

}

void c_io_midi::read_realtime_stop()
{

}

void c_io_midi::read_realtime_sensing()
{

}

void c_io_midi::read_realtime_reset()
{

}

// Write system exclusive messages
void c_io_midi::write_sysex(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw;
    raw.push_back(0xf0);
    raw.insert(raw.end(), msg.begin(), msg.end());
    raw.push_back(0xf7);

    // Send
    int ret = snd_rawmidi_write(m_midi_output, raw.data(), raw.size());
    if (ret < static_cast<int>(raw.size()))
        std::cout << boost::format("Midi: Failed to send message! size=%d, error=%s") % raw.size() % snd_strerror(ret) << std::endl;
}

void c_io_midi::write_sysex_ohmrgb(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw{ 0x00, 0x01, 0x61, 0x07 };
    raw.insert(raw.end(), msg.begin(), msg.end());

    // Send
    write_sysex(raw);
}

void c_io_midi::write_sysex_ohmrgb_set_leds(std::vector<uint8_t> leds)
{
    // Message
    std::vector<uint8_t> raw(43);
    raw[0] = 0x04;
    for (size_t i = 1; i < raw.size(); i++)
        raw[i] = 0;

    // Leds
    if (leds.size() > 82)
        return;
    for (size_t i = 0; i < (leds.size() + 1) / 2; i++) {
        if (2 * i + 1 < leds.size()) {
            raw[1 + i] = (leds[i] | (leds[i + 1] << 3));
        } else {
            raw[1 + i] = leds[i];
        }
    }

    // Send
    write_sysex_ohmrgb(raw);
}
