#
# Portmidi libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for Portmidi")

# Find header
find_path(PORTMIDI_HEADER "portmidi.h")
if (PORTMIDI_HEADER)
    set(PORTMIDI_FOUND TRUE)
endif ()

# Set flags
set(PORTMIDI_CFLAGS "")
set(PORTMIDI_LDFLAGS "-lportmidi")
info_library("Portmidi" PORTMIDI FATAL_ERROR)
