#
# SDL2 libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for SDL2")

# Find
pkg_check_modules(SDL2 sdl2)
info_library("SDL2" SDL2 FATAL_ERROR)
