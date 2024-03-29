cmake_minimum_required(VERSION 3.5)
project(gtest-diag-client-lib)

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

file(GLOB DOIP_HANDLER "${CMAKE_CURRENT_SOURCE_DIR}/doip_handler/*.cpp")
file(GLOB TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/test_case/*.cpp")

add_executable(${PROJECT_NAME}
        main.cpp
        ${DOIP_HANDLER}
        ${TEST_SRCS}
        )

# include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

target_link_libraries(${PROJECT_NAME}
        diag-client
        platform-core
        boost-support
        utility-support
        GTest::gtest_main
        GTest::gmock_main
        )

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
