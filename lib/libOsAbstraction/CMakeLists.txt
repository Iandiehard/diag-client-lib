cmake_minimum_required(VERSION 3.5)
project(OsAbstraction)

option(ENABLE_QT_SUPPORT "Build QT support library" OFF)

if(ENABLE_QT_SUPPORT)
    # adding Qt support library
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libQt)
endif(ENABLE_QT_SUPPORT)

# adding Boost support library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libBoost)
file(GLOB_RECURSE  BOOST_COMMON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libBoost/libCommon/*h)
file(GLOB_RECURSE  BOOST_JSONPARSER_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libBoost/libJsonParser/*h)
# adding Linux support library
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libLinux)

# install OsAbstraction files
set(BOOST_HEADERS
    ${BOOST_JSONPARSER_HEADERS}
    ${BOOST_COMMON_HEADERS}
)