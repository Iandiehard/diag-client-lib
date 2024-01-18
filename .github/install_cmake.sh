#!/bin/bash
CMAKE_MAJOR_VERSION="3"
CMAKE_MINOR_VERSION="25"
CMAKE_PATCH_VERSION="3"
wget "https://github.com/Kitware/CMake/archive/refs/tags/v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}.tar.gz"
tar -zxvf v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}.tar.gz
rm -rf v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}.tar.gz
cd CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}
./bootstrap
make
make install
cd ..
rm -rf CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}