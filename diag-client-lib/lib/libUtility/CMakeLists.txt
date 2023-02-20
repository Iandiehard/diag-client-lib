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

option(USE_DLT "Option to use Dlt for logging" OFF)

if (USE_DLT)
    include(FetchContent)
    FetchContent_Declare(
            automotive-dlt
            URL https://github.com/COVESA/dlt-daemon/archive/refs/tags/v2.18.8.tar.gz
    )

    FetchContent_MakeAvailable(automotive-dlt)
endif (USE_DLT)

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/utility/*.cpp")

add_library(${PROJECT_NAME}
        ${SRCS}
        )

add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        )

#link target directory
if (USE_DLT)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Genivi::dlt
            )
else ()
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            )
endif (USE_DLT)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
        )
