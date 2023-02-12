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

include(FetchContent)
FetchContent_Declare(
        automotive-dlt
        URL https://github.com/COVESA/dlt-daemon/archive/refs/tags/v2.18.8.tar.gz
)
# find needed packages
# find_package(automotive-dlt REQUIRED)
FetchContent_MakeAvailable(automotive-dlt)

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/utility/*.cpp")

add_library(${PROJECT_NAME}
        ${SRCS}
        )

add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        )

#link target directory
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Genivi::dlt
        )

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
        )
