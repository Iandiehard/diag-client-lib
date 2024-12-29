# Diagnostic Client library CMake File
# Copyright (C) 2024  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(doip-client)

#set default language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME}
        ${HEADERS}
        ${SOURCES}
)

#Link include directory
target_include_directories(${PROJECT_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

#link target directory
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        platform-core
        utility-support
        uds-transport-layer-api
        boost-support
)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)
