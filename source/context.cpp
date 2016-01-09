/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "context.hpp"
#include "media/media.hpp"
#include "controller/controller.hpp"
#include "video/video.hpp"

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
    c_global::controller = std::make_shared<c_controller>();
    c_global::video = std::make_shared<c_video>();

    // Media
    if (auto pt_root = ptree.get_child_optional("media")) {
        for (auto& pt_path : *pt_root)
            c_global::media->media_add(pt_path.second.data());
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

    // Tracker workspaces
    if (auto pt_root = ptree.get_child_optional("trackers")) {
        for (auto& pt_tracker : *pt_root) {
            c_global::video->tracker_add(
                pt_tracker.second.get<std::string>("name"),
                pt_tracker.second.get<std::string>("screen"),
                pt_tracker.second.get<int>("pos_x"),
                pt_tracker.second.get<int>("pos_y"),
                pt_tracker.second.get<int>("pos_w"),
                pt_tracker.second.get<int>("pos_h")
            );
        }
    }

    #if 0
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
    #endif

    // Keys
    if (auto pt_root = ptree.get_child_optional("keys")) {
        for (auto& pt_key : *pt_root) {
            // List
            std::vector<std::string> list;
            for (auto& pt_elem : pt_key.second)
                list.push_back(pt_elem.second.data());
            if (list.size() < 4)
                continue;

            // Mapping
            std::string device = list[0];
            std::vector<std::string> keys;
            boost::split(keys, list[1], boost::is_any_of("+"));
            std::string target = list[2];
            std::string action = list[3];

            // Add
            c_global::controller->mapping_add(device, keys, target, action);
        }
    }

    // Main thread
    m_run = true;
}

c_context::~c_context()
{
    // Reset subsystems
    c_global::video.reset();
    c_global::controller.reset();
    c_global::media.reset();
}

void c_context::run()
{
    // Profiler and timer
    c_time_cyclic timer(g_time_fps2us(60));

    // Loop
    while (m_run.load()) {
        // Timer
        timer.cycle();

        // Subsystems
        c_global::media->dispatch();

        // Input
        c_global::controller->dispatch_input(timer);
        c_global::video->dispatch_input(timer);

        // Render
        c_global::controller->dispatch_render(timer);
        c_global::video->dispatch_render(timer);
    }
}
