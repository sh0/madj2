/*
 * MADJ project
 * Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_GLOBAL
#define H_GLOBAL

// Internal
#include "config.hpp"

// Declarations
class c_context;
class c_media;
class c_video;
class c_controller;

// Global state class
class c_global : boost::noncopyable
{
    public:
        // Subsystems
        static std::shared_ptr<c_context> context;
        static std::shared_ptr<c_media> media;
        static std::shared_ptr<c_video> video;
        static std::shared_ptr<c_controller> controller;
};

#endif
