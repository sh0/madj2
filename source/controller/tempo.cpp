/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "controller/tempo.hpp"

// Constructor
c_controller_tempo::c_controller_tempo() :
    // Definition
    m_define_state(false),
    // Freeform
    m_freeform_state(false),
    m_freeform_last(-1),
    m_freeform_region(0),
    m_freeform_value(0.5),
    // Tempo
    m_tempo_state(false)
{

}

// Definition
void c_controller_tempo::define_begin()
{
    // State
    m_define_state = true;
    m_define_beats.clear();
}

void c_controller_tempo::define_end()
{
    // Validate tempo
    if (m_define_beats.size() > 2) {
        m_tempo_start = m_tempo_beats.front();

        m_tempo_beats.clear();
        for (size_t i = 0; i < m_define_beats.size() - 1; i++)
            m_tempo_beats.push_back(m_define_beats[i + 1] - m_define_beats[i]);

        m_tempo_duration = 0.0;
        for (auto duration : m_tempo_beats)
            m_tempo_duration += duration;

        m_tempo_state = true;
    }

    // State
    m_define_state = false;
    m_define_beats.clear();
}

void c_controller_tempo::define_beat()
{
    m_define_beats.push_back(g_time_now_us());
}

int64_t c_controller_tempo::define_last()
{
    if (m_define_beats.empty())
        return -1.0;
    return m_define_beats.back();
}

// Freeform
void c_controller_tempo::freeform_value(double value)
{
    // Check for changes
    if (m_freeform_value == value)
        return;

    // Update
    m_freeform_value = value;

    int region = 0;
    if (value < 0.01)
        region = -1;
    if (value > 0.99)
        region = 1;

    if (region != 0 && region != m_freeform_region)
        m_freeform_last = g_time_now_us();
    m_freeform_region = region;
}

// Tempo
double c_controller_tempo::tempo_value_at_timepoint(int64_t tp)
{
    // Freeform override
    if (m_freeform_state)
        return m_freeform_value;

    // Check if we have valid tempo
    if (!m_tempo_state)
        return 0.0;

    // Find the current beat
    int64_t offset = std::max<int64_t>(0, (tp - m_tempo_start) % m_tempo_duration);
    for (size_t i = 0; i < m_tempo_beats.size(); i++) {
        if (offset < m_tempo_beats[i])
            return static_cast<double>(offset) / static_cast<double>(m_tempo_beats[i]);
        offset = std::max<int64_t>(0, offset - m_tempo_beats[i]);
    }

    // End of the tempo cycle
    return 1.0;
}

bool c_controller_tempo::tempo_beat_in_interval(int64_t tp_a, int64_t tp_b)
{
    // Input validation
    if (tp_a > tp_b)
        return false;

    // Freeform override
    if (m_freeform_state)
        return (tp_a <= m_freeform_last && tp_b >= m_freeform_last);

    // Check if we have valid tempo
    if (!m_tempo_state)
        return false;

    // Check if we have more than one cycle of beats
    if (tp_b - tp_a >= m_tempo_duration)
        return true;

    // Find beat number within cycle
    auto beat_phase_counter = [&](int64_t tp) {
        int64_t offset = std::max<int64_t>(0, (tp - m_tempo_start) % m_tempo_duration);
        for (size_t i = 0; i < m_tempo_beats.size(); i++) {
            if (offset < m_tempo_beats[i])
                return i;
            offset = std::max<int64_t>(0, offset - m_tempo_beats[i]);
        }
        return m_tempo_beats.size();
    };
    return (beat_phase_counter(tp_a) == beat_phase_counter(tp_b) ? false : true);
}
