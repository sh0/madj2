#
# ALSA libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for ALSA")

# Find
pkg_check_modules(ALSA alsa)
info_library("ALSA" ALSA FATAL_ERROR)
