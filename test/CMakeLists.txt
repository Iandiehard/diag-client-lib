cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

# Download and compile gtest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

# Build the component test
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/component)
