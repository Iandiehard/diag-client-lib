#!/bin/bash
apt-get update -qq

# Install compilers
apt-get install clang-14
apt-get install g++-11

# Install Boost library
BOOST_MAJOR_VERSION="1"
BOOST_MINOR_VERSION="79"

# Install DLT daemon (needed for logging)
DLT_MAJOR_VERSION="2"
DLT_MINOR_VERSION="18"
DLT_PATCH_VERSION="8"
wget "https://github.com/COVESA/dlt-daemon/archive/refs/tags/v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz"
mkdir dlt-daemon
tar -xvf v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz -C dlt-daemon
mkdir -p dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}/build
cd dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION} || exit
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --install build --config Release
