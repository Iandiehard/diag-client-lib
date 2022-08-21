project(LinuxSupportLib)

#set default language
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

#find common files
file(GLOB LIBLINUX_COMMON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libCommon/*.cpp")
#find Socket files
file(GLOB LIBLINUX_SOCKET_CAN_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/can/src/*.cpp")
set(LIBLINUX_SOCKET_SRCS ${LIBLINUX_SOCKET_CAN_SRCS})

add_library(${PROJECT_NAME} STATIC
     ${LIBLINUX_COMMON_SRCS}
     ${LIBLINUX_SOCKET_CAN_SRCS}
)
add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#include common header
set(LINUX_common_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libCommon")
#include socket header
set(LINUX_socket_can_header_path "${CMAKE_CURRENT_SOURCE_DIR}/libSocket/can/include")

set(LINUXSupportLib_Header
    ${LINUX_common_header_path}
    ${LINUX_socket_can_header_path}
)

#Link include directory
target_include_directories(${PROJECT_NAME} PUBLIC ${LINUXSupportLib_Header})
#link target directory
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  ara-lib::CommonLibrary
)