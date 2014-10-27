#
# LADSPA libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for LADSPA")

# Find header
find_path(LADSPA_HEADER "ladspa.h")
if (LADSPA_HEADER)
    set(LADSPA_FOUND TRUE)
endif ()

# Set flags
set(LADSPA_CFLAGS "")
set(LADSPA_LDFLAGS "")
info_library("LADSPA" LADSPA FATAL_ERROR)
