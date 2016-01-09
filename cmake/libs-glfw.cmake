#
# GLFW libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for GLFW")

# Find
pkg_check_modules(GLFW glfw3)
info_library("GLFW" GLFW FATAL_ERROR)
