/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MM_MAIN
#define H_MM_MAIN

// Internal
#include "mj_config.h"

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// Media manager class
class c_media : c_noncopiable
{
    public:
        // Constructor
        c_media();

        // Dispatch
        void dispatch() { }

        // Scan media
        void media_add(std::string path);

    private:
        // Files
        std::vector<boost::filesystem::path> m_file_audio;
        std::vector<boost::filesystem::path> m_file_video;
};

#endif
