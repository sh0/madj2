/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_context.h"

// C
#include <csignal>

// Boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// Instance
std::weak_ptr<c_context> g_madj;

// Signal handler
static void g_signal(int sig)
{
    if (sig == SIGINT) {
        // Message
        std::cout << "\r" << "Signal: Caught SIGINT, quitting!" << std::endl;

        // Kill instance
        if (auto madj = g_madj.lock()) {
            madj->kill();
            g_madj.reset();
        }
    }
}

// Main
int main(int argc, char** argv)
{
    // Command-line option parsing
    boost::program_options::options_description opt_desc("madj options");
    opt_desc.add_options()
        ("help", "options help")
        ("config,c", boost::program_options::value<std::string>(), "configuration file path")
    ;
    boost::program_options::variables_map opt_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt_desc), opt_map);
    boost::program_options::notify(opt_map);

    // Help option
    if (opt_map.count("help")) {
        std::cout << opt_desc << std::endl;
        return 1;
    }

    // Default path
    boost::filesystem::path opt_config_path(MJ_DATA_PATH);
    opt_config_path /= "config.json";

    // Configuration and log file
    std::string opt_config = opt_config_path.native();
    if (opt_map.count("config"))
        opt_config = opt_map["config"].as<std::string>();

    // Banner
    std::cout << "                      _  _ " << std::endl;
    std::cout << "                     | |(_)" << std::endl;
    std::cout << "  _ __ ___   __ _  __| | _ " << std::endl;
    std::cout << " | '_ ` _ \\ / _` |/ _` || |" << std::endl;
    std::cout << " | | | | | | (_| | (_| || |" << std::endl;
    std::cout << " |_| |_| |_|\\__,_|\\__,_|| |" << std::endl;
    std::cout << "                       _/ |" << std::endl;
    std::cout << "                      |__/ " << std::endl;
    std::cout << boost::format("Build %s %s") % __TIME__ % __DATE__ << std::endl;

    // Create context
    std::shared_ptr<c_context> madj;
    try {
        madj = std::make_shared<c_context>(opt_config);
        g_madj = madj;
    } catch (c_exception& ex) {
        msg_exception(ex, "Main: Failed to initialize main context, aborting!");
        return 1;
    }

    // Signals setup
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = g_signal;
    sa.sa_flags = SA_NODEFER | SA_RESETHAND;
    if (sigaction(SIGINT, &sa, nullptr) < 0)
        std::cout << "Main: Failed to set signal handler!" << std::endl;

    // Loop
    madj->run();

    // Close
    madj.reset();
    return 0;
}

