ARG VARIANT=ubuntu-20.04
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# Install packages
RUN apt update && \
    apt-get -y install python3-dev && \
    apt-get -y install build-essential qt5-default libqt5serialport5-dev && \
    apt-get -y install can-utils && \
    apt-get -y install libsocketcan-dev &&\
    apt-get -y install cmake zlib1g-dev libdbus-glib-1-dev

# Install bazel
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    curl https://bazel.build/bazel-release.pub.gpg | apt-key add - 

RUN apt-get update && apt-get install bazel && \
    apt-get install --only-upgrade bazel

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

RUN apt-get update && \
      apt-get -y install sudo

RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo

USER docker
CMD /bin/bash
