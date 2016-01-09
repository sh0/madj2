/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_CONTROLLER_TEMPO
#define H_CONTROLLER_TEMPO

// Internal
#include "config.hpp"
#include "timer.hpp"

// Tempo class
class c_controller_tempo : boost::noncopyable
{
    public:
        // Constructor
        c_controller_tempo();

        // Definition
        bool define_state() { return m_define_state; }
        void define_begin();
        void define_end();
        void define_beat();
        int64_t define_last();

        // Freeform
        bool freeform_state() { return m_freeform_state; }
        void freeform_begin() { m_freeform_state = true; }
        void freeform_end() { m_freeform_state = false; }
        void freeform_value(double value);
        double freeform_value() { return m_freeform_value; }

        // Homing
        void homing_beat();

        // Tempo
        bool tempo_state() { return m_tempo_state; }
        double tempo_value_at_timepoint(int64_t tp);
        bool tempo_beat_in_interval(int64_t tp_a, int64_t tp_b); // [tp_a, tp_b)
        double tempo_bpm() { return m_tempo_bpm; }

        // Events
        bool event_action(std::string action, bool active);

    private:
        // Definition
        bool m_define_state;
        std::vector<int64_t> m_define_beats;

        // Freeform
        bool m_freeform_state;
        int64_t m_freeform_last;
        int m_freeform_region;
        double m_freeform_value;

        // Tempo
        bool m_tempo_state;
        int64_t m_tempo_start;
        int64_t m_tempo_duration;
        std::vector<int64_t> m_tempo_beats;
        double m_tempo_bpm;
};

#endif
