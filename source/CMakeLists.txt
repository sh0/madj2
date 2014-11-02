#
# MADJ project
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Config
configure_file(
    "${SOURCE_DIR}/mj_cmake.h.in"
    "${BINARY_DIR}/mj_cmake.h"
)
include_directories(${SOURCE_DIR} ${BINARY_DIR})

# Sources
file(GLOB CORE_SOURCE ${SOURCE_DIR}/*.cpp)
file(GLOB CORE_HEADER ${SOURCE_DIR}/*.h)

# Core flags
append_flags(CORE_CFLAGS "-std=gnu++11 -Wall -march=native -pthread")
append_flags(CORE_CFLAGS "-O2 -ggdb -fno-omit-frame-pointer")
append_flags(CORE_CFLAGS "-Wpointer-arith -Wno-ctor-dtor-privacy -Winvalid-offsetof -Wno-strict-aliasing -Wno-unused-local-typedefs -Wno-deprecated")
append_flags(CORE_CFLAGS "${ALSA_CFLAGS}")
append_flags(CORE_CFLAGS "${BOOST_CFLAGS}")
append_flags(CORE_CFLAGS "${CEGUI_CFLAGS}")
append_flags(CORE_CFLAGS "${FFMPEG_CFLAGS}")
append_flags(CORE_CFLAGS "${GLEW_CFLAGS}")
append_flags(CORE_CFLAGS "${LADSPA_CFLAGS}")
append_flags(CORE_CFLAGS "${PULSE_CFLAGS}")
append_flags(CORE_CFLAGS "${SDL_CFLAGS}")

# Linker flags
append_flags(CORE_LDFLAGS "${ALSA_LDFLAGS}")
append_flags(CORE_LDFLAGS "${BOOST_LDFLAGS}")
append_flags(CORE_LDFLAGS "${CEGUI_LDFLAGS}")
append_flags(CORE_LDFLAGS "${FFMPEG_LDFLAGS}")
append_flags(CORE_LDFLAGS "${GLEW_LDFLAGS}")
append_flags(CORE_LDFLAGS "${LADSPA_LDFLAGS}")
append_flags(CORE_LDFLAGS "${PULSE_LDFLAGS}")
append_flags(CORE_LDFLAGS "${SDL_LDFLAGS}")

# Compile
set(CORE_EXEC "${PROJECT_NAME}")
add_executable(${CORE_EXEC} ${CORE_SOURCE})
get_target_property(_flags ${CORE_EXEC} COMPILE_FLAGS)
append_flags(_flags "${CORE_CFLAGS}")
set_target_properties(${CORE_EXEC} PROPERTIES COMPILE_FLAGS "${_flags}")
string(REPLACE " " ";" CORE_LDFLAGS ${CORE_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${CORE_LDFLAGS})

# Debug
message(STATUS "################################################")
if (VERBOSE)
    message(STATUS "Project CFLAGS: \"${CORE_CFLAGS}\"")
    message(STATUS "Project LDFLAGS: \"${CORE_LDFLAGS}\"")
    message(STATUS "################################################")
endif ()

# Install
install(TARGETS ${CORE_EXEC} DESTINATION bin)
