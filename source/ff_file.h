/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_FF_FILE
#define H_FF_FILE

// Internal
#include "mj_config.h"
#include "ff_file_track.h"
#include "ff_file_video.h"
#include "ff_file_audio.h"

// Boost
#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

// FFmpeg
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// File class
class c_file : c_noncopiable
{
    public:
        // Constructor and destructor
        c_file(const boost::filesystem::path& path);
        ~c_file();

        // Timing
        double time_duration() { return m_time_duration; }

        // Tracks
        std::vector<std::shared_ptr<c_file_video>>& track_video() { return m_track_video; }
        std::vector<std::shared_ptr<c_file_audio>>& track_audio() { return m_track_audio; }

    private:
        // Path
        boost::filesystem::path m_path;

        // File
        boost::interprocess::file_mapping m_file;
        boost::interprocess::mapped_region m_region;
        const uint8_t* m_data;
        uint64_t m_offset;
        uint64_t m_size;

        // File reading
        uint8_t read_u8();
        uint16_t read_u16();
        uint32_t read_u32();
        uint64_t read_u64();
        void read_check(uint64_t size);
        void read_data(uint8_t* data, uint32_t size);

        // Tracks
        std::vector<std::shared_ptr<c_file_video>> m_track_video;
        std::vector<std::shared_ptr<c_file_audio>> m_track_audio;

        // Timing
        double m_time_duration;
};

#endif
