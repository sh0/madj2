/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_FFST
#define H_MEDIA_FILE_FFST

// Internal
#include "config.hpp"
#include "media/file.hpp"

// Boost
#include <boost/filesystem.hpp>

// FFmpeg
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// FFmpeg subtitles reading
class c_media_file_ffst : public c_media_subtitle, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file_ffst(boost::filesystem::path path);
        virtual ~c_media_file_ffst() { }

        // Entries
        virtual std::vector<c_media_subtitle::s_entry>& entries() { return m_entries; }

    private:
        // Entries
        std::vector<c_media_subtitle::s_entry> m_entries;
};

#endif
