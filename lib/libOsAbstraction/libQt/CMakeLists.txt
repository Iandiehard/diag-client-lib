project(QTSupportLib)

include(GNUInstallDirs)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#set default language
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

#find common files
file(GLOB LIBQT_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libCommon/*.cpp")
#find Socket files
file(GLOB LIBQT_SOCKET_CAN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/can/*.cpp")
file(GLOB LIBQT_SOCKET_TCP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/tcp/*.cpp")
file(GLOB LIBQT_SOCKET_UDP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/udp/*.cpp")
set(LIBQT_SOCKET_SRCS ${LIBQT_SOCKET_CAN_SRCS} ${LIBQT_SOCKET_TCP_SRCS} ${LIBQT_SOCKET_UDP_SRCS})
#find Timer files
file(GLOB LIBQT_TIMER_ONESHOT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libTimer/oneShot/*.cpp")
set(LIBQT_TIMER_SRCS ${LIBQT_TIMER_ONESHOT_SRCS})

add_library(${PROJECT_NAME} STATIC
     ${LIBQT_COMMON_SRCS}
     ${LIBQT_SOCKET_SRCS}
     ${LIBQT_TIMER_SRCS}
)
add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#include common header
set(QT_common_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libCommon")
#include socket header
set(QT_socket_can_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/can")
set(QT_socket_tcp_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/tcp")
set(QT_socket_udp_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/udp")
#include timer header
set(QT_socket_timer_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libTimer/oneShot")

set(QTSupportLib_Header
    ${QT_common_header_path}
    ${QT_socket_can_header_path}
    ${QT_socket_tcp_header_path}
    ${QT_socket_udp_header_path}
    ${QT_socket_timer_header_path}
)

#Link include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${QTSupportLib_Header})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network)
