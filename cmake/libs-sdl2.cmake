#
# SDL2 libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for SDL2")

# Find
pkg_check_modules(SDL sdl2)
info_library("SDL2" SDL FATAL_ERROR)
