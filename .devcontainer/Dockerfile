ARG VARIANT=ubuntu-20.04
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# Install necessary dependencies
RUN apt-get update &&\
    apt-get install -y --no-install-recommends \
        build-essential \
        autoconf \
        automake \
        libtool \
        pkg-config \
        apt-transport-https \
        ca-certificates \
        software-properties-common \
        wget \
        git \
        curl \
        gnupg \
        zlib1g-dev \
        swig \
        vim \
        gdb \
        valgrind \
        locales \
        tcpdump \
        cmake \
        doxygen \
        locales-all &&\
    apt-get clean

# Install Boost libraries and compile it for x86_64 linux
ARG BOOST_MAJOR_VERSION="1"
ARG BOOST_MINOR_VERSION="79"

RUN wget "https://boostorg.jfrog.io/artifactory/main/release/"${BOOST_MAJOR_VERSION}"."${BOOST_MINOR_VERSION}".0/source/boost_"${BOOST_MAJOR_VERSION}"_"${BOOST_MINOR_VERSION}"_0.tar.gz" && \
    mkdir /opt/boost && \
    tar -zxvf boost_"${BOOST_MAJOR_VERSION}"_"${BOOST_MINOR_VERSION}"_0.tar.gz -C /opt/boost  && \
    rm -rf boost_"${BOOST_MAJOR_VERSION}"_"${BOOST_MINOR_VERSION}"_0.tar.gz && \
    cd /opt/boost/boost_"${BOOST_MAJOR_VERSION}"_"${BOOST_MINOR_VERSION}"_0 && \
    ./bootstrap.sh && ./b2 variant=release link=static --includedir=/opt/boost/include --libdir=/opt/boost/lib install && \
    cd  .. && rm -rf boost_"${BOOST_MAJOR_VERSION}"_"${BOOST_MINOR_VERSION}"_0

# Install DLT Deamon
ARG DLT_MAJOR_VERSION="2"
ARG DLT_MINOR_VERSION="18"
ARG DLT_PATCH_VERSION="8"

RUN wget "https://github.com/COVESA/dlt-daemon/archive/refs/tags/v2.18.8.tar.gz" && \
    mkdir /opt/dlt-daemon  && \
    tar -xvf v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz -C /opt/dlt-daemon  && \
    rm -rf v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz && \
    mkdir -p /opt/dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}/build && \
    cd /opt/dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION} && \
    cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release && \
    cmake --install build --config Release