#  MANDAREIN Diagnostic Client library CMake File
#  Copyright (C) 2022  Avijit Dey
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

project(diag-client LANGUAGES CXX)

#set default language
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared library" ON)

# find needed packages
find_package(Threads REQUIRED)
find_package(automotive-dlt REQUIRED)

#find main sources
file(GLOB MAIN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB MAIN_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/common/*.cpp")
file(GLOB MAIN_DCM_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/*.cpp")
file(GLOB MAIN_DCM_CONNECTION_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/connection/*.cpp")
file(GLOB MAIN_DCM_CONVERSION_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/conversion/*.cpp")
file(GLOB MAIN_DCM_TIMER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/timer/*.cpp")
file(GLOB MAIN_DCM_CONFIGPARSER_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/config_parser/*.cpp")
file(GLOB MAIN_DCM_SERVICE "${CMAKE_CURRENT_SOURCE_DIR}/src/dcm/service/*.cpp")

set(LIB_MAIN_SRCS 
    ${MAIN_SRCS} 
    ${MAIN_COMMON_SRCS} 
    ${MAIN_DCM_SRCS} 
    ${MAIN_DCM_CONNECTION_SRCS} 
    ${MAIN_DCM_CONVERSION_SRCS}
    ${MAIN_DCM_TIMER_SRCS}
    ${MAIN_DCM_CONFIGPARSER_SRCS}
    ${MAIN_DCM_SERVICE}
)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED
    ${LIB_MAIN_SRCS}
    )
endif(BUILD_SHARED_LIBS)

# Include headers
set(include_header_path ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Src headers
set(src_header_path ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Library header include
set(lib_header_path ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${include_header_path}
    ${src_header_path} 
    ${lib_header_path} 
    "$<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>"
)

# link all static libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ara-lib::CommonLibrary
    ara-lib::DoIP
    ara-lib::BoostSupportLib
    ara-lib::UdsTransportLayerApi
    Threads::Threads
    Genivi::dlt
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
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
)
