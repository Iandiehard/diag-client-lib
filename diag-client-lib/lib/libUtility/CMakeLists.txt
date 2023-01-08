#  Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(utility)

#set default language
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# find needed packages
find_package(automotive-dlt REQUIRED)

file(GLOB STATE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/utility/*.cpp")

add_library(${PROJECT_NAME} INTERFACE)

add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)
