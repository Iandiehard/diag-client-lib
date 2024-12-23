ARG VARIANT=22.04
FROM ubuntu:${VARIANT}

# Install necessary dependencies
RUN apt-get update && \
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
        gcc \
        g++-11 \
        clang-14 \ 
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
        clang-format \
        python3-pip \
        doxygen \
        graphviz \
        net-tools \
        iproute2 \
        libssl-dev \
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

RUN wget "https://github.com/COVESA/dlt-daemon/archive/refs/tags/v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz" && \
    mkdir /opt/dlt-daemon  && \
    tar -xvf v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz -C /opt/dlt-daemon  && \
    rm -rf v${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}.tar.gz && \
    mkdir -p /opt/dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION}/build && \
    cd /opt/dlt-daemon/dlt-daemon-${DLT_MAJOR_VERSION}.${DLT_MINOR_VERSION}.${DLT_PATCH_VERSION} && \
    cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release && \
    cmake --install build --config Release

# Add Dlt environment variable to bashrc for google test
 RUN echo 'export DLT_INITIAL_LOG_LEVEL="::6"' >> ~/.bashrc && \
     echo 'export DLT_LOCAL_PRINT_MODE=FORCE_ON' >> ~/.bashrc

# Install openssl 1.1.1w
RUN wget https://www.openssl.org/source/openssl-1.1.1w.tar.gz -O - | tar -xz
WORKDIR /openssl-1.1.1w
RUN ./config --prefix=/usr/local/openssl --openssldir=/usr/local/openssl && make && make install
