/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "config.hpp"
#include "global.hpp"
#include "context.hpp"
#include "media/media.hpp"
#include "video/video.hpp"
#include "controller/controller.hpp"

// Subsystems
std::shared_ptr<c_context> c_global::context;
std::shared_ptr<c_media> c_global::media;
std::shared_ptr<c_video> c_global::video;
std::shared_ptr<c_controller> c_global::controller;
