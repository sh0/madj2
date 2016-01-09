#
# MADJ project
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
#

# Config
configure_file(
    "${SOURCE_DIR}/cmake.hpp.in"
    "${BINARY_DIR}/cmake.hpp"
)
include_directories(${SOURCE_DIR} ${BINARY_DIR})

# Sources
file(GLOB_RECURSE CORE_SOURCE RELATIVE ${SOURCE_DIR} *.cpp)
file(GLOB_RECURSE CORE_HEADER RELATIVE ${SOURCE_DIR} *.hpp)

# Core flags
append_flags(CORE_CFLAGS "-std=gnu++11 -Wall -march=native -pthread")
append_flags(CORE_CFLAGS "-O2 -ggdb -fno-omit-frame-pointer")
append_flags(CORE_CFLAGS "-Wpointer-arith -Wno-ctor-dtor-privacy -Winvalid-offsetof -Wno-strict-aliasing -Wno-unused-local-typedefs -Wno-deprecated")
append_flags(CORE_CFLAGS "-D__FILENAME__='\"$(subst ${SOURCE_DIR}/,,$(abspath $<))\"'")
append_flags(CORE_CFLAGS "${BOOST_CFLAGS}")
append_flags(CORE_CFLAGS "${CEGUI_CFLAGS}")
append_flags(CORE_CFLAGS "${FFMPEG_CFLAGS}")
append_flags(CORE_CFLAGS "${FFMS_CFLAGS}")
append_flags(CORE_CFLAGS "${BENTO4_CFLAGS}")
append_flags(CORE_CFLAGS "${OPENGL_CFLAGS}")
append_flags(CORE_CFLAGS "${GLEW_CFLAGS}")
append_flags(CORE_CFLAGS "${GLFW_CFLAGS}")
append_flags(CORE_CFLAGS "${SDL2_CFLAGS}")
append_flags(CORE_CFLAGS "${AUBIO_CFLAGS}")
append_flags(CORE_CFLAGS "${PORTAUDIO_CFLAGS}")
append_flags(CORE_CFLAGS "${PORTMIDI_CFLAGS}")
append_flags(CORE_CFLAGS "${LEAPMOTION_CFLAGS}")

# Linker flags
append_flags(CORE_LDFLAGS "${BOOST_LDFLAGS}")
append_flags(CORE_LDFLAGS "${CEGUI_LDFLAGS}")
append_flags(CORE_LDFLAGS "${FFMPEG_LDFLAGS}")
append_flags(CORE_LDFLAGS "${FFMS_LDFLAGS}")
append_flags(CORE_LDFLAGS "${BENTO4_LDFLAGS}")
append_flags(CORE_LDFLAGS "${OPENGL_LDFLAGS}")
append_flags(CORE_LDFLAGS "${GLEW_LDFLAGS}")
append_flags(CORE_LDFLAGS "${GLFW_LDFLAGS}")
append_flags(CORE_LDFLAGS "${SDL2_LDFLAGS}")
append_flags(CORE_LDFLAGS "${AUBIO_LDFLAGS}")
append_flags(CORE_LDFLAGS "${PORTAUDIO_LDFLAGS}")
append_flags(CORE_LDFLAGS "${PORTMIDI_LDFLAGS}")
append_flags(CORE_LDFLAGS "${LEAPMOTION_LDFLAGS}")

# Compile
set(CORE_EXEC "${PROJECT_NAME}")
add_executable(${CORE_EXEC} ${CORE_SOURCE})
get_target_property(_flags ${CORE_EXEC} COMPILE_FLAGS)
append_flags(_flags "${CORE_CFLAGS}")
set_target_properties(${CORE_EXEC} PROPERTIES COMPILE_FLAGS "${_flags}")

# Linking
target_link_libraries(${CORE_EXEC} ${BOOST_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${CEGUI_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${FFMPEG_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${FFMS_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${BENTO4_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${OPENGL_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${GLEW_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${GLFW_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${SDL2_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${AUBIO_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${PORTAUDIO_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${PORTMIDI_LDFLAGS})
target_link_libraries(${CORE_EXEC} ${LEAPMOTION_LDFLAGS})

# Debug
message(STATUS "################################################")
if (VERBOSE)
    message(STATUS "Project CFLAGS: \"${CORE_CFLAGS}\"")
    message(STATUS "Project LDFLAGS: \"${CORE_LDFLAGS}\"")
    message(STATUS "################################################")
endif ()

# Install
install(TARGETS ${CORE_EXEC} DESTINATION bin)
