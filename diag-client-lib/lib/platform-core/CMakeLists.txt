#  Diagnostic Client library CMake File
#  Copyright (C) 2024  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(platform-core)

include(GNUInstallDirs)

# Set default language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find sources
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/core/include/*.h")
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.cpp")

add_library(${PROJECT_NAME}
        ${HEADERS}
        ${SOURCES}
)

#Link include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)
