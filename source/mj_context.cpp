/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_context.h"
#include "vo_main.h"

// C++
#include <fstream>

// Boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

// Constructor and destructor
c_context::c_context(std::string config_fn)
{
    // Config file
    std::ifstream config_fs;
    config_fs.open(config_fn);
    if (!config_fs.is_open())
        throw c_exception("Failed to open configuration file!", { throw_format("path", config_fn) });
    std::string config_text;
    while (config_fs.good()) {
        std::string line;
        getline(config_fs, line);
        config_text += line + "\n";
    }
    config_fs.close();

    // Strip block (/* xxx */) style comments
    while (auto it_cs = boost::algorithm::find_first(config_text, "/" "*")) {
        it_cs = boost::make_iterator_range(it_cs.begin(), config_text.end());
        auto it_ce = boost::algorithm::find_first(it_cs, "*" "/");
        if (it_ce)
            it_cs = boost::make_iterator_range(it_cs.begin(), it_ce.end());
        for (auto it = it_cs.begin(); it != it_cs.end(); it++)
            if (*it != '\n')
                *it = ' ';
    }

    // Strip line (// xxx) style comments
    while (auto it_cs = boost::algorithm::find_first(config_text, "//")) {
        it_cs = boost::make_iterator_range(it_cs.begin(), config_text.end());
        auto it_ce = boost::algorithm::find_first(it_cs, "\n");
        if (it_ce)
            it_cs = boost::make_iterator_range(it_cs.begin(), it_ce.begin());
        for (auto it = it_cs.begin(); it != it_cs.end(); it++)
            *it = ' ';
    }

    // Parse config
    boost::property_tree::ptree ptree;
    try {
        std::istringstream stream(config_text);
        boost::property_tree::json_parser::read_json(stream, ptree);
    } catch (...) {
        throw_nested("Error parsing configuration!");
    }

    // Create subsystems
    c_global::video = std::make_shared<c_video>();

    #if 0
    c_global::media = std::make_shared<c_media>(MJ_DATA_PATH);
    c_global::input = std::make_shared<c_input>(this);
    c_global::audio = std::make_shared<c_audio>();
    c_global::extension = std::make_shared<c_extension>();
    c_global::workspace = std::make_shared<c_workspace>();

    // Media config
    if (auto pt_root = ptree.get_child_optional("media")) {
        std::vector<std::string> path_list;
        for (auto& pt_path : *pt_root)
            path_list.push_back(pt_path.second.data());
        config_media(path_list);
    }

    // Soundcard config
    if (auto pt_root = ptree.get_child_optional("soundcards")) {
        for (auto& pt_card : *pt_root) {

            std::vector<std::string> channels;
            if (auto pt_chan = pt_card.second.get_child_optional("channels")) {
                for (auto& pt_elem : *pt_chan)
                    channels.push_back(pt_elem.second.data());
            }

            config_soundcard(
                pt_card.second.get<std::string>("driver"),
                pt_card.second.get<std::string>("device"),
                channels
            );
        }
    }
    #endif

    // Screen config
    if (auto pt_root = ptree.get_child_optional("screens")) {
        for (auto& pt_screen : *pt_root) {
            c_global::video->screen_add(
                pt_screen.second.get<std::string>("name"),
                pt_screen.second.get<std::string>("color"),
                pt_screen.second.get<int>("view_cols"),
                pt_screen.second.get<int>("view_rows"),
                pt_screen.second.get<int>("pos_x"),
                pt_screen.second.get<int>("pos_y"),
                pt_screen.second.get<int>("width"),
                pt_screen.second.get<int>("height"),
                pt_screen.second.get<bool>("fullscreen")
            );
        }
    }

    #if 0
    // Twitter config
    if (auto pt_root = ptree.get_child_optional("twitter")) {
        for (auto& pt_twitter : *pt_root) {
            config_twitter(
                pt_twitter.second.get<std::string>("name"),
                pt_twitter.second.get<std::string>("follow"),
                pt_twitter.second.get<std::string>("track"),
                pt_twitter.second.get<std::string>("locations")
            );
        }
    }

    // Tracker workspaces
    if (auto pt_root = ptree.get_child_optional("ws-tracker")) {
        for (auto& pt_tracker : *pt_root) {
            std::vector<std::string> audio;
            if (auto pt_audio = pt_tracker.second.get_child_optional("audio")) {
                for (auto& pt_elem : *pt_audio)
                    audio.push_back(pt_elem.second.data());
            }

            config_workspace_tracker(
                pt_tracker.second.get<std::string>("name"),
                pt_tracker.second.get<std::string>("screen"),
                pt_tracker.second.get<int>("slot"),
                audio
            );
        }
    }

    // Joysticks
    if (auto pt_root = ptree.get_child_optional("joysticks")) {
        for (auto& pt_joy : *pt_root) {
            std::vector<std::string> buttons;
            if (auto pt_buttons = pt_joy.second.get_child_optional("buttons")) {
                for (auto& pt_elem : *pt_buttons)
                    buttons.push_back(pt_elem.second.data());
            }

            std::vector<std::string> axes;
            if (auto pt_axes = pt_joy.second.get_child_optional("axes")) {
                for (auto& pt_elem : *pt_axes)
                    axes.push_back(pt_elem.second.data());
            }

            config_joystick(
                pt_joy.second.get<std::string>("name"),
                pt_joy.second.get<std::string>("dev"),
                pt_joy.second.get<std::string>("color"),
                buttons,
                axes
            );
        }
    }

    // Keys
    if (auto pt_root = ptree.get_child_optional("keys")) {
        for (auto& pt_key : *pt_root) {
            std::vector<std::string> list;
            for (auto& pt_elem : pt_key.second)
                list.push_back(pt_elem.second.data());
            config_key(list);
        }
    }
    #endif

    // Main thread
    m_run = true;
}

c_context::~c_context()
{
    // Reset subsystems
    c_global::video.reset();
    #if 0
    c_global::workspace.reset();
    c_global::extension.reset();
    c_global::audio.reset();
    c_global::input.reset();
    c_global::media.reset();
    #endif
}

void c_context::run()
{
    // Profiler and timer
    c_time_thread timer(0);

    // Loop
    while (m_run.load()) {

        // Sleep
        timer.cycle();

        // Subsystems
        c_global::video->dispatch();
        #if 0
        c_global::input->dispatch();
        c_global::audio->dispatch();
        c_global::workspace->dispatch();
        #endif
    }
}

#if 0
// Config
bool c_context::config_media(
    std::vector<std::string> path_list
) {
    // Home path
    std::string path_home = getenv("HOME");

    // Loop paths
    for (auto path_item : path_list) {
        // Home path conversion
        if (path_item.empty())
            continue;
        if (path_item[0] == '~')
            boost::algorithm::replace_first(path_item, "~", path_home);

        // Convert path
        boost::filesystem::path pfs = path_item;

        // Add
        msg_warning(boost::format("Config: Media path: %1%") % pfs);
        c_global::media->media_add(pfs);
    }
    return true;
}

bool c_context::config_soundcard(
    std::string driver,
    std::string device,
    std::vector<std::string> channels
) {
    // Check
    if ((driver != "jack") && (driver != "pulse")) {
        msg_warning(
            "Config: Soundcard: Only drivers \"jack\" and "
            "\"pulse\" are supported!"
        );
        return false;
    }

    // Add
    msg_warning(
        boost::format("Config: Soundcard: driver=%s, device=%s, num_channels=%d") %
        driver % device % channels.size()
    );
    return c_global::audio->driver_add(driver, device, channels);
}

bool c_context::config_screen(
    std::string name,
    int32_t screen,
    int32_t view_cols,
    int32_t view_rows,
    int32_t width,
    int32_t height,
    bool fullscreen,
    std::string color
) {
    // Check
    if (name.empty()) {
        msg_warning("Config: Screen: Must specify name in \"name\" field!");
        return false;
    } else if (
        view_rows < 1 || view_rows > 10 ||
        view_cols < 1 || view_cols > 10
    ) {
        msg_warning(
            boost::format(
                "Config: Screen: Must have \"view_rows\" and \"view_cols\" "
                "fields both with values between 1 and 10! name=%s"
            ) % name
        );
        return false;
    }

    // Add
    msg_warning(
        boost::format(
            "Config: Screen! "
            "name=%s, screen=%d, view_cols=%d, view_rows=%d, "
            "width=%d, height=%d, fullscreen=%s, color=%s"
        ) % name % screen % view_cols % view_rows %
        width % height % (fullscreen ? "true" : "false") % color
    );
    return c_global::video->screen_add(
        name, screen, view_cols, view_rows,
        width, height, fullscreen, color
    );
}

bool c_context::config_workspace_tracker(
    std::string name,
    std::string screen,
    int32_t slot,
    std::vector<std::string> audio
) {
    // Check
    if (name.empty()) {
        msg_warning("Config: Tracker workspace must specify name in \"name\" field!");
        return false;
    } else if (screen.empty()) {
        msg_warning("Config: Tracker workspace must specify screen in \"screen\" field!");
        return false;
    }

    // Add
    msg_warning(
        boost::format(
            "Config: Tracker workspace! "
            "name=%s, screen=%s, slot=%d, num_audio=%d"
        ) % name % screen % slot % audio.size()
    );
    return c_global::workspace->tracker_add(name, screen, slot, audio);
}

bool c_context::config_workspace_twitter(
    std::string name,
    std::string screen,
    int32_t slot
) {
    // Check
    if (name.empty()) {
        msg_warning("Config: Twitter workspace must specify name in \"name\" field!");
        return false;
    } else if (screen.empty()) {
        msg_warning("Config: Twitter workspace must specify screen in \"screen\" field!");
        return false;
    }

    // Add
    msg_warning(
        boost::format("Config: Twitter workspace! name=%s, screen=%s, slot=%d") %
        name % screen % slot
    );
    return c_global::workspace->twitter_add(name, screen, slot);
}

bool c_context::config_joystick(
    std::string name,
    std::string dev,
    std::string color,
    std::vector<std::string> buttons,
    std::vector<std::string> axes
) {
    // Check
    if (name.empty()) {
        msg_warning("Config: Joystick must specify name in \"name\" field!");
        return false;
    }

    // Add
    msg_warning(
        boost::format(
            "Config: Joystick! "
             "name=%s, dev=%s, color=%s, num_buttons=%d, num_axes=%d"
        ) % name % dev % color % buttons.size() % axes.size()
    );
    return true;
}

bool c_context::config_key(
    std::vector<std::string> key
) {
    // Check
    if (key.size() != 4) {
        msg_warning(
            "Config: Key: Entry must have exactly 4 strings - "
            "device, item, scope, action!"
        );
        return false;
    }

    // Parametes
    std::string& dev = key[0];
    std::string& item = key[1];
    std::string& scope = key[2];
    std::string& action = key[3];

    // Add
    /*
    msg_warning(
        boost::format("Config: Key! device=%s, item=%s, scope=%s, action=%s") %
        dev % item % scope % action
    );
    */
    return c_global::input->bind_add(dev, item, action, scope);
}
#endif