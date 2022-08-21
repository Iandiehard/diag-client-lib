#
# Main CMake
#
cmake_minimum_required(VERSION 3.5)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Cmake options
option(BUILD_SHARED_LIBS "Build shared library" ON)

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
