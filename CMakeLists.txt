#  Diagnostic Client library CMake File
#  Copyright (C) 2023  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Entry Project CMake
#
cmake_minimum_required(VERSION 3.5)
project(diag-client)

# Cmake options
option(BUILD_SHARED_LIBS "Option to build as shared library" OFF)
option(BUILD_WITH_DLT "Option to use Dlt for logging" OFF)
option(BUILD_DOXYGEN "Option to generate doxygen file" OFF)
option(BUILD_WITH_TEST "Option to build test target" OFF)
option(BUILD_EXAMPLES "Option to build example targets" ON)
option(BUILD_WIN_SYSTEM "Option to build within windows" OFF)

SET(BOOST_PATH "./" CACHE STRING "select boost path")
message("boost library path:${BOOST_PATH}")

# add compiler preprocessor flag when dlt enabled
if (BUILD_WITH_DLT)
    add_compile_definitions(ENABLE_DLT_LOGGER)
    message("Dlt logging enabled in diag-client library")
endif (BUILD_WITH_DLT)

if (BUILD_WIN_SYSTEM)
    set(ENABLE_WIN_SYSTEM "ON")
    add_definitions(-DBOOST_PATH=${BOOST_PATH})
    message("Enable Definition of the build in Windows System")
endif (BUILD_WIN_SYSTEM)

# Build diag-client library
add_subdirectory(diag-client-lib)

# Build diag-client doxygen documentation
if (BUILD_DOXYGEN)
    add_subdirectory(docs)
endif (BUILD_DOXYGEN)

# Build diag-client test targets
if (BUILD_WITH_TEST)
    add_subdirectory(test)
endif (BUILD_WITH_TEST)

# Build diag-client example targets
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (BUILD_EXAMPLES)
