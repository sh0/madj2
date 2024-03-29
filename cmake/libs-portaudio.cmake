#
# PortAudio libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for PortAudio")

# Find
pkg_check_modules(PORTAUDIO portaudio-2.0)
string(REGEX REPLACE "-framework;" "-framework " PORTAUDIO_LDFLAGS "${PORTAUDIO_LDFLAGS}")
info_library("PortAudio" PORTAUDIO FATAL_ERROR)
