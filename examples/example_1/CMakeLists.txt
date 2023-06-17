#  Diagnostic Client library CMake File
#  Copyright (C) 2023  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.5)

project(diag-client-example-1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find needed packages
find_package(Threads REQUIRED)

#find main sources
file(GLOB MAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}
        ${MAIN_SRCS}
        )

# diag client lib headers
set(diag_client_header_path ${CMAKE_CURRENT_SOURCE_DIR}/../diag_client_lib/build-bin/diag-client/lib/include)

# Src headers
set(src_header_path src)


target_include_directories(${PROJECT_NAME} PRIVATE
        ${src_header_path}
        ${diag_client_header_path}
        )

# link all dynamic libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        diag-client
        platform-core
        Threads::Threads
        )
