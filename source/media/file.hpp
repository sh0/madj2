/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE
#define H_MEDIA_FILE

// Internal
#include "config.hpp"
#include "opengl/texture.hpp"

// C++
#include <thread>
#include <atomic>

// Boost
#include <boost/filesystem.hpp>

// Video base class
class c_media_video
{
    public:
        // Destructor
        virtual ~c_media_video() { }

        // Info
        virtual int64_t frames() = 0;
        virtual double rate() = 0;
        virtual double aspect() = 0;
        virtual int width() = 0;
        virtual int height() = 0;

        // Read
        virtual std::shared_ptr<c_opengl_image> read(int64_t id) = 0;
};

// Subtitle base class
class c_media_subtitle
{
    public:
        // Destructor
        virtual ~c_media_subtitle() { }

        // Entries
        struct s_entry {
            // Data
            double ts_s;
            double ts_e;
            std::vector<std::string> content;

            // Constructor
            s_entry() : ts_s(-1.0), ts_e(-1.0) { }
        };
        virtual std::vector<s_entry>& entries() = 0;
};

// Media files
class c_media_file : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file(boost::filesystem::path path);
        ~c_media_file();

        // Dispatch
        void dispatch();

        // State
        bool state_error() { return m_state_error; }
        bool state_loaded() { return m_state_loaded; }

        // Media
        std::shared_ptr<c_media_video> video() { return m_video; }
        std::shared_ptr<c_media_subtitle> subtitle() { return m_subtitle; }

        // Playback mode
        enum e_playback {
            PLAYBACK_STOP = 0,
            PLAYBACK_PLAY,
            PLAYBACK_BOUNCE
        };
        e_playback playback_mode() { return m_playback_mode; }
        double playback_point() { return m_playback_point; }
        int64_t playback_frame() { return m_playback_frame; }

        // Stop playback
        void playback_stop();

        // Start playback
        void playback_play(double speed, double frame = -1.0);
        double playback_play_point() { return m_playback_play_point; }
        double playback_play_frame() { return m_playback_play_frame; }
        double playback_play_speed() { return m_playback_play_speed; }

    private:
        // Thread
        std::atomic_bool m_run;
        std::thread m_thread;
        std::mutex m_mutex;
        void thread(boost::filesystem::path path);

        // State
        std::atomic_bool m_state_error;
        std::atomic_bool m_state_loaded;

        // Media
        std::shared_ptr<c_media_video> m_video;
        std::shared_ptr<c_media_subtitle> m_subtitle;

        // Playback mode
        e_playback m_playback_mode;
        double m_playback_point;
        int64_t m_playback_frame;

        // Playback play
        double m_playback_play_point;
        double m_playback_play_frame;
        double m_playback_play_speed;
};

#endif
