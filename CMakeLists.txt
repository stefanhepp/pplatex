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
# - libpcre2-dev (Debian/Ubuntu) or pcre2 (Homebrew)
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

# CMake 4.0 removed compatibility with projects requiring < 3.5, so the old
# 3.0 floor made this project unconfigurable. 3.20 is a conservative modern
# baseline: it ships in Debian 12, Ubuntu 22.04 and RHEL 9.
cmake_minimum_required(VERSION 3.20)

# Main project name
project(pplatex)

# Find PCRE2 via its POSIX compatibility layer. pkg-config is the only
# discovery mechanism PCRE2 ships, so fall back to a manual search for
# platforms without it (typically Windows).
find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
  pkg_check_modules(PCRE2 QUIET IMPORTED_TARGET libpcre2-posix)
endif()

if (PCRE2_FOUND)
  set(PCRE2_LINK_LIBRARIES PkgConfig::PCRE2)
else()
  find_path(PCRE2_INCLUDE_DIR pcre2posix.h)
  find_library(PCRE2_POSIX_LIBRARY NAMES pcre2-posix pcre2-posix-static)
  find_library(PCRE2_LIBRARY NAMES pcre2-8 pcre2-8-static)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(PCRE2 DEFAULT_MSG
    PCRE2_INCLUDE_DIR PCRE2_POSIX_LIBRARY PCRE2_LIBRARY)

  set(PCRE2_LINK_LIBRARIES ${PCRE2_POSIX_LIBRARY} ${PCRE2_LIBRARY})
  set(PCRE2_INCLUDE_DIRS ${PCRE2_INCLUDE_DIR})
  mark_as_advanced(PCRE2_INCLUDE_DIR PCRE2_POSIX_LIBRARY PCRE2_LIBRARY)
endif()

# Build pplatex
add_subdirectory(src)

