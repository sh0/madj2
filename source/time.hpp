/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_TIME
#define H_TIME

// Internal
#include "config.hpp"

// C++
#include <chrono>
#include <thread>

// Static functions
inline int64_t mj_time_now() // us units
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

inline int64_t mj_time_fps2us(double fps) { return (1.0 / fps) * 1000000.0; }
inline int64_t mj_time_sec2us(double sec) { return sec * 1000000.0; }

// Cyclic timer (seconds - float64)
class c_time_cyclic
{
    public:
        // Constructor
        c_time_cyclic(double rate_need, double rate_error) :
            m_rate_need(rate_need), m_rate_real(rate_need), m_rate_error(rate_error)
            { m_time_last = static_cast<double>(mj_time_now()) / 1000000.0; }

        // Cycle control
        void cycle() {
            double time_new = static_cast<double>(mj_time_now()) / 1000000.0;
            auto clamp_fn = [](double v, double a, double b){ return (v <= a ? a : (v >= b ? b : v)); };
            m_rate_real = clamp_fn(
                time_new - m_time_last,
                std::max(0.0, m_rate_need * (1.0 - m_rate_error)),
                m_rate_need * (1.0 + m_rate_error)
            );
            if (m_time_last < time_new)
                m_time_last = time_new;
        }

        // Rate info
        double rate_need() { return m_rate_need; }
        double rate_real() { return m_rate_real; }
        double rate_error() { return m_rate_error; }

        // Timestamps
        double time_last() { return m_time_last; }
        double time_next() { return m_time_last + m_rate_need; }

    private:
        // Rates
        double m_rate_need;
        double m_rate_real;
        double m_rate_error;

        // Timestamps
        double m_time_last;
};

// Thread timer (milliseconds - int64)
class c_time_thread
{
    public:
        // Constructor
        c_time_thread(int64_t rate) : m_rate(rate)
            { m_time_last = mj_time_now(); }

        // Cycle control
        void cycle() {
            int64_t time_new = mj_time_now();
            int64_t time_diff = (m_time_last + (m_rate * 1000)) - time_new;
            if (time_diff > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(time_diff / 1000));
            m_time_last = time_new + std::max(time_diff, 0L);
        }

    private:
        // Rates
        int64_t m_rate;

        // Timestamps
        int64_t m_time_last;
};

// Period timer
class c_time_period
{
    public:
        // Constructor
        c_time_period(std::shared_ptr<c_time_cyclic> cyclic, double period) :
            m_cyclic(cyclic),
            m_period(period),
            m_last(cyclic->time_last())
            { }

        // Period completion at next frame
        double percent() {
            double diff = m_cyclic->time_next() - m_last;
            if (diff < 0.0) {
                diff = 0.0;
                m_last = m_cyclic->time_next();
            } else if (diff >= m_period) {
                diff = fmod(diff, m_period);
                m_last = m_cyclic->time_next() - diff;
            }
            return diff / m_period;
        }

    private:
        // Times
        std::shared_ptr<c_time_cyclic> m_cyclic;
        double m_period;
        double m_last;
};

#endif

