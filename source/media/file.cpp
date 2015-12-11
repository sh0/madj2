/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file.hpp"
#include "media/file_mp4.hpp"
#include "media/file_ffms.hpp"
#include "media/file_srt.hpp"
#include "media/file_ffst.hpp"

// Boost
#include <boost/algorithm/string.hpp>

// Constructor and destructor
c_media_file::c_media_file(boost::filesystem::path path)
{
    // Load video
    std::string path_ext = path.extension().native();
    boost::algorithm::to_lower(path_ext);
    if (path_ext == ".mp4" || path_ext == ".mkv" || path_ext == ".avi") {
        //m_video = std::make_shared<c_media_file_mp4>(path);
        m_video = std::make_shared<c_media_file_ffms>(path);
    } else {
        std::cout << "File: Unknown media file!" << std::endl;
    }

    // Load subtitles
    std::vector<boost::filesystem::path> path_subs = { ".srt", ".ass", ".ssa" };
    for (auto path_sub : path_subs) {
        auto path_tmp = path;
        path_tmp.replace_extension(path_sub);
        if (boost::filesystem::is_regular_file(path_tmp)) {
            m_subtitle = std::make_shared<c_media_file_ffst>(path_tmp);
            break;
        }
    }

    // Debug
    #if 1
    if (m_subtitle) {
        for (auto& entry : m_subtitle->entries()) {
            std::string content = boost::algorithm::join(entry.content, ", ");
            std::cout << boost::format("Subtitle: * ts_s = %f, ts_e = %f, content = [%s]") %
                entry.ts_s % entry.ts_e % content << std::endl;
        }
    }
    #endif
}

c_media_file::~c_media_file()
{

}
