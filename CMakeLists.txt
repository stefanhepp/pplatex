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

