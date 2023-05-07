#  Diagnostic Client library CMake File
#  Copyright (C) 2023  Avijit Dey
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 3.5)
project(diag-client-lib)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build all the libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Build the diag client application
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/appl)

#Export library
install(EXPORT DiagClientLibTargets
        FILE DiagClientLibTargets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DiagClientLib
        )

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/DiagClientLibConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DiagClientLib
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/DiagClientLibConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DiagClientLib
        )

# install example directory
if (INSTALL_EXAMPLES)
    install(DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/examples
            DESTINATION examples
            )
endif (INSTALL_EXAMPLES)
