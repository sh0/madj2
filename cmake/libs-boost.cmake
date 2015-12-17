#
# Boost libraries
# Copyright (C) 2015 Siim Meerits <siim@yutani.ee>
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
    append_flags(BOOST_CFLAGS -I${_incs})
endforeach (_incs)
set(BOOST_LDFLAGS ${Boost_LIBRARIES})
info_library("boost" BOOST FATAL_ERROR)
