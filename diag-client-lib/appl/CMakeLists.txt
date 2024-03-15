#  Diagnostic Client library CMake File
#  Copyright (C) 2024  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(diag-client LANGUAGES CXX)

#set default language
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find needed packages
find_package(Threads REQUIRED)

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

# link all static libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        platform-core
        utility-support
        doip-client
        boost-support
        uds-transport-layer-api
        Threads::Threads
)

# install etc files
install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/etc/
        DESTINATION etc
)

# install diag.client
install(TARGETS ${PROJECT_NAME}
        EXPORT DiagClientLibTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# install include files
install(FILES ${HEADERS}
        DESTINATION ${PROJECT_NAME}/
)
