/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_TIMER
#define H_TIMER

// Internal
#include "config.hpp"

// C++
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>

// Boost
#include <boost/circular_buffer.hpp>

// Static functions
inline int64_t g_time_now_us() // us units
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

inline double g_time_now_s() // s units
{
    return static_cast<double>(g_time_now_us()) / 1000000.0;
}

inline int64_t g_time_fps2us(double fps) { return (1.0 / fps) * 1000000.0; }
inline int64_t g_time_sec2us(double sec) { return sec * 1000000.0; }
inline double g_time_us2sec(int64_t us) { return static_cast<double>(us) / 1000000.0; }

// Cyclic timer
class c_time_cyclic
{
    public:
        // Constructor
        c_time_cyclic(int64_t rate, bool sleep = false) :
            // Sleeping
            m_sleep(sleep),
            // Rates
            m_rate_real(rate), m_rate_sync(rate), m_rate_history(30),
            // Timestamps
            m_time_last(g_time_now_us())
        { }

        // Cycle control
        void cycle() {
            // Current time
            int64_t time_new = g_time_now_us();

            // Rate calculation and sleeping
            m_rate_real = std::max<int64_t>(0, m_time_last - time_new);
            m_rate_history.push_back(m_rate_real);
            if (m_sleep && m_rate_sync - m_rate_real >= 1000)
                std::this_thread::sleep_for(std::chrono::milliseconds((m_rate_sync - m_rate_real) / 1000));

            // Update time
            if (m_time_last < time_new)
                m_time_last = time_new;
        }

        // Rates
        int64_t rate_real() { return m_rate_real; }
        int64_t rate_sync() { return m_rate_sync; }
        int64_t rate_history() {
            int64_t sum = 0;
            for (int64_t r : m_rate_history)
                sum += r;
            return sum / m_rate_history.size();
        }

        // Timestamps
        int64_t time_last() { return m_time_last; }
        int64_t time_this() { return m_time_last + (1 * m_rate_sync); }
        int64_t time_next() { return m_time_last + (2 * m_rate_sync); }

    private:
        // Sleeping
        bool m_sleep;

        // Rates
        int64_t m_rate_real;
        int64_t m_rate_sync;
        boost::circular_buffer<int64_t> m_rate_history;

        // Timestamps
        int64_t m_time_last;
};

#endif

