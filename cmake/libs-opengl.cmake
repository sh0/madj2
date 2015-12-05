#
# OpenGL libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for OpenGL")

# Find
pkg_check_modules(OPENGL gl)
info_library("OpenGL" OPENGL FATAL_ERROR)
