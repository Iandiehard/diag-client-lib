#  Diagnostic Client library CMake File
#  Copyright (C) 2023  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(boost-support)

#set default language
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.78.0)
find_package(OpenSSL)

file(GLOB LIBBOOST_COMMON_SRCS "src/boost-support/common/*.cpp")
file(GLOB LIBBOOST_CLIENT_TCP_SRCS "include/boost-support/client/tcp/*.cpp")
file(GLOB LIBBOOST_SOCKET_SRCS "src/boost-support/socket/*.cpp")
file(GLOB LIBBOOST_SOCKET_TCP_SRCS "src/boost-support/socket/tcp/*.cpp")
file(GLOB LIBBOOST_SOCKET_TLS_SRCS "src/boost-support/socket/tls/*.cpp")
file(GLOB LIBBOOST_SOCKET_UDP_SRCS "src/boost-support/socket/udp/*.cpp")
file(GLOB LIBBOOST_JSON_PARSER_SRCS "include/boost-support/parser/*.cpp")

set(LIBBOOST_SOCKET_SRCS
        ${LIBBOOST_SOCKET_SRCS}
        ${LIBBOOST_SOCKET_TCP_SRCS}
        ${LIBBOOST_SOCKET_TLS_SRCS}
        ${LIBBOOST_SOCKET_UDP_SRCS}
)

add_library(${PROJECT_NAME}
        ${LIBBOOST_COMMON_SRCS}
        ${LIBBOOST_CLIENT_TCP_SRCS}
        ${LIBBOOST_SOCKET_SRCS}
        ${LIBBOOST_JSON_PARSER_SRCS}
)

#Link include directory
target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>"
)

#link target directory
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        platform-core
        utility-support
        ${Boost_LIBRARIES}
        OpenSSL::SSL
)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)