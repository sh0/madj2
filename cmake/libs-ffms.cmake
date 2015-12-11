#
# FFMS libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for FFMS")

# Find
pkg_check_modules(FFMS ffms2)
info_library("FFMS" FFMS FATAL_ERROR)
