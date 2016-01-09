/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_FILE
#define H_MEDIA_FILE

// Internal
#include "config.hpp"
#include "media/file.hpp"
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

        // Media
        std::shared_ptr<c_media_video> video() { return m_video; }
        std::shared_ptr<c_media_subtitle> subtitle() { return m_subtitle; }

    private:
        // Media
        std::shared_ptr<c_media_video> m_video;
        std::shared_ptr<c_media_subtitle> m_subtitle;
};

#endif
