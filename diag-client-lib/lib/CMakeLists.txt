#  Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.5)
project(ara-lib)

# adding common library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libCommon)
# adding utility library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libUtility)
# adding uds transport layer 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libUdsTransportLayerApi)
# adding Boost support library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libBoost)
# adding Doip Client library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libDoIP)