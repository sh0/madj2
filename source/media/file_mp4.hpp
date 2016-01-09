/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE_MP4
#define H_MEDIA_FILE_MP4

// Internal
#include "config.hpp"
#include "media/file.hpp"
#include "opengl/texture.hpp"

// Boost
#include <boost/filesystem.hpp>

// Bento4
#include <Ap4.h>

// FFmpeg
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

// MP4 file demuxer
class c_media_file_mp4 : public c_media_video, boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file_mp4(boost::filesystem::path path);
        virtual ~c_media_file_mp4();

        // Info
        virtual int64_t frames() { return m_frames; }
        virtual double rate() { return 0.0; };
        virtual double aspect() { return 1.0; };
        virtual int width() { return m_width; }
        virtual int height() { return m_height; }

        // Read
        virtual std::shared_ptr<c_opengl_image> read(int64_t id);

    private:
        // Path
        boost::filesystem::path m_path;

        // File
        AP4_ByteStream* m_stream;
        std::unique_ptr<AP4_File> m_file;
        AP4_Movie* m_movie;
        AP4_Track* m_track;

        // Info
        int64_t m_frames;
        int m_width;
        int m_height;

        // Codec
        std::shared_ptr<AVCodecContext> m_codec_context;
        std::shared_ptr<AVFrame> m_codec_frame;

        // Scaling
        SwsContext* m_swscaler;
};

#endif
