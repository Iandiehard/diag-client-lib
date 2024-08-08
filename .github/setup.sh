#!/bin/bash
apt-get update -qq

# Install compilers
apt-get install clang-14
apt-get install g++-11

# Install Boost library
BOOST_MAJOR_VERSION="1"
BOOST_MINOR_VERSION="79"

# Jfrog boost link is broken
# wget "https://boostorg.jfrog.io/artifactory/main/release/${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}.0/source/boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_0.tar.gz"
wget "https://sourceforge.net/projects/boost/files/boost/${BOOST_MAJOR_VERSION}.${BOOST_MINOR_VERSION}.0/boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_0.tar.gz"
mkdir boost
tar -zxvf boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_0.tar.gz -C boost
cd boost/boost_${BOOST_MAJOR_VERSION}_${BOOST_MINOR_VERSION}_0 || exit
./bootstrap.sh
./b2 variant=release install
cd ../../

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
