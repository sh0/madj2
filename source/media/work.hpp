/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_WORK
#define H_MEDIA_WORK

// Internal
#include "config.hpp"
#include "timer.hpp"
#include "media/file.hpp"
#include "opengl/texture.hpp"

// C++
#include <array>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

// Boost
#include <boost/filesystem.hpp>

// Media files
class c_media_work : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_work();
        ~c_media_work();

        // File
        void open(boost::filesystem::path path);

        // Frames
        std::shared_ptr<c_opengl_texture_2d> frame_play() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return cache_view(CACHE_PLAY);
        }

        // Dispatch
        void dispatch(c_time_cyclic& timer);

        // State
        bool state_error() { return m_state_error; }
        bool state_loaded() { return m_state_loaded; }

        // Playback mode
        enum e_playback {
            PLAYBACK_STOP = 0,
            PLAYBACK_PLAY,
            PLAYBACK_BOUNCE
        };
        e_playback playback_mode() { return m_playback_mode; }
        double playback_time() { return m_playback_time; }

        // Stop playback
        void playback_stop();

        // Start playback
        void playback_play(double speed, double time = -1.0);
        double playback_play_time() { return m_playback_play_time; }
        double playback_play_speed() { return m_playback_play_speed; }

    private:
        // Thread
        std::atomic_bool m_run;
        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable m_cond;
        void thread();

        // State
        std::atomic_bool m_state_error;
        std::atomic_bool m_state_loaded;

        // File
        std::list<boost::filesystem::path> m_queue;
        std::shared_ptr<c_media_file> m_file;

        // Cache
        enum e_cache {
            CACHE_PLAY = 0,
            CACHE_COUNT
        };
        struct s_cache {
            // Data
            int64_t frame_this;
            int64_t frame_next;
            bool uploaded;
            std::shared_ptr<c_opengl_image> image;
            std::shared_ptr<c_opengl_texture_2d> texture;

            // Constrcutor
            s_cache() : frame_this(-1), frame_next(-1), uploaded(false) { }
        };
        std::array<s_cache, CACHE_COUNT> m_cache;
        void cache_clean();
        std::shared_ptr<c_opengl_texture_2d> cache_view(int id);
        bool cache_load(int id, int64_t frame);

        // Media
        int64_t m_media_frames;
        double m_media_rate;

        // Playback mode
        e_playback m_playback_mode;
        double m_playback_time;

        // Playback play
        int64_t m_playback_play_point;
        double m_playback_play_time;
        double m_playback_play_speed;
};

#endif
