#
# Boost libraries
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# Messages
message(STATUS "################################################")
message(STATUS "Checking for Boost")

# Settings
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# Find
find_package(Boost 1.40 COMPONENTS system filesystem program_options regex)
set(BOOST_FOUND TRUE)
foreach (_incs ${Boost_INCLUDE_DIRS})
    append_flags(BOOST_CFLAGS "-I${_incs}")
endforeach (_incs)
foreach (_libs ${Boost_LIBRARIES})
    append_flags(BOOST_LDFLAGS "${_libs}")
endforeach (_libs)
info_library("boost" BOOST FATAL_ERROR)
