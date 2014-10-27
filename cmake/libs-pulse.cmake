#
# Pulseaudio libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for Pulseaudio")

# Find
pkg_check_modules(PULSE libpulse)
info_library("Pulse" PULSE FATAL_ERROR)
