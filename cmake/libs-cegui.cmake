#
# CEGUI libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for CEGUI")

# Find
pkg_check_modules(CEGUI CEGUI-0 CEGUI-0-OPENGL3)
if (NOT CEGUI_FOUND)
    pkg_check_modules(CEGUI CEGUI-9999 CEGUI-9999-OPENGL3)
endif ()
info_library("CEGUI" CEGUI FATAL_ERROR)
