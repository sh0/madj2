/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/ohmrgb.hpp"

// Boost
#include <boost/algorithm/string/join.hpp>

// Constructor
c_controller_ohmrgb::c_controller_ohmrgb(PmDeviceID input, PmDeviceID output) :
    c_controller_midi(input, output)
{
    // Info
    std::cout << boost::format("OhmRGB: input = %d, output = %d") % input % output << std::endl;

    // Reset
    //write_sysex_ohmrgb_factory_reset();

    // Leds
    for (size_t i = 0; i < m_state_leds.size(); i++)
        m_state_leds[i] = (i % 7) + 1;
    leds_update();

    // Backlight
    //write_backlight_dimmer(127);
    //write_logo_dimmer(127);

    // Get controls
    write_sysex_ohmrgb_request_controls();
}

// Leds
static std::map<std::string, int> g_leds = {
    // G
    { "g1" , 56 }, { "g2" , 48 }, { "g3" , 40 }, { "g4" , 32 }, { "g5" , 24 }, { "g6" , 16 }, { "g7" ,  8 }, { "g8" ,  0 },
    { "g9" , 60 }, { "g10", 52 }, { "g11", 44 }, { "g12", 36 }, { "g13", 28 }, { "g14", 20 }, { "g15", 12 }, { "g16",  4 },
    { "g17", 57 }, { "g18", 49 }, { "g19", 41 }, { "g20", 33 }, { "g21", 25 }, { "g22", 17 }, { "g23",  9 }, { "g24",  1 },
    { "g25", 61 }, { "g26", 53 }, { "g27", 45 }, { "g28", 37 }, { "g29", 29 }, { "g30", 21 }, { "g31", 13 }, { "g32",  5 },
    { "g33", 58 }, { "g34", 50 }, { "g35", 42 }, { "g36", 34 }, { "g37", 26 }, { "g38", 18 }, { "g39", 10 }, { "g40",  2 },
    { "g41", 62 }, { "g42", 54 }, { "g43", 46 }, { "g44", 38 }, { "g45", 30 }, { "g46", 22 }, { "g47", 14 }, { "g48",  6 },
    { "g49", 59 }, { "g50", 51 }, { "g51", 43 }, { "g52", 35 }, { "g53", 27 }, { "g54", 19 }, { "g55", 11 }, { "g56",  3 },
    { "g57", 63 }, { "g58", 55 }, { "g59", 47 }, { "g60", 39 }, { "g61", 31 }, { "g62", 23 }, { "g63", 15 }, { "g64",  7 },
    // B
    { "b1", 66 }, { "b2", 67 }, { "b3", 70 }, { "b4", 71 },
    { "b5", 79 }, { "b6", 78 }, { "b7", 77 }, { "b8", 76 },
    // F
    { "f4", 64 }, { "f5", 68 }, { "f6", 72 },
    { "f1", 65 }, { "f2", 69 }, { "f3", 73 },
    // X
    { "xl", 74 }, { "xr", 75 },
    // BPM
    { "bpm", 80 }
};

void c_controller_ohmrgb::leds_set(std::string id, uint8_t value)
{
    auto it = g_leds.find(id);
    if (it != g_leds.end())
        m_state_leds[it->second] = value;
}

// Controls
void c_controller_ohmrgb::control_set(std::string id, uint8_t value)
{
    // Debug
    std::cout << boost::format("OhmRGB: Control! id = %s, value = %d") % id % static_cast<int>(value) << std::endl;

    // Save state
    m_state_control[id] = value;

    // Leds
    leds_set(id, (value == 0 ? 0 : 7));
    leds_update();
}

uint8_t c_controller_ohmrgb::control_get(std::string id)
{
    auto it = m_state_control.find(id);
    if (it != m_state_control.end())
        return it->second;
    return 0;
}

// Read control messages
static std::map<int, std::string> g_note_on = {
    // G
    {  0, "g1"  }, {  8, "g2"  }, { 16, "g3"  }, { 24, "g4"  }, { 32, "g5"  }, { 40, "g6"  }, { 48, "g7"  }, { 56, "g8"  },
    {  1, "g9"  }, {  9, "g10" }, { 17, "g11" }, { 25, "g12" }, { 33, "g13" }, { 41, "g14" }, { 49, "g15" }, { 57, "g16" },
    {  2, "g17" }, { 10, "g18" }, { 18, "g19" }, { 26, "g20" }, { 34, "g21" }, { 42, "g22" }, { 50, "g23" }, { 58, "g24" },
    {  3, "g25" }, { 11, "g26" }, { 19, "g27" }, { 27, "g28" }, { 35, "g29" }, { 43, "g30" }, { 51, "g31" }, { 59, "g32" },
    {  4, "g33" }, { 12, "g34" }, { 20, "g35" }, { 28, "g36" }, { 36, "g37" }, { 44, "g38" }, { 52, "g39" }, { 60, "g40" },
    {  5, "g41" }, { 13, "g42" }, { 21, "g43" }, { 29, "g44" }, { 37, "g45" }, { 45, "g46" }, { 53, "g47" }, { 61, "g48" },
    {  6, "g49" }, { 14, "g50" }, { 22, "g51" }, { 30, "g52" }, { 38, "g53" }, { 46, "g54" }, { 54, "g55" }, { 62, "g56" },
    {  7, "g57" }, { 15, "g58" }, { 23, "g59" }, { 31, "g60" }, { 39, "g61" }, { 47, "g62" }, { 55, "g63" }, { 63, "g64" },
    // B
    { 65, "b1"  }, { 73, "b2"  }, { 66, "b3"  }, { 74, "b4"  },
    { 67, "b5"  }, { 75, "b6"  }, { 68, "b7"  }, { 76, "b8"  },
    // F
    { 69, "f4"  }, { 70, "f5"  }, { 71, "f6"  },
    { 77, "f1"  }, { 78, "f2"  }, { 79, "f3"  },
    // X
    { 64, "xl"  }, { 72, "xr"  },
    // BPM
    { 87, "bpm"  }
};

void c_controller_ohmrgb::read_note_on(int channel, int key, int velocity)
{
    // Debug
    //std::cout << boost::format("OhmRGB: Button! id = %d, value = %d") % key % velocity << std::endl;

    // Submit
    auto it = g_note_on.find(key);
    if (it != g_note_on.end())
        control_set(it->second, (velocity == 0 ? 0 : 1));
}

static std::map<int, std::string> g_control = {
    // Knobs (left)
    { 17, "k1"  }, { 16, "k2"  }, {  9, "k3"  }, {  8, "k4"  },
    { 19, "k5"  }, { 18, "k6"  }, { 11, "k7"  }, { 10, "k8"  },
    { 21, "k9"  }, { 20, "k10" }, { 13, "k11" }, { 12, "k12" },
    // Knobs (right)
    {  3, "k13" }, {  1, "k14" }, {  0, "k15" }, {  2, "k16" },
    // Sliders (left)
    { 23, "s1"  }, { 22, "s2"  }, { 15, "s3"  }, { 14, "s4"  },
    // Sliders (right)
    {  5, "s5"  }, {  7, "s6"  }, {  6, "s7"  }, {  4, "s8"  },
    // Crossfader
    { 24, "crossfader" }
};

void c_controller_ohmrgb::read_control(int channel, int id, int value)
{
    // Debug
    //std::cout << boost::format("OhmRGB: Slider! id = %d, value = %d") % id % value << std::endl;

    // Submit
    auto it = g_control.find(id);
    if (it != g_control.end())
        control_set(it->second, value);
}

// Read system exclusive messages
void c_controller_ohmrgb::read_sysex(std::vector<uint8_t> msg)
{
    if (msg.size() >= 5 && msg[0] == 0x00 && msg[1] == 0x01 && msg[2] == 0x61) {
        read_sysex_ohmrgb(msg[4], msg[3], std::vector<uint8_t>(msg.begin() + 5, msg.end()));
    } else {
        if (msg.size() >= 4) {
            std::cout << boost::format("OhmRGB: Unknown sysex! id=[0x%02x, 0x%02x, 0x%02x]") % msg[1] % msg[2] % msg[3] << std::endl;
        } else if (msg.size() >= 2) {
            std::cout << boost::format("OhmRGB: Unknown sysex! id=[0x%02x]") % msg[1] << std::endl;
        } else {
            std::cout << boost::format("OhmRGB: Unknown sysex!") << std::endl;
        }
    }
}

void c_controller_ohmrgb::read_sysex_ohmrgb(int op, int device, std::vector<uint8_t> msg)
{
    if (op == 0x7e) {
        // NAK - Negative acknowledge
        std::cout << "OhmRGB: NAK!" << std::endl;
    } else if (op == 0x7f) {
        // ACK - Positive acknowledge
        //std::cout << "OhmRGB: ACK!" << std::endl;
    } else if (op == 0x04) {
        // Request all LED indicators
        read_sysex_ohmrgb_request_leds(msg);
    } else {
        // Unknown message
        std::vector<std::string> list;
        for (uint8_t r : msg)
            list.push_back(boost::str(boost::format("%02x") % static_cast<uint32_t>(r)));
        std::cout << boost::format("OhmRGB: Unknown message: op = 0x%02x data = [%s]") % op % boost::algorithm::join(list, " ") << std::endl;
    }
}

void c_controller_ohmrgb::read_sysex_ohmrgb_request_leds(std::vector<uint8_t> msg)
{
    if (msg.size() < 43)
        return;
    for (size_t i = 0; i < msg.size(); i++) {
        m_state_leds[2 * i + 0] = (msg[i] & 0x07);
        m_state_leds[2 * i + 1] = ((msg[i] >> 3) & 0x07);
    }
}

// Write system exclusive messages
void c_controller_ohmrgb::write_sysex_ohmrgb(std::vector<uint8_t> msg)
{
    // Message
    std::vector<uint8_t> raw{ 0x00, 0x01, 0x61, 0x07 };
    raw.insert(raw.end(), msg.begin(), msg.end());

    // Send
    write_sysex(raw);
}

void c_controller_ohmrgb::write_sysex_ohmrgb_set_leds(std::array<uint8_t, 82>& leds)
{
    // Message
    std::vector<uint8_t> raw(43);
    raw[0] = 0x04;
    for (size_t i = 1; i < raw.size(); i++)
        raw[i] = 0;

    // Leds
    for (size_t i = 0; i < leds.size(); i++)
        raw[(i / 2) + 1] |= ((leds[i] & 0x07) << ((i % 2) == 0 ? 0 : 3));

    // Send
    write_sysex_ohmrgb(raw);
}
