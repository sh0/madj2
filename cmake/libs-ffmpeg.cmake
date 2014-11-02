#
# FFmpeg libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for FFmpeg")

# Find
pkg_check_modules(FFMPEG libavutil libavformat libavcodec libavresample libswscale)
info_library("FFmpeg" FFMPEG FATAL_ERROR)
