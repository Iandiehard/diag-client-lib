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

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif (WIN32)

gtest_discover_tests(${PROJECT_NAME})

# Copy etc directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/etc $<TARGET_FILE_DIR:${PROJECT_NAME}>/etc)

# Copy cert directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/cert $<TARGET_FILE_DIR:${PROJECT_NAME}>/cert)
