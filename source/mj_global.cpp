/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

// Internal
#include "mj_config.h"
#include "mj_global.h"
#include "mj_context.h"
#include "mm_main.h"
#include "vo_main.h"
#include "io_main.h"

// Subsystems
std::shared_ptr<c_context> c_global::context;
std::shared_ptr<c_media> c_global::media;
std::shared_ptr<c_video> c_global::video;
std::shared_ptr<c_io> c_global::io;
