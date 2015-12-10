/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "media/file.hpp"
#include "media/file_mp4.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_media_file::c_media_file(boost::filesystem::path path)
{
    // Extension
    std::string path_ext = path.extension().native();
    boost::algorithm::to_lower(path_ext);

    // Load media
    if (path_ext == ".mp4") {
        m_video = std::make_shared<c_media_file_mp4>(path);
    } else {
        std::cout << "File: Unknown media file!" << std::endl;
    }
}

c_media_file::~c_media_file()
{

}
