/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file.hpp"
#include "media/file_mp4.hpp"
#include "media/file_ffms.hpp"
#include "media/file_srt.hpp"
#include "media/file_ffst.hpp"

// C++
#include <chrono>

// Boost
#include <boost/algorithm/string.hpp>

// Timing
inline double time_now() // s units
{
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count()) / 1000000.0;
}

// Constructor and destructor
c_media_file::c_media_file(boost::filesystem::path path) :
    // Playback mode
    m_playback_mode(PLAYBACK_STOP),
    m_playback_point(0),
    m_playback_frame(0),
    // Playback play
    m_playback_play_point(time_now()),
    m_playback_play_frame(0.0),
    m_playback_play_speed(1.0)
{
    // Load video
    std::string path_ext = path.extension().native();
    boost::algorithm::to_lower(path_ext);
    if (path_ext == ".mp4" || path_ext == ".mkv" || path_ext == ".avi") {
        //m_video = std::make_shared<c_media_file_mp4>(path);
        m_video = std::make_shared<c_media_file_ffms>(path);
    } else {
        std::cout << "File: Unknown media file!" << std::endl;
    }

    // Load subtitles
    std::vector<boost::filesystem::path> path_subs = { ".srt", ".ass", ".ssa" };
    for (auto path_sub : path_subs) {
        auto path_tmp = path;
        path_tmp.replace_extension(path_sub);
        if (boost::filesystem::is_regular_file(path_tmp)) {
            m_subtitle = std::make_shared<c_media_file_ffst>(path_tmp);
            break;
        }
    }

    // Debug
    #if 0
    if (m_subtitle) {
        for (auto& entry : m_subtitle->entries()) {
            std::string content = boost::algorithm::join(entry.content, ", ");
            std::cout << boost::format("Subtitle: * ts_s = %f, ts_e = %f, content = [%s]") %
                entry.ts_s % entry.ts_e % content << std::endl;
        }
    }
    #endif
}

c_media_file::~c_media_file()
{

}

// Dispatch
void c_media_file::dispatch()
{
    if (!m_video)
        return;

    if (m_playback_mode == PLAYBACK_PLAY) {
        // Play
        double curr = time_now();
        m_playback_play_frame = std::max<double>(std::min<double>(m_playback_play_speed * (curr - m_playback_play_point), m_video->frames()), 0.0);
        m_playback_play_point = curr;

        // Mode
        m_playback_point = std::max<double>(std::min<double>(m_playback_play_frame, m_video->frames()), 0.0);
        m_playback_frame = std::max<int64_t>(std::min<int64_t>(m_playback_point, m_video->frames() - 1), 0);
    }
}

// Stop playback
void c_media_file::playback_stop()
{
    m_playback_mode = PLAYBACK_STOP;
}

// Start playback
void c_media_file::playback_play(double speed, double frame)
{
    m_playback_mode = PLAYBACK_PLAY;
    m_playback_play_speed = speed;
    m_playback_play_point = time_now();
    if (frame >= 0.0)
        m_playback_play_frame = frame;
    else
        m_playback_play_frame = std::max<double>(std::min<double>(frame, m_video->frames()), 0.0);
}
