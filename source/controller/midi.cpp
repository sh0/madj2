/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "controller/controller.hpp"
#include "controller/midi.hpp"

// Boost
#include <boost/algorithm/string/join.hpp>

// MIDI commands
#define MIDI_CMD_NOTE_OFF            0x80  // note off
#define MIDI_CMD_NOTE_ON             0x90  // note on
#define MIDI_CMD_NOTE_PRESSURE       0xa0  // key pressure
#define MIDI_CMD_CONTROL             0xb0  // control change
#define MIDI_CMD_PGM_CHANGE          0xc0  // program change
#define MIDI_CMD_CHANNEL_PRESSURE    0xd0  // channel pressure
#define MIDI_CMD_BENDER              0xe0  // pitch bender
#define MIDI_CMD_COMMON_SYSEX        0xf0  // sysex (system exclusive) begin
#define MIDI_CMD_COMMON_MTC_QUARTER  0xf1  // MTC quarter frame
#define MIDI_CMD_COMMON_SONG_POS     0xf2  // song position
#define MIDI_CMD_COMMON_SONG_SELECT  0xf3  // song select
#define MIDI_CMD_COMMON_TUNE_REQUEST 0xf6  // tune request
#define MIDI_CMD_COMMON_SYSEX_END    0xf7  // end of sysex
#define MIDI_CMD_COMMON_CLOCK        0xf8  // clock
#define MIDI_CMD_COMMON_START        0xfa  // start
#define MIDI_CMD_COMMON_CONTINUE     0xfb  // continue
#define MIDI_CMD_COMMON_STOP         0xfc  // stop
#define MIDI_CMD_COMMON_SENSING      0xfe  // active sensing
#define MIDI_CMD_COMMON_RESET        0xff  // reset

// Constructor and destructor
c_controller_midi::c_controller_midi(PmDeviceID input, PmDeviceID output) :
    // Devices
    m_input_device(input), m_input_stream(nullptr),
    m_output_device(output), m_output_stream(nullptr),
    // Buffers
    m_input_size(0),
    // Opcodes
    m_last_opcode(0)
{
    // Reading
    if (m_input_device >= 0) {
        PmError ret = Pm_OpenInput(&m_input_stream, m_input_device, nullptr, m_input_buffer.size(), nullptr, nullptr);
        if (ret != pmNoError)
            throw c_exception("MIDI: Failed to initialize input device!", { throw_format("error", Pm_GetErrorText(ret)) });
    }

    // Writing
    if (m_output_device >= 0) {
        PmError ret = Pm_OpenOutput(&m_output_stream, m_output_device, nullptr, 1024, nullptr, nullptr, 0);
        if (ret != pmNoError)
            throw c_exception("MIDI: Failed to initialize output device!", { throw_format("error", Pm_GetErrorText(ret)) });
    }
}

c_controller_midi::~c_controller_midi()
{
    // Close streams
    if (m_input_stream)
        Pm_Close(m_input_stream);
    if (m_output_stream)
        Pm_Close(m_output_stream);
}

// Dispatch
void c_controller_midi::dispatch_input(c_time_cyclic& timer)
{
    if (m_input_stream) {
        while (Pm_Poll(m_input_stream)) {
            // Read data
            int ret = Pm_Read(m_input_stream, m_buffer_recv.write_data(), m_buffer_recv.write_size());

            // Debug
            /*
            if (ret > 0) {
                std::vector<std::string> list;
                for (int i = 0; i < ret; i++)
                    list.push_back(boost::str(boost::format("0x%08x") % m_buffer_recv.write_data()[i].message));
                std::cout << boost::format("MIDI: Message: %s") % boost::algorithm::join(list, ", ") << std::endl;
            }
            */

            if (ret > 0)
                m_buffer_recv.write_push(ret);

            // Process buffers
            while (m_buffer_recv.read_size() > 0) {
                int count = read_msg(m_buffer_recv.read_data(), m_buffer_recv.read_size());
                if (count > 0)
                    m_buffer_recv.read_pop(count);
                else
                    break;
            }

            // Emergency reset
            if (m_buffer_recv.read_size() > 128)
                m_buffer_recv.read_pop(m_buffer_recv.read_size());
        }
    }
}

void c_controller_midi::dispatch_render(c_time_cyclic& timer)
{

}

// Read message opcodes
int c_controller_midi::read_msg(PmEvent* events, size_t size)
{
    auto msg = events->message;
    if (Pm_MessageStatus(msg) == MIDI_CMD_COMMON_SYSEX) {
        // Compress everything to a byte stream
        std::vector<uint8_t> buf = {
            static_cast<uint8_t>((msg >> 8) & 0xff),
            static_cast<uint8_t>((msg >> 16) & 0xff),
            static_cast<uint8_t>((msg >> 24) & 0xff)
        };
        for (size_t i = 1; i < size; i++) {
            auto arg = events[i].message;
            buf.push_back(arg & 0xff);
            buf.push_back((arg >> 8) & 0xff);
            buf.push_back((arg >> 16) & 0xff);
            buf.push_back((arg >> 24) & 0xff);
        }

        // Look for terminating byte
        int len = 0;
        for (size_t i = 0; i < buf.size(); i++) {
            if (buf[i] == MIDI_CMD_COMMON_SYSEX_END) {
                len = i + 1;
                break;
            }
        }

        // Debug
        //std::cout << boost::format("MIDI: Sysex message: length = %d, buffer = %d") % len % buf.size() << std::endl;

        // Process found message (if any)
        if (len > 0) {
            read_sysex(std::vector<uint8_t>(buf.data(), buf.data() + len - 1));
            return 1 + (len > 3 ? (len - 3 + 3) / 4 : 0);
        } else {
            return 0;
        }
    } else {
        // Short message
        read_short(msg);
        return 1;
    }
}

void c_controller_midi::read_short(PmMessage msg)
{
    switch (Pm_MessageStatus(msg) & 0xf0) {
        case MIDI_CMD_NOTE_OFF:
            // Note off
            read_note_off(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg), Pm_MessageData2(msg));
            return;
        case MIDI_CMD_NOTE_ON:
            // Note on
            read_note_on(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg), Pm_MessageData2(msg));
            return;
        case MIDI_CMD_NOTE_PRESSURE:
            // Key pressure
            read_note_pressure(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg), Pm_MessageData2(msg));
            return;
        case MIDI_CMD_CONTROL:
            // Control change
            read_control(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg), Pm_MessageData2(msg));
            return;
        case MIDI_CMD_PGM_CHANGE:
            // Program change
            read_program_change(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg));
            return;
        case MIDI_CMD_CHANNEL_PRESSURE:
            // Channel pressure
            read_channel_pressure(Pm_MessageStatus(msg) & 0x0f, Pm_MessageData1(msg));
            return;
        case MIDI_CMD_BENDER:
            // Pitch bender
            read_bender(Pm_MessageStatus(msg) & 0x0f, static_cast<int>(Pm_MessageData1(msg)) | (static_cast<int>(Pm_MessageData2(msg)) << 7));
            return;
        case MIDI_CMD_COMMON_SYSEX:
            // System messages
            return;
    }

    switch (Pm_MessageStatus(msg)) {
        case MIDI_CMD_COMMON_MTC_QUARTER:
            // MTC quarter frame
            read_common_mtc_quarter(Pm_MessageData1(msg) >> 4, Pm_MessageData1(msg) & 0x0f);
            return;
        case MIDI_CMD_COMMON_SONG_POS:
            // Song position
            read_common_song_pos(static_cast<int>(Pm_MessageData1(msg)) | (static_cast<int>(Pm_MessageData2(msg)) << 7));
            return;
        case MIDI_CMD_COMMON_SONG_SELECT:
            // Song selection
            read_common_song_select(Pm_MessageData1(msg));
            return;
        case MIDI_CMD_COMMON_TUNE_REQUEST:
            // Tune request
            read_common_tune_request();
            return;
        case MIDI_CMD_COMMON_CLOCK:
            // Start clock
            read_realtime_clock();
            return;
        case MIDI_CMD_COMMON_START:
            // Start
            read_realtime_start();
            return;
        case MIDI_CMD_COMMON_CONTINUE:
            // Continue
            read_realtime_continue();
            return;
        case MIDI_CMD_COMMON_STOP:
            // Stop
            read_realtime_stop();
            return;
        case MIDI_CMD_COMMON_SENSING:
            // Active sensing
            read_realtime_sensing();
            return;
        case MIDI_CMD_COMMON_RESET:
            // Reset
            read_realtime_reset();
            return;
        case MIDI_CMD_COMMON_SYSEX:
            // System-exclusive message
            return;
    }
}

void c_controller_midi::read_sysex(std::vector<uint8_t> msg)
{
    // Debug
    std::vector<std::string> list;
    for (uint8_t dat : msg)
        list.push_back(boost::str(boost::format("0x%02x") % dat));
    std::cout << boost::format("MIDI: Sysex: %s") % boost::algorithm::join(list, ", ") << std::endl;
}

// Read channel messages
void c_controller_midi::read_note_off(int channel, int key, int velocity)
{
    std::cout << boost::format("MIDI: Note off! channel=%d, key=%d, velocity=%d") % channel % key % velocity << std::endl;
    if (m_channel[channel].note[key] != false) {
        m_channel[channel].note[key] = false;
        c_global::controller->input_midi_key(boost::str(boost::format("c%dn%d") % channel % key), false);
    }
    if (m_channel[channel].velocity[key] != velocity) {
        m_channel[channel].velocity[key] = velocity;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dv%d") % channel % key), static_cast<float>(velocity) / 127.0f);
    }
}

void c_controller_midi::read_note_on(int channel, int key, int velocity)
{
    std::cout << boost::format("MIDI: Note on! channel=%d, key=%d, velocity=%d") % channel % key % velocity << std::endl;
    if (m_channel[channel].note[key] != true) {
        m_channel[channel].note[key] = true;
        c_global::controller->input_midi_key(boost::str(boost::format("c%dn%d") % channel % key), true);
    }
    if (m_channel[channel].velocity[key] != velocity) {
        m_channel[channel].velocity[key] = velocity;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dv%d") % channel % key), static_cast<float>(velocity) / 127.0f);
    }
}

void c_controller_midi::read_note_pressure(int channel, int key, int pressure)
{
    std::cout << boost::format("MIDI: Pressure! channel=%d, key=%d, pressure=%d") % channel % key % pressure << std::endl;
    if (m_channel[channel].pressure[key] != pressure) {
        m_channel[channel].pressure[key] = pressure;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dp%d") % channel % key), static_cast<float>(pressure) / 127.0f);
    }
}

void c_controller_midi::read_control(int channel, int id, int value)
{
    std::cout << boost::format("MIDI: Control! channel=%d, id=%d, value=%d") % channel % id % value << std::endl;
    if (m_channel[channel].control[id] != value) {
        m_channel[channel].control[id] = value;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dc%d") % channel % id), static_cast<float>(value) / 127.0f);
    }
}

void c_controller_midi::read_program_change(int channel, int program)
{
    std::cout << boost::format("MIDI: Program change! channel=%d, program=%d") % channel % program << std::endl;
    if (m_channel[channel].ch_program != program) {
        m_channel[channel].ch_program = program;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dprogram") % channel), static_cast<float>(program) / 127.0f);
    }
}

void c_controller_midi::read_channel_pressure(int channel, int pressure)
{
    std::cout << boost::format("MIDI: Channel pressure! channel=%d, pressure=%d") % channel % pressure << std::endl;
    if (m_channel[channel].ch_pressure != pressure) {
        m_channel[channel].ch_pressure = pressure;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dpressure") % channel), static_cast<float>(pressure) / 127.0f);
    }
}

void c_controller_midi::read_bender(int channel, int value)
{
    std::cout << boost::format("MIDI: Bender! channel=%d, value=%d") % channel % value << std::endl;
    if (m_channel[channel].ch_bender != value) {
        m_channel[channel].ch_bender = value;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dbender") % channel), static_cast<float>(value) / 127.0f);
    }
}

// Read system common messages
void c_controller_midi::read_common_mtc_quarter(int id, int value)
{
    std::cout << boost::format("MIDI: MTC quarter! id=%d, value=%d") % id % value << std::endl;
}

void c_controller_midi::read_common_song_pos(int id)
{
    std::cout << boost::format("MIDI: Song position! id=%d") % id << std::endl;
}

void c_controller_midi::read_common_song_select(int id)
{
    std::cout << boost::format("MIDI: Song selection! id=%d") % id << std::endl;
}

void c_controller_midi::read_common_tune_request()
{
    std::cout << boost::format("MIDI: Tune request!") << std::endl;
}

// Read system real-time messages
void c_controller_midi::read_realtime_clock()
{
    std::cout << boost::format("MIDI: Clock!") << std::endl;
}

void c_controller_midi::read_realtime_start()
{
    std::cout << boost::format("MIDI: Start!") << std::endl;
}

void c_controller_midi::read_realtime_continue()
{
    std::cout << boost::format("MIDI: Continue!") << std::endl;
}

void c_controller_midi::read_realtime_stop()
{
    std::cout << boost::format("MIDI: Stop!") << std::endl;
}

void c_controller_midi::read_realtime_sensing()
{
    std::cout << boost::format("MIDI: Sensing!") << std::endl;
}

void c_controller_midi::read_realtime_reset()
{
    std::cout << boost::format("MIDI: Reset!") << std::endl;
}

// Write messages
void c_controller_midi::write_short(PmMessage msg)
{
    if (m_output_stream)
        Pm_WriteShort(m_output_stream, 0, msg);
}

void c_controller_midi::write_sysex(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw;
    raw.push_back(0xf0);
    raw.insert(raw.end(), msg.begin(), msg.end());
    raw.push_back(0xf7);

    // Debug
    /*
    std::vector<std::string> list;
    for (uint8_t r : raw)
        list.push_back(boost::str(boost::format("%02x") % static_cast<uint32_t>(r)));
    std::cout << boost::format("MIDI: Sending: %s") % boost::algorithm::join(list, " ") << std::endl;
    */

    // Send
    if (m_output_stream)
        Pm_WriteSysEx(m_output_stream, 0, raw.data());
}

// Write channel messages
void c_controller_midi::write_note_off(int channel, int key, int velocity)
{
    write_short(Pm_Message(MIDI_CMD_NOTE_OFF | channel, key & 0x7f, velocity & 0x7f));
}

void c_controller_midi::write_note_on(int channel, int key, int velocity)
{
    write_short(Pm_Message(MIDI_CMD_NOTE_ON | channel, key & 0x7f, velocity & 0x7f));
}

void c_controller_midi::write_note_pressure(int channel, int key, int pressure)
{
    write_short(Pm_Message(MIDI_CMD_NOTE_PRESSURE | channel, key & 0x7f, pressure & 0x7f));
}

void c_controller_midi::write_control(int channel, int id, int value)
{
    write_short(Pm_Message(MIDI_CMD_CONTROL | channel, id & 0x7f, value & 0x7f));
}

void c_controller_midi::write_program_change(int channel, int program)
{
    write_short(Pm_Message(MIDI_CMD_PGM_CHANGE | channel, program & 0x7f, 0));
}

void c_controller_midi::write_channel_pressure(int channel, int pressure)
{
    write_short(Pm_Message(MIDI_CMD_CHANNEL_PRESSURE | channel, pressure & 0x7f, 0));
}

void c_controller_midi::write_bender(int channel, int value)
{
    write_short(Pm_Message(MIDI_CMD_BENDER | channel, value & 0x7f, (value >> 7) & 0x7f));
}

// Write system common messages
void c_controller_midi::write_common_mtc_quarter(int id, int value)
{
    write_short(Pm_Message(MIDI_CMD_COMMON_MTC_QUARTER, ((id & 0x07) << 4) | (value & 0x0f), 0));
}

void c_controller_midi::write_common_song_pos(int id)
{
    write_short(Pm_Message(MIDI_CMD_COMMON_SONG_POS, id & 0x7f, (id >> 7) & 0x7f));
}

void c_controller_midi::write_common_song_select(int id)
{
    write_short(Pm_Message(MIDI_CMD_COMMON_SONG_SELECT, id & 0x7f, 0));
}

void c_controller_midi::write_common_tune_request()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_TUNE_REQUEST, 0, 0));
}

// Read system real-time messages
void c_controller_midi::write_realtime_clock()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_CLOCK, 0, 0));
}

void c_controller_midi::write_realtime_start()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_START, 0, 0));
}

void c_controller_midi::write_realtime_continue()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_CONTINUE, 0, 0));
}

void c_controller_midi::write_realtime_stop()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_STOP, 0, 0));
}

void c_controller_midi::write_realtime_sensing()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_SENSING, 0, 0));
}

void c_controller_midi::write_realtime_reset()
{
    write_short(Pm_Message(MIDI_CMD_COMMON_RESET, 0, 0));
}
