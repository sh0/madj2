/*
 * MADJ project
 * Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
 */

#ifndef H_IO_MAIN
#define H_IO_MAIN

// Internal
#include "mj_config.h"

// Main I/O class
class c_io : c_noncopiable
{
    public:
        // Input
        void input_keyboard(std::string key, bool value, bool mod_ctrl, bool mod_shift, bool mod_alt, bool mod_gui);
        void input_midi_key(std::string key, bool value);
        void input_midi_pot(std::string key, float value);

    private:

};

#endif
