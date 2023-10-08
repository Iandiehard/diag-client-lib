#!/bin/bash
OPENSSL_CMAKE_MAJOR_VERSION="3"
OPENSSL_CMAKE_MINOR_VERSION="1"
OPENSSL_CMAKE_PATCH_VERSION="3"
wget "https://github.com/jimmy-park/openssl-cmake/archive/refs/tags/3.1.3.tar.gz"
mkdir openssl-cmake
tar -xvf ${OPENSSL_CMAKE_MAJOR_VERSION}.${OPENSSL_CMAKE_MINOR_VERSION}.${OPENSSL_CMAKE_PATCH_VERSION}.tar.gz -C openssl-cmake
mkdir -p openssl-cmake/openssl-cmake-${OPENSSL_CMAKE_MAJOR_VERSION}.${OPENSSL_CMAKE_MINOR_VERSION}.${OPENSSL_CMAKE_PATCH_VERSION}/build
cd openssl-cmake/openssl-cmake-${OPENSSL_CMAKE_MAJOR_VERSION}.${OPENSSL_CMAKE_MINOR_VERSION}.${OPENSSL_CMAKE_PATCH_VERSION} || exit
cmake -B build -DOPENSSL_CONFIGURE_OPTIONS=no-shared no-tests -DOPENSSL_TARGET_VERSION=1.1.1w -DOPENSSL_CONFIGURE_VERBOSE=ON -DOPENSSL_INSTALL=ON
cmake --build build
cmake --install build
