INCLUDE (CheckIncludeFiles)

project(VPL)

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

######################################################################
# Options
######################################################################

option(OPTION_SSE32 "Enable compilation of sse code using nasm" OFF)
  find_program(NASM_EXECUTABLE nasm)
set(USE_SSE_ 0)

option(OPTION_SSE64 "Enable compilation of sse code using nasm" OFF)
  find_program(NASM_EXECUTABLE nasm)
set(USE_SSE_ 0)

# Allow selection of static/dynamic libraries
option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)
option(BUILD_STATIC_LIBS "Build Static Libraries" OFF)

# Build rendering code
OPTION(BUILD_RENDERER "Build Rendering Library" ON)

# Profiling with GPROF
option(GPROF_PROFILING "Profiling with GPROF" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" )
endif()

######################################################################
# Preprocessor defines
######################################################################

if(CMAKE_COMPILER_IS_GNUCXX)

  if(GPROF_PROFILING)
  message(STATUS "Enabling profiling flags for GCC")
  add_definitions("-pg")
  set(CMAKE_MODULE_LINKER_FLAGS "-pg")
  set(CMAKE_SHARED_LINKER_FLAGS "-pg")
  set(CMAKE_EXE_LINKER_FLAGS "-pg")
  endif(GPROF_PROFILING)

  add_definitions("-ansi -std=c++98 -Wall -W")

  # add debug definitions
  if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  add_definitions("-D_DEBUG")
  endif()

endif(CMAKE_COMPILER_IS_GNUCXX)

if(MSVC)
  add_definitions(/D _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

######################################################################
# Header check
######################################################################

CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)

add_subdirectory(base)
add_subdirectory(render)


#############################################################################
# Installation
#############################################################################

install(FILES ${VPL_HEADERS} ${VPL_RENDER_HEADERS}  DESTINATION include COMPONENT headers)

set(CPACK_PACKAGE_NAME "vpl")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vpl - Vector Path Library")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "vpl")

# This must always be last!
include(CPack)
