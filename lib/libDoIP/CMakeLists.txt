#  MANDAREIN Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(doip-client)

#set default language
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# find needed packages
find_package(automotive-dlt REQUIRED)

# find DoIP sources
file(GLOB LIBDOIP_CHANNEL_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/channel/*.cpp")
file(GLOB LIBDOIP_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp")
file(GLOB LIBDOIP_HANDLER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/handler/*.cpp")
file(GLOB LIBDOIP_SOCKETS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/sockets/*.cpp")
file(GLOB LIBDOIP_CONNECTION_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/connection/*.cpp")
file(GLOB LIBDOIP_MAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(LIBDOIP_LIB_SRCS
    ${LIBDOIP_CHANNEL_SRCS}
    ${LIBDOIP_HANDLER_SRCS}
    ${LIBDOIP_SOCKETS_SRCS}
    ${LIBDOIP_TIMER_SRCS}
    ${LIBDOIP_CONNECTION_SRCS}
    ${LIBDOIP_MAIN_SRCS}
)

add_library(${PROJECT_NAME}
    ${LIBDOIP_LIB_SRCS}
)

add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# link library
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ara-lib::common
  ara-lib::utility
  ara-lib::uds-transport-layer-api
  ara-lib::boost-support
  Genivi::dlt
)

install(TARGETS ${PROJECT_NAME}
    EXPORT DiagClientLibTargets
)
