/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "media/file_srt.hpp"

// C++
#include <iostream>
#include <fstream>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>

// Constructor and destructor
c_media_file_srt::c_media_file_srt(boost::filesystem::path path)
{
    // Open file
    std::ifstream fs(path.native());
    if (!fs.is_open())
        throw c_exception("SRT: Failed to open file!", { throw_format("path", path) });

    // State machine
    enum e_state {
        STATE_NONE,
        STATE_TIMESTAMP,
        STATE_CONTENT
    };
    e_state state = STATE_NONE;
    c_media_subtitle::s_entry entry;

    // Regexes
    const boost::regex regex_timestamp("^([0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2},[0-9]{1,}) --> ([0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2},[0-9]{1,})");

    // Read lines
    int line_id = 0;
    std::string line;
    while (std::getline(fs, line)) {
        // Line id
        line_id++;

        // Trim
        boost::algorithm::trim(line);

        // States
        if (state == STATE_NONE) {
            // Either empty line or sequence id
            if (line.empty())
                continue;
            try {
                int id = std::stoi(line);
                if (id < 0)
                    std::cout << boost::format("SRT: Invalid sequence id found, aborting! line = %d, path = %s") % line_id % path << std::endl;
                state = STATE_TIMESTAMP;
            } catch (std::invalid_argument ex) {
                std::cout << boost::format("SRT: Invalid sequence id found, aborting! line = %d, path = %s") % line_id % path << std::endl;
                return;
            } catch (std::out_of_range ex) {
                std::cout << boost::format("SRT: Invalid sequence id found, aborting! line = %d, path = %s") % line_id % path << std::endl;
                return;
            }
        } else if (state == STATE_TIMESTAMP) {
            // Match timestamps
            boost::smatch match;
            if (!boost::regex_search(line, match, regex_timestamp)) {
                std::cout << boost::format("SRT: Invalid timestamp line found, aborting! line = %d, path = %s") % line_id % path << std::endl;
                return;
            }

            // Parse timestamps
            auto timestamp = [](std::string token) {
                return 0.0;
            };
            entry.ts_s = timestamp(match.str(1));
            entry.ts_e = timestamp(match.str(2));
            if (entry.ts_s < 0.0 || entry.ts_e < 0.0) {
                std::cout << boost::format("SRT: Invalid timestamp values found, aborting! line = %d, path = %s") % line_id % path << std::endl;
                return;
            }

            // State
            state = STATE_CONTENT;

        } else if (state == STATE_CONTENT) {

        }
    }
}
