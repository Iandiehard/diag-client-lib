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

if (${ENABLE_WIN_SYSTEM} MATCHES "ON")
        include_directories(${BOOST_PATH}/include)
        link_directories(${BOOST_PATH}/lib)
else()
        find_package(Boost 1.78.0)
endif()


file(GLOB LIBBOOST_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp")
file(GLOB LIBBOOST_SOCKET_TCP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/socket/tcp/*.cpp")
file(GLOB LIBBOOST_SOCKET_UDP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/socket/udp/*.cpp")
file(GLOB LIBBOOST_JSON_PARSER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp")

set(LIBBOOST_SOCKET_SRCS
        ${LIBBOOST_SOCKET_TCP_SRCS}
        ${LIBBOOST_SOCKET_UDP_SRCS}
        )

add_library(${PROJECT_NAME}
        ${LIBBOOST_COMMON_SRCS}
        ${LIBBOOST_SOCKET_SRCS}
        ${LIBBOOST_JSON_PARSER_SRCS}
        )

if (${ENABLE_WIN_SYSTEM} MATCHES "ON")
        #Link include directory
        target_include_directories(${PROJECT_NAME} PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>"
                )

        #link target directory
        target_link_libraries(${PROJECT_NAME}
                PRIVATE
                common
                utility-support
                wsock32
                Ws2_32
                libboost_system-mgw7-mt-s-x64-1_78.a
                )
else()
        #Link include directory
        target_include_directories(${PROJECT_NAME} PUBLIC
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>"
                )

        #link target directory
        target_link_libraries(${PROJECT_NAME}
                PRIVATE
                common
                utility-support
                ${Boost_LIBRARIES}
                )
endif()


install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
        )