/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/work.hpp"

// C++
#include <chrono>

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_media_work::c_media_work() :
    // Thread
    m_run(true),
    // State
    m_state_error(false),
    m_state_loaded(false),
    // Playback mode
    m_playback_mode(PLAYBACK_STOP),
    m_playback_point(0),
    m_playback_frame(0),
    // Playback play
    m_playback_play_point(g_time_now_us()),
    m_playback_play_frame(0.0),
    m_playback_play_speed(1.0)
{
    // Thread
    m_thread = std::thread(&c_media_work::thread, this);
}

c_media_work::~c_media_work()
{
    // Thread
    m_run = false;
    m_cond.notify_one();
    m_thread.join();
}

// File
void c_media_work::open(boost::filesystem::path path)
{
    // Enqueue
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_back(path);
    }

    // Wake up thread
    m_cond.notify_one();
}

// Dispatch
void c_media_work::dispatch(c_time_cyclic& timer)
{
    // Lock
    std::lock_guard<std::mutex> lock(m_mutex);

    // Checks
    if (!m_file || !m_file->video())
        return;

    // Video
    int64_t frames = m_file->video()->frames();
    double rate = m_file->video()->rate();
    //int64_t rate = g_time_fps2us(m_file->video()->rate());

    // Playback
    if (m_playback_mode == PLAYBACK_PLAY) {
        // Play
        m_playback_play_frame = std::max<double>(std::min<double>(m_playback_play_speed * (curr - m_playback_play_point), frames), 0.0);
        m_playback_play_point = curr;

        // Mode
        m_playback_point = std::max<double>(std::min<double>(m_playback_play_frame, frames), 0.0);
        m_playback_frame = std::max<int64_t>(std::min<int64_t>(m_playback_point, frames - 1), 0);

        // Load
        cache_load(CACHE_PLAY, m_playback_frame);
    }

    // Wake up thread
    m_cond.notify_one();
}

// Stop playback
void c_media_work::playback_stop()
{
    m_playback_mode = PLAYBACK_STOP;
}

// Start playback
void c_media_work::playback_play(double speed, double frame)
{
    m_playback_mode = PLAYBACK_PLAY;
    m_playback_play_speed = speed;
    m_playback_play_point = time_now();
    if (frame >= 0.0)
        m_playback_play_frame = frame;
    else
        m_playback_play_frame = std::max<double>(std::min<double>(frame, m_video->frames()), 0.0);
}

// Thread
void c_media_work::thread()
{
    // File loading loop
    while (m_run) {
        // Path
        boost::filesystem::path path;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [&]{ return !m_queue.empty() || !m_run; });
            if (m_queue.empty() || !m_run)
                continue;
            auto path = m_queue.back();
            m_queue.clear();
        }

        // Open
        try {
            m_state_error = false;
            m_file = std::make_shared<c_media_file>(path);
        } catch (c_exception& ex) {
            m_state_error = true;
            std::cout << ex.what() << std::endl;
            continue;
        }

        // Decoding loop
        m_state_loaded = true;
        while (m_run) {
            // Obtain entry to cache
            int cache_id = -1;
            int64_t cache_frame = -1;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cond.wait(lock, [&]{
                    for (size_t i = 0; i < m_cache.size(); i++) {
                        if (m_cache[i].frame >= 0 && !m_cache[i].image) {
                            cache_id = i;
                            cache_frame = m_cache[i].frame;
                            break;
                        }
                    }
                    return cache_id >= 0 || !m_run;
                });
                if (cache_id < 0 || !m_run)
                    continue;
            }

            // Cache frame
            try {
                auto image = m_file->video()->read(cache_frame);
                if (image) {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    if (m_cache[cache_id].frame == cache_frame)
                        m_cache[cache_id].image = image;
                }
            } catch (c_exception& ex) {
                if (!m_state_error)
                    std::cout << ex.what() << std::endl;
                m_state_error = true;
                continue;
            }
        }
        m_state_loaded = false;
    }
}

// Cache
std::shared_ptr<c_opengl_texture_2d> c_media_work::cache_view(int id)
{
    // Checks
    if (id < 0 || id >= static_cast<int>(m_cache.size()))
        return nullptr;

    // Upload texture
    if (!m_cache[id].uploaded && m_cache[id].image) {
        m_cache[id].uploaded = true;
        if (!m_cache[id].texture)
            m_cache[id].texture = std::make_shared<c_opengl_texture_2d>();
        m_cache[id].texture->upload(m_cache[id].image);
    }
    return m_cache[id].texture;
}

void c_media_work::cache_load(int id, int64_t frame)
{
    // Checks
    if (id < 0 || id >= static_cast<int>(m_cache.size()))
        return;
    if (m_cache[id].frame == frame)
        return;

    // Reset
    m_cache[id].frame = frame;
    m_cache[id].image = nullptr;
    m_cache[id].uploaded = false;

    // Look for frame in cache
    for (size_t i = 0; i < m_cache.size(); i++) {
        if (id == static_cast<int>(i))
            continue;
        if (m_cache[i].frame == frame && m_cache[i].image) {
            // Copy image
            m_cache[id].image = m_cache[i].image;

            // Upload
            m_cache[id].uploaded = true;
            if (!m_cache[id].texture)
                m_cache[id].texture = std::make_shared<c_opengl_texture_2d>();
            m_cache[id].texture->upload(m_cache[id].image);
            return;
        }
    }
}
