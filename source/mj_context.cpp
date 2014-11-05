/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_context.h"
#include "mm_main.h"
#include "io_main.h"
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
    c_global::media = std::make_shared<c_media>();
    c_global::io = std::make_shared<c_io>();
    c_global::video = std::make_shared<c_video>();

    #if 0
    c_global::audio = std::make_shared<c_audio>();
    c_global::extension = std::make_shared<c_extension>();
    c_global::workspace = std::make_shared<c_workspace>();
    #endif

    // Media
    if (auto pt_root = ptree.get_child_optional("media")) {
        for (auto& pt_path : *pt_root)
            c_global::media->media_add(pt_path.second.data());
    }

    // MIDI
    if (auto pt_root = ptree.get_child_optional("midi")) {
        for (auto& pt_midi : *pt_root) {
            c_global::io->midi_add(
                pt_midi.second.get<std::string>("name"),
                pt_midi.second.get<std::string>("color"),
                pt_midi.second.get<std::string>("device")
            );
        }
    }

    // Screen
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
    c_global::io.reset();
    c_global::media.reset();
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
        c_global::media->dispatch();
        c_global::io->dispatch();
        c_global::video->dispatch();
    }
}

#if 0
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
