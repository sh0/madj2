#
# Aubio libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for aubio")

# Find
pkg_check_modules(AUBIO aubio)
info_library("aubio" AUBIO FATAL_ERROR)
