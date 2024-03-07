cmake_minimum_required(VERSION 3.5)
project(diag-client-lib-component-test)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MAIN CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE COMMON CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp")
file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_cases/*.cpp")

add_executable(${PROJECT_NAME}
        ${MAIN}
        ${COMMON}
        ${TEST_SRCS}
        )

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        )

target_link_libraries(${PROJECT_NAME}
        diag-client
        platform-core
        boost-support
        utility-support
        GTest::gtest_main
        GTest::gmock_main
        )

gtest_discover_tests(${PROJECT_NAME})

# Copy the default config file
set(DEFAULT_CONFIG "etc/diag_client_config.json")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Copying file '${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_CONFIG}' for '${PROJECT_BINARY_DIR}' to working directory..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${DEFAULT_CONFIG} ${PROJECT_BINARY_DIR}/)
