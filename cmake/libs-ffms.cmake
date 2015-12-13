#
# FFMS libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for FFMS")

# Find
pkg_check_modules(FFMS ffms2)
if (NOT FFMS_FOUND)
    # Paths
    set(FFMS_PATH ${CMAKE_SOURCE_DIR}/libraries/ffms2)
    set(FFMS_PATH_LIBRARY ${FFMS_PATH}/build/src/core/.libs)
    set(FFMS_PATH_INCLUDE ${FFMS_PATH}/include)

    # Flags
    set(FFMS_FOUND TRUE)
    set(FFMS_CFLAGS "-I${FFMS_PATH_INCLUDE}")
    set(FFMS_LDFLAGS "${FFMS_PATH_LIBRARY}/libffms2.a -lz")
endif ()
info_library("FFMS" FFMS FATAL_ERROR)
