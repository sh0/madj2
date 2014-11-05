/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mm_main.h"

// Boost
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

// Constructor
c_media::c_media()
{

}

// Scan media
void c_media::media_add(std::string path)
{
    // Home path
    std::string path_home = getenv("HOME");
    if (boost::starts_with(path, "~"))
        boost::algorithm::replace_first(path, "~", path_home);

    // Message
    std::cout << "Media: " << path << std::endl;
}
