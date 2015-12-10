/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE
#define H_MEDIA_FILE

// Internal
#include "config.hpp"
#include "opengl/texture.hpp"

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
        virtual int width() = 0;
        virtual int height() = 0;

        // Read
        virtual std::shared_ptr<c_opengl_texture_2d> read(int64_t id) = 0;
};

// Media files
class c_media_file : boost::noncopyable
{
    public:
        // Constructor and destructor
        c_media_file(boost::filesystem::path path);
        ~c_media_file();

        // Video
        std::shared_ptr<c_media_video> video() { return m_video; }

    private:
        // Video
        std::shared_ptr<c_media_video> m_video;
};

#endif
