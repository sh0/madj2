#
# MADJ project
# Copyright (C) 2014 Siim Meerits <siim@yutani.ee>
#

# CMake requirements
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)

# Paths
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(BINARY_DIR "${CMAKE_BINARY_DIR}")

# Check build directory
if (${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
	MESSAGE("Please do an out-of-tree build:")
	MESSAGE("rm CMakeCache.txt; mkdir build; cd build; cmake ..; make")
	MESSAGE(FATAL_ERROR "in-tree-build detected")
endif ()

# Project
project(madj)
set(DATA_PATH "${CMAKE_SOURCE_DIR}/data")

# Options
option(VERBOSE "Verbose CMake output" OFF)

# Tools and libraries
include(cmake-macros)
include(cmake-tools)
include(libs-boost)
include(libs-cegui)
include(libs-ffmpeg)
include(libs-opengl)
include(libs-glew)
include(libs-portmidi)
include(libs-sdl2)

# Subdirs
add_subdirectory(source)
