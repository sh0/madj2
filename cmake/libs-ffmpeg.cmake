#
# FFmpeg libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for FFmpeg")

# Find libavutil
pkg_check_modules(AVUTIL libavutil)
info_library("libavutil" AVUTIL FATAL_ERROR)

# Find libavformat
pkg_check_modules(AVFORMAT libavformat)
info_library("libavformat" AVFORMAT FATAL_ERROR)

# Find libavcodec
pkg_check_modules(AVCODEC libavcodec)
info_library("libavcodec" AVCODEC FATAL_ERROR)

# Find libavresample
pkg_check_modules(AVRESAMPLE libavresample)
info_library("libavresample" AVRESAMPLE FATAL_ERROR)

# Find libswscale
pkg_check_modules(SWSCALE libswscale)
info_library("libswscale" SWSCALE FATAL_ERROR)

