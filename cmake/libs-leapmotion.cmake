#
# Leap Motion libraries
# Copyright (C) 2016 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for Leap Motion")

# Paths
set(LEAPMOTION_PATH ${CMAKE_SOURCE_DIR}/libraries/leapmotion)
set(LEAPMOTION_PATH_INCLUDE ${LEAPMOTION_PATH}/include)
if (APPLE)
    set(LEAPMOTION_PATH_LIBRARY ${LEAPMOTION_PATH}/lib-osx)
elseif (UNIX)
    set(LEAPMOTION_PATH_LIBRARY ${LEAPMOTION_PATH}/lib-linux/x64)
endif ()

# Set flags
if (EXISTS "${LEAPMOTION_PATH_INCLUDE}/Leap.h")
    set(LEAPMOTION_FOUND TRUE)
    set(LEAPMOTION_CFLAGS -I${LEAPMOTION_PATH_INCLUDE})
    set(LEAPMOTION_LDFLAGS -L${LEAPMOTION_PATH_LIBRARY} -lLeap)
    info_library("Leap Motion" LEAPMOTION FATAL_ERROR)
else ()
    set(LEAPMOTION_FOUND FALSE)
    set(LEAPMOTION_CFLAGS )
    set(LEAPMOTION_LDFLAGS )
    info_library("Leap Motion" LEAPMOTION FATAL_ERROR)
endif ()
