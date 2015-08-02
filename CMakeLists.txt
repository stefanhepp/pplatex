#
# Author: Stefan Hepp <stefan@stefant.org>
#
# CMake build scripts for pplatex. 
#
# This is an alternative build script for the SCons based build script.
# It is recommended to build in a separate directory. Assuming this git
# repository has been checked out to 'pplatex', run:
#   mkdir ../build
#   cd ../build
#   cmake ../pplatex
#   make
#
# Requirements on Linux are:
# - pkg-config
# - libpcre3-dev
# Requirements on Windows are:
# - MSVC Compiler (should work with Cygwin/MinGW as well)
# - PCRE (statically linked)
#
# You can compile PCRE on Windows using cmake from the source distribution file.
# If you link with a dynamic PCRE library, remove the -DPCRE_STATIC definition in
# src/CMakeLists.txt
# 
# To compile on Windows, create a build directory and run cmake-gui <this-repo-dir> 
# in that build directory. Set PCRE_INCLUDE_DIR to the directory containing pcre.h, and
# set PCRE_PCREPOSIX_LIBRARY and PCRE_PCRE_LIBRARY to the pcreposix.lib and pcre.lib
# filename, respectively. Set the CMAKE_BUILD_TYPE to 'Release' (optional), and use the
# nmake or make generator, then run nmake (or make) in the build directory, after generating
# the build files.
#
# TODO: Adapt FindPCRE.cmake to choose static or dynamic linking via a CMake option
#       and set -DPCRE_STATIC accordingly.
#

# Minimum requirement is probably much lower, but lets say 3.0 for now.
cmake_minimum_required(VERSION 3.0)

# Main project name
project(pplatex)

# Load module to find PCRE
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find and configure PCRE library
find_package(PCRE REQUIRED)

# Build pplatex
add_subdirectory(src)

