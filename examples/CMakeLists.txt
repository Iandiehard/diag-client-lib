#  Diagnostic Client library CMake File
#  Copyright (C) 2024  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.5)

# build example_1
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example_1)

# build example_2
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example_2)
