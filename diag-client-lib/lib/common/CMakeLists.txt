#  Diagnostic Client library CMake File
#  Copyright (C) 2023  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(common)

include(GNUInstallDirs)

# Set default language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)

#set includes paths
set(header_path "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*h)
set(public_header ${HEADERS})
set(header ${public_header})

#Link include directories
target_include_directories(${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
        )
