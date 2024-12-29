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

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME}
        ${HEADERS}
        ${SOURCES}
)

#Link include directory
target_include_directories(${PROJECT_NAME}
        PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
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
