#
# OpenGL libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for OpenGL")

# Find
if (APPLE)
    set(OPENGL_FOUND TRUE)
    set(OPENGL_CFLAGS "")
    set(OPENGL_LDFLAGS "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo")
elseif (UNIX)
    pkg_check_modules(OPENGL gl)
endif ()
info_library("OpenGL" OPENGL FATAL_ERROR)
