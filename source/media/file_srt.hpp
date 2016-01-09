/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_SRT
#define H_MEDIA_FILE_SRT

// Internal
#include "config.hpp"
#include "media/file.hpp"

// Boost
#include <boost/filesystem.hpp>

// SubRip format subtitles
class c_media_file_srt : public c_media_subtitle, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file_srt(boost::filesystem::path path);
        virtual ~c_media_file_srt() { }

        // Entries
        virtual std::vector<c_media_subtitle::s_entry>& entries() { return m_entries; }

    private:
        // Content
        std::vector<c_media_subtitle::s_entry> m_entries;
};

#endif
