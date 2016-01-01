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
    // Media
    m_media_frames(0),
    m_media_rate(0),
    // Playback mode
    m_playback_mode(PLAYBACK_STOP),
    m_playback_time(0.0),
    // Playback play
    m_playback_play_point(g_time_now_us()),
    m_playback_play_time(0.0),
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
        // Lock
        std::lock_guard<std::mutex> lock(m_mutex);

        // File
        m_queue.push_back(path);

        // Playback
        m_playback_mode = PLAYBACK_PLAY;

        m_playback_play_point = g_time_now_us();
        m_playback_play_time = 0.0;
        m_playback_play_speed = 1.0;
    }

    // Wake up thread
    m_cond.notify_one();
}

// Dispatch
void c_media_work::dispatch(c_time_cyclic& timer)
{
    bool wakeup = false;
    {
        // Lock
        std::lock_guard<std::mutex> lock(m_mutex);

        // Checks
        if (m_media_frames <= 0)
            return;

        // Video
        //int64_t rate = g_time_fps2us(m_media_rate);

        // Playback
        if (m_playback_mode == PLAYBACK_STOP) {
            // Frame
            int64_t frame = std::max<int64_t>(std::min<int64_t>(m_playback_time / m_media_rate, m_media_frames - 1), 0);
            cache_load(CACHE_PLAY, frame);

        } else if (m_playback_mode == PLAYBACK_PLAY) {
            // Play
            m_playback_play_time += m_playback_play_speed * g_time_us2sec(std::max<int64_t>(timer.time_this() - m_playback_play_point, 0));
            m_playback_play_time = std::max<double>(std::min<double>(m_playback_play_time, m_media_frames), 0.0);
            m_playback_play_point = timer.time_this();
            m_playback_time = std::max<double>(std::min<double>(m_playback_play_time, m_media_frames * m_media_rate), 0.0);

            // Load
            int64_t frame = std::max<int64_t>(std::min<int64_t>(m_playback_time / m_media_rate, m_media_frames - 1), 0);
            cache_load(CACHE_PLAY, frame);
        }
    }
    if (wakeup)
        m_cond.notify_one();
}

// Stop playback
void c_media_work::playback_stop()
{
    m_playback_mode = PLAYBACK_STOP;
}

// Start playback
void c_media_work::playback_play(double speed, double time)
{
    m_playback_mode = PLAYBACK_PLAY;
    m_playback_play_point = g_time_now_us();
    if (time >= 0.0)
        m_playback_play_time = time;
    else
        m_playback_play_time = m_playback_time;
    m_playback_play_speed = speed;
    m_playback_time = std::max<double>(std::min<double>(m_playback_play_time, m_media_frames * m_media_rate), 0.0);
}

// Thread
void c_media_work::thread()
{
    // File loading loop
    while (m_run) {
        // Wait for file
        boost::filesystem::path path;
        {
            // Lock
            std::unique_lock<std::mutex> lock(m_mutex);

            // Obtain path
            m_cond.wait(lock, [&]{ return !m_queue.empty() || !m_run; });
            if (m_queue.empty() || !m_run)
                continue;
            path = m_queue.back();
            m_queue.clear();
        }

        // Open file
        //std::cout << boost::format("Work: Opening! path = %s") % path << std::endl;
        try {
            m_state_error = false;
            m_file = std::make_shared<c_media_file>(path);
        } catch (c_exception& ex) {
            m_state_error = true;
            std::cout << ex.what() << std::endl;
            continue;
        }

        // Set up playback
        {
            // Lock
            std::unique_lock<std::mutex> lock(m_mutex);

            // Clean cache
            cache_clean();

            // Media properties
            if (m_file->video()) {
                m_media_frames = m_file->video()->frames();
                m_media_rate = m_file->video()->rate();
            } else {
                m_media_frames = 0;
                m_media_rate = 0;
            }
        }

        // Decoding loop
        m_state_loaded = true;
        while (m_run) {
            // Obtain entry to cache
            int cache_id = -1;
            int64_t cache_frame = -1;
            {
                // Lock
                std::unique_lock<std::mutex> lock(m_mutex);

                // Obtain frame
                m_cond.wait(lock, [&]{
                    if (!m_queue.empty())
                        return true;
                    for (size_t i = 0; i < m_cache.size(); i++) {
                        if (m_cache[i].frame_next >= 0 && m_cache[i].frame_next != m_cache[i].frame_this) {
                            cache_id = i;
                            cache_frame = m_cache[i].frame_next;
                            break;
                        }
                    }
                    return cache_id >= 0 || !m_run;
                });
                if (!m_queue.empty())
                    break;
                if (cache_id < 0 || cache_frame < 0 || !m_run)
                    continue;
            }

            // Cache frame
            //std::cout << boost::format("Work: Caching! id = %d, frame = %d") % cache_id % cache_frame << std::endl;
            try {
                auto image = m_file->video()->read(cache_frame);
                if (image) {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_cache[cache_id].frame_this = cache_frame;
                    m_cache[cache_id].uploaded = false;
                    m_cache[cache_id].image = image;
                } else {
                    m_cache[cache_id].frame_this = cache_frame;
                    m_cache[cache_id].image = nullptr;
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
void c_media_work::cache_clean()
{
    for (auto& cache : m_cache) {
        cache.frame_this = -1;
        cache.frame_next = -1;
        cache.uploaded = false;
        cache.image = nullptr;
    }
}

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

bool c_media_work::cache_load(int id, int64_t frame)
{
    // Check bounds
    if (id < 0 || id >= static_cast<int>(m_cache.size()))
        return false;

    // Check frame
    m_cache[id].frame_next = frame;
    if (m_cache[id].frame_this == m_cache[id].frame_next)
        return false;

    // Wake up
    m_cond.notify_one();
    return true;
}
