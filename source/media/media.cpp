/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "media/media.hpp"

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
                if (ext == ".mkv")
                    m_files.push_back(iter->path());
            }
        }
    } catch (const boost::filesystem::filesystem_error& ex) {
        throw_nested("Media: Filesystem error!");
    }

    // Message
    std::cout << boost::format("Media (%s): %d files") % path % m_files.size() << std::endl;
}
