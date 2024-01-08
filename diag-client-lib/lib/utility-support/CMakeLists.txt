#  Diagnostic Client library CMake File
#  Copyright (C) 2024  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(utility-support)

#set default language
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if (BUILD_WITH_DLT)
    find_package(automotive-dlt REQUIRED)
endif (BUILD_WITH_DLT)

file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/utility/*.cpp")

add_library(
        ${PROJECT_NAME}
        ${SRCS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

#link target directory
if (BUILD_WITH_DLT)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Genivi::dlt
    )
else ()
    target_link_libraries(${PROJECT_NAME}
            PRIVATE)
endif (BUILD_WITH_DLT)

install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
)
