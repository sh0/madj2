/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "controller/controller.hpp"
#include "controller/midi.hpp"

// Constructor and destructor
c_controller_midi::c_controller_midi(PmDeviceID input, PmDeviceID output) :
    // Devices
    m_input_device(input), m_input_stream(nullptr),
    m_output_device(output), m_output_stream(nullptr),
    // Opcodes
    m_last_opcode(0)
{
    // Reading
    if (m_input_device >= 0) {
        PmError ret = Pm_OpenInput(&m_input_stream, m_input_device, nullptr, m_input_buffer.size(), nullptr, nullptr);
        if (ret != pmNoError)
            throw c_exception("Midi: Failed to initialize input device!", { throw_format("error", Pm_GetErrorText(ret)) });
    }

    // Writing
    if (m_output_device >= 0) {
        PmError ret = Pm_OpenOutput(&m_output_stream, m_output_device, nullptr, 1024, nullptr, nullptr, 0);
        if (ret != pmNoError)
            throw c_exception("Midi: Failed to initialize output device!", { throw_format("error", Pm_GetErrorText(ret)) });
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
void c_controller_midi::dispatch()
{
    if (m_input_stream) {
        while (Pm_Poll(m_input_stream)) {
            int ret = Pm_Read(m_input_stream, m_input_buffer.data(), m_input_buffer.size())
        }
    }
}

/*
void c_controller_midi::dispatch_read()
{
    // Read
    int ret = snd_rawmidi_read(m_midi_input, m_buffer_recv.write_data(), m_buffer_recv.write_size());
    if (ret > 0)
        m_buffer_recv.write_push(ret);

    // Process
    while (true) {
        int size = read_op0(m_buffer_recv.read_data(), m_buffer_recv.read_size());
        if (size > 0) {
            m_buffer_recv.read_pop(size);
        } else {
            if (size < 0) {
                //std::cout << "Midi: Unknown command!" << std::endl;
                m_buffer_recv.read_pop(m_buffer_recv.read_size());
            }
            return;
        }
    }
}

void c_controller_midi::dispatch_write()
{
    // Write
    if (m_buffer_send.read_size() > 0) {
        // Send
        int ret = snd_rawmidi_write(m_midi_output, m_buffer_send.read_data(), m_buffer_send.read_size());
        if (ret > 0)
            m_buffer_send.read_pop(ret);
    }
}
*/

// Read message opcodes
int c_controller_midi::read_op0(uint8_t* data, size_t size)
{
    // Check
    if (size == 0)
        return 0;

    // Use case
    if ((data[0] & 0x80) != 0 || m_last_opcode == 0) {
        return read_op1(data, size);
    } else {
        std::vector<uint8_t> raw{m_last_opcode};
        raw.insert(raw.end(), data, data + size);
        int ret = read_op1(raw.data(), raw.size());
        if (ret > 0)
            return ret - 1;
        else
            return ret;
    }
}

int c_controller_midi::read_op1(uint8_t* data, size_t size)
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
            m_last_opcode = MIDI_CMD_NOTE_OFF;
            read_note_off(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_NOTE_ON:
            // Note on
            if (size < 3)
                return 0;
            m_last_opcode = MIDI_CMD_NOTE_ON;
            read_note_on(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_NOTE_PRESSURE:
            // Key pressure
            if (size < 3)
                return 0;
            m_last_opcode = MIDI_CMD_NOTE_PRESSURE;
            read_note_pressure(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_CONTROL:
            // Control change
            if (size < 3)
                return 0;
            m_last_opcode = MIDI_CMD_CONTROL;
            read_control(data[0] & 0x0f, data[1], data[2]);
            return 3;
        case MIDI_CMD_PGM_CHANGE:
            // Program change
            if (size < 2)
                return 0;
            m_last_opcode = MIDI_CMD_PGM_CHANGE;
            read_program_change(data[0] & 0x0f, data[1]);
            return 2;
        case MIDI_CMD_CHANNEL_PRESSURE:
            // Channel pressure
            if (size < 2)
                return 0;
            m_last_opcode = MIDI_CMD_CHANNEL_PRESSURE;
            read_channel_pressure(data[0] & 0x0f, data[1]);
            return 2;
        case MIDI_CMD_BENDER:
            // Pitch bender
            if (size < 3)
                return 0;
            m_last_opcode = MIDI_CMD_BENDER;
            read_bender(data[0] & 0x0f, static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 7));
            return 3;
        case MIDI_CMD_COMMON_SYSEX:
            // System messages
            return read_op2(data, size);
        default:
            // Unknown opcode
            m_last_opcode = 0;
            std::cout << boost::format("Midi: Unknown message! op1=0x%02x, size=%d") % static_cast<int>(data[0]) % size << std::endl;
            return -1;
    }
}

int c_controller_midi::read_op2(uint8_t* data, size_t size)
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
                m_last_opcode = 0;
                read_sysex(data + 1, offset - 1);
                return offset + 1;
            }
        case MIDI_CMD_COMMON_MTC_QUARTER:
            // MTC quarter frame
            if (size < 2)
                return 0;
            m_last_opcode = MIDI_CMD_COMMON_MTC_QUARTER;
            read_common_mtc_quarter(data[1] >> 4, data[1] & 0x0f);
            return 1;
        case MIDI_CMD_COMMON_SONG_POS:
            // Song position
            if (size < 3)
                return 0;
            m_last_opcode = MIDI_CMD_COMMON_SONG_POS;
            read_common_song_pos(static_cast<int>(data[1]) | (static_cast<int>(data[2]) << 7));
            return 3;
        case MIDI_CMD_COMMON_SONG_SELECT:
            // Song selection
            if (size < 2)
                return 0;
            m_last_opcode = MIDI_CMD_COMMON_SONG_SELECT;
            read_common_song_select(data[1]);
            return 2;
        case MIDI_CMD_COMMON_TUNE_REQUEST:
            // Tune request
            m_last_opcode = MIDI_CMD_COMMON_TUNE_REQUEST;
            read_common_tune_request();
            return 1;
        case MIDI_CMD_COMMON_CLOCK:
            // Start clock
            m_last_opcode = MIDI_CMD_COMMON_CLOCK;
            read_realtime_clock();
            return 1;
        case MIDI_CMD_COMMON_START:
            // Start
            m_last_opcode = MIDI_CMD_COMMON_START;
            read_realtime_start();
            return 1;
        case MIDI_CMD_COMMON_CONTINUE:
            // Continue
            m_last_opcode = MIDI_CMD_COMMON_CONTINUE;
            read_realtime_continue();
            return 1;
        case MIDI_CMD_COMMON_STOP:
            // Stop
            m_last_opcode = MIDI_CMD_COMMON_STOP;
            read_realtime_stop();
            return 1;
        case MIDI_CMD_COMMON_SENSING:
            // Active sensing
            m_last_opcode = MIDI_CMD_COMMON_SENSING;
            read_realtime_sensing();
            return 1;
        case MIDI_CMD_COMMON_RESET:
            // Reset
            m_last_opcode = MIDI_CMD_COMMON_RESET;
            read_realtime_reset();
            return 1;
        default:
            // Unknown opcode
            m_last_opcode = 0;
            std::cout << boost::format("Midi: Unknown message! op2=0x%02x, size=%d") % static_cast<int>(data[0]) % size << std::endl;
            return -1;
    }
}

// Read channel messages
void c_controller_midi::read_note_off(int channel, int key, int velocity)
{
    std::cout << boost::format("Midi: Note off! channel=%d, key=%d, velocity=%d") % channel % key % velocity << std::endl;
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
    std::cout << boost::format("Midi: Note on! channel=%d, key=%d, velocity=%d") % channel % key % velocity << std::endl;
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
    std::cout << boost::format("Midi: Pressure! channel=%d, key=%d, pressure=%d") % channel % key % pressure << std::endl;
    if (m_channel[channel].pressure[key] != pressure) {
        m_channel[channel].pressure[key] = pressure;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dp%d") % channel % key), static_cast<float>(pressure) / 127.0f);
    }
}

void c_controller_midi::read_control(int channel, int id, int value)
{
    std::cout << boost::format("Midi: Control! channel=%d, id=%d, value=%d") % channel % id % value << std::endl;
    if (m_channel[channel].control[id] != value) {
        m_channel[channel].control[id] = value;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dc%d") % channel % id), static_cast<float>(value) / 127.0f);
    }
}

void c_controller_midi::read_program_change(int channel, int program)
{
    std::cout << boost::format("Midi: Program change! channel=%d, program=%d") % channel % program << std::endl;
    if (m_channel[channel].ch_program != program) {
        m_channel[channel].ch_program = program;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dprogram") % channel), static_cast<float>(program) / 127.0f);
    }
}

void c_controller_midi::read_channel_pressure(int channel, int pressure)
{
    std::cout << boost::format("Midi: Channel pressure! channel=%d, pressure=%d") % channel % pressure << std::endl;
    if (m_channel[channel].ch_pressure != pressure) {
        m_channel[channel].ch_pressure = pressure;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dpressure") % channel), static_cast<float>(pressure) / 127.0f);
    }
}

void c_controller_midi::read_bender(int channel, int value)
{
    std::cout << boost::format("Midi: Bender! channel=%d, value=%d") % channel % value << std::endl;
    if (m_channel[channel].ch_bender != value) {
        m_channel[channel].ch_bender = value;
        c_global::controller->input_midi_pot(boost::str(boost::format("c%dbender") % channel), static_cast<float>(value) / 127.0f);
    }
}

// Read system exclusive messages
void c_controller_midi::read_sysex(uint8_t* data, size_t size)
{
    if (size >= 5 && data[0] == 0x00 && data[1] == 0x01 && data[2] == 0x61) {
        read_sysex_ohmrgb(data[4], data[3], data + 5, size - 5);
    } else {
        if (size >= 4) {
            std::cout << boost::format("Midi: Unknown sysex! id=[0x%02x, 0x%02x, 0x%02x]") % data[1] % data[2] % data[3] << std::endl;
        } else if (size >= 2) {
            std::cout << boost::format("Midi: Unknown sysex! id=[0x%02x]") % data[1] << std::endl;
        } else {
            std::cout << boost::format("Midi: Unknown sysex!") << std::endl;
        }
    }
}

void c_controller_midi::read_sysex_ohmrgb(int op, int device, uint8_t* data, size_t size)
{
    if (op == 0x7e) {
        // NAK - Negative acknowledge
        std::cout << "Midi: Ohmrgb NAK!" << std::endl;
    } else if (op == 0x7f) {
        // ACK - Positive acknowledge
        std::cout << "Midi: Ohmrgb ACK!" << std::endl;
    }
}

// Read system common messages
void c_controller_midi::read_common_mtc_quarter(int id, int value)
{
    std::cout << boost::format("Midi: MTC quarter! id=%d, value=%d") % id % value << std::endl;
}

void c_controller_midi::read_common_song_pos(int id)
{
    std::cout << boost::format("Midi: Song position! id=%d") % id << std::endl;
}

void c_controller_midi::read_common_song_select(int id)
{
    std::cout << boost::format("Midi: Song selection! id=%d") % id << std::endl;
}

void c_controller_midi::read_common_tune_request()
{
    std::cout << boost::format("Midi: Tune request!") << std::endl;
}

// Read system real-time messages
void c_controller_midi::read_realtime_clock()
{
    std::cout << boost::format("Midi: Clock!") << std::endl;
}

void c_controller_midi::read_realtime_start()
{
    std::cout << boost::format("Midi: Start!") << std::endl;
}

void c_controller_midi::read_realtime_continue()
{
    std::cout << boost::format("Midi: Continue!") << std::endl;
}

void c_controller_midi::read_realtime_stop()
{
    std::cout << boost::format("Midi: Stop!") << std::endl;
}

void c_controller_midi::read_realtime_sensing()
{
    std::cout << boost::format("Midi: Sensing!") << std::endl;
}

void c_controller_midi::read_realtime_reset()
{
    std::cout << boost::format("Midi: Reset!") << std::endl;
}

// Write system exclusive messages
void c_controller_midi::write_sysex(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw;
    raw.push_back(0xf0);
    raw.insert(raw.end(), msg.begin(), msg.end());
    raw.push_back(0xf7);

    // Send
    //m_buffer_send.write(raw);
}

void c_controller_midi::write_sysex_ohmrgb(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw{ 0x00, 0x01, 0x61, 0x07 };
    raw.insert(raw.end(), msg.begin(), msg.end());

    // Send
    write_sysex(raw);
}
