#  Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.5)
project(OsAbstraction)

# adding Boost support library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libBoost)
file(GLOB_RECURSE  BOOST_COMMON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libBoost/libCommon/*h)
file(GLOB_RECURSE  BOOST_JSONPARSER_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libBoost/libJsonParser/*h)

# install OsAbstraction files
set(BOOST_HEADERS
    ${BOOST_JSONPARSER_HEADERS}
    ${BOOST_COMMON_HEADERS}
)