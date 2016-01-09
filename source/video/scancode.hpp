/*
 * MADJ project
 * Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_VIDEO_SCANCODE
#define H_VIDEO_SCANCODE

// Internal
#include "config.hpp"
#include "opengl/opengl.hpp"

// Convert SDL2 scancodes and text to CEGUI values
int g_convert_sdl_scancode_to_cegui(int scan);
std::vector<Uint32> g_convert_sdl_textinput_to_cegui(std::string utf8str);

#endif
