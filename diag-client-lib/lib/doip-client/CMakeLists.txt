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

file(GLOB LIBDOIP_TCP_CHANNEL_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/channel/tcp_channel/*.cpp")
file(GLOB LIBDOIP_UDP_CHANNEL_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/channel/udp_channel/*.cpp")
file(GLOB LIBDOIP_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp")
file(GLOB LIBDOIP_SOCKETS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/sockets/*.cpp")
file(GLOB LIBDOIP_CONNECTION_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/connection/*.cpp")
file(GLOB LIBDOIP_ERROR_DOMAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/error_domain/*.cpp")
file(GLOB LIBDOIP_MAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(LIBDOIP_LIB_SRCS
        ${LIBDOIP_TCP_CHANNEL_SRCS}
        ${LIBDOIP_UDP_CHANNEL_SRCS}
        ${LIBDOIP_COMMON_SRCS}
        ${LIBDOIP_HANDLER_SRCS}
        ${LIBDOIP_SOCKETS_SRCS}
        ${LIBDOIP_TIMER_SRCS}
        ${LIBDOIP_CONNECTION_SRCS}
        ${LIBDOIP_ERROR_DOMAIN_SRCS}
        ${LIBDOIP_MAIN_SRCS}
)

add_library(${PROJECT_NAME}
        ${LIBDOIP_LIB_SRCS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# link library
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
