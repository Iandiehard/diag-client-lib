#  Diagnostic Client library CMake File
#  Copyright (C) 2024  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(doip-client)

#set default language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE LIBDOIP_LIB_HDRS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE LIBDOIP_LIB_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${PROJECT_NAME}
        ${LIBDOIP_LIB_SRCS}
        ${LIBDOIP_LIB_HDRS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

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
