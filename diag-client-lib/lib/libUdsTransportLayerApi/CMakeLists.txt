project(uds-transport-layer-api)

include(GNUInstallDirs)

# Set default language settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} INTERFACE)
add_library(ara-lib::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#set includes paths
set(header_path "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR})
set(public_header ${HEADERS})
set(header ${public_header})

#Link include directories
target_include_directories(${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )

install(TARGETS ${PROJECT_NAME}
  EXPORT DiagClientLibTargets
  )