/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_FFMS
#define H_MEDIA_FILE_FFMS

// Internal
#include "config.hpp"
#include "media/file.hpp"
#include "opengl/texture.hpp"

// FFMS2
#include <ffms.h>

// FFMS2 file demuxer
class c_media_file_ffms : public c_media_video, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file_ffms(boost::filesystem::path path);
        virtual ~c_media_file_ffms();

        // Info
        virtual int64_t frames() { return m_frames; }
        virtual double rate() { return m_rate; }
        virtual double aspect() { return m_aspect; }
        virtual int width() { return m_width; }
        virtual int height() { return m_height; }

        // Read
        virtual std::shared_ptr<c_opengl_image> read(int64_t id);

    private:
        // Path
        boost::filesystem::path m_path;

        // Errors
        std::array<char, 1024> m_ffmsg;
        FFMS_ErrorInfo m_fferr;

        // File
        FFMS_VideoSource* m_source;

        // Info
        int64_t m_frames;
        double m_rate;
        double m_aspect;
        int m_width;
        int m_height;
};

#endif
