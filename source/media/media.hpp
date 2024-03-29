/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MEDIA_MEDIA
#define H_MEDIA_MEDIA

// Internal
#include "config.hpp"

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// Media manager class
class c_media : boost::noncopyable
{
    public:
        // Constructor
        c_media();

        // Dispatch
        void dispatch() { }

        // Scan media
        void media_add(std::string path);
        const std::vector<boost::filesystem::path>& media_files() { return m_files; }

    private:
        // Files
        std::vector<boost::filesystem::path> m_files;
};

#endif
