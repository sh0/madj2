/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_MJ_GLOBAL
#define H_MJ_GLOBAL

// Internal
#include "mj_config.h"

// Declarations
class c_video;
/*
class c_media;
class c_input;
class c_audio;
class c_workspace;
*/

// Global state class
class c_global : c_noncopiable
{
    public:
        // Subsystems
        static std::shared_ptr<c_video> video;
        /*
        static std::shared_ptr<c_media> media;
        static std::shared_ptr<c_input> input;
        static std::shared_ptr<c_audio> audio;
        static std::shared_ptr<c_workspace> workspace;
        */
};

#endif
