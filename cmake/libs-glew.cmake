#
# GLEW libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for GLEW")

# Find
pkg_check_modules(GLEW glew)
info_library("GLEW" GLEW FATAL_ERROR)
