#  MANDAREIN Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(BoostSupportLib)

#set default language
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

# find needed packages
find_package(automotive-dlt REQUIRED)
find_package(Boost 1.78.0)

file(GLOB LIBBOOST_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libCommon/*.cpp")
file(GLOB LIBBOOST_SOCKET_TCP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/tcp/*.cpp")
file(GLOB LIBBOOST_SOCKET_UDP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/udp/*.cpp")
file(GLOB LIBBOOST_SOCKET_ASYNC_TIMER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libTimer/oneShotAsync/*.cpp")
file(GLOB LIBBOOST_SOCKET_SYNC_TIMER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libTimer/oneShotSync/*.cpp")
file(GLOB LIBBOOST_JSON_PARSER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libJsonParser/*.cpp")

set(LIBBOOST_SOCKET_SRCS
    ${LIBBOOST_SOCKET_TCP_SRCS}
    ${LIBBOOST_SOCKET_UDP_SRCS}
)

add_library(${PROJECT_NAME}
    ${LIBBOOST_COMMON_SRCS}
    ${LIBBOOST_SOCKET_SRCS}
    ${LIBBOOST_SOCKET_ASYNC_TIMER_SRCS}
    ${LIBBOOST_SOCKET_SYNC_TIMER_SRCS}
    ${LIBBOOST_JSON_PARSER_SRCS}
)

add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#include header
set(boost_header_path 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
set(BoostSupportLib_Header
    ${boost_header_path}
)
#Link include directory
target_include_directories(${PROJECT_NAME} PUBLIC 
  "$<BUILD_INTERFACE:${BoostSupportLib_Header}>"
  "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>"
)
#link target directory
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ara-lib::CommonLibrary
  Genivi::dlt
  ${Boost_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)