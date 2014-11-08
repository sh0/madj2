/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mm_main.h"

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

    // Check existence
    if (!boost::filesystem::exists(path) || !boost::filesystem::is_directory(path))
        throw c_exception("Media: Media path does not exist!", { throw_format("path", path) });

    // Recursive scan
    try {
        boost::filesystem::recursive_directory_iterator iter(path);
        for (; iter != boost::filesystem::recursive_directory_iterator(); ++iter) {
            if (iter->status().type() == boost::filesystem::file_type::regular_file) {
                // Get extension
                if (!iter->path().has_extension())
                    continue;
                auto ext = iter->path().extension().native();
                boost::algorithm::to_lower(ext);

                // File type selection
                if (ext == ".mjv") {
                    m_file_video.push_back(iter->path());
                } else if (ext == ".mp3") {
                    m_file_audio.push_back(iter->path());
                }
            }
        }
    } catch (const boost::filesystem::filesystem_error& ex) {
        throw_nested("Media: Filesystem error!");
    }

    // Message
    std::cout << boost::format("Media (%s): audio=%d, video=%d") % path % m_file_audio.size() % m_file_video.size() << std::endl;
}
