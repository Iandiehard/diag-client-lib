cmake_minimum_required(VERSION 3.5)
project(ara-lib)

# adding common library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libCommon)
# adding uds transport layer 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libUdsTransportLayerApi)
# adding OsAbstraction library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libOsAbstraction)
# adding Doip Client library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libDoIP)