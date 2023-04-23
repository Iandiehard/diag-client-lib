#
# Entry Project CMake
#
cmake_minimum_required(VERSION 3.5)
project(diag-client)

# Cmake options
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(INSTALL_EXAMPLES "Install Example App" OFF)
option(BUILD_WITH_DLT "Option to use Dlt for logging" ON)
option(BUILD_DOXYGEN "Option to generate doxygen file" ON)
option(BUILD_WITH_TEST "Option to build test target" ON)

# add compiler preprocessor flag when dlt enabled
if (BUILD_WITH_DLT)
    add_compile_definitions(ENABLE_DLT_LOGGER)
    message("Dlt logging enabled in diag-client library")
endif (BUILD_WITH_DLT)

# Build diag-client library
add_subdirectory(diag-client-lib)

# Build diag-client test targets
if (BUILD_WITH_TEST)
    add_subdirectory(test)
endif (BUILD_WITH_TEST)

# Build diag-client doxygen documentation
if (BUILD_DOXYGEN)
    add_subdirectory(docs)
endif (BUILD_DOXYGEN)
