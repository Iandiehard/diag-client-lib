#!/bin/bash
BUILD_TYPE="Debug"
COMPILER_NAME="gcc11"
BUILD_DIR=$COMPILER_NAME-$BUILD_TYPE

if [ ! -d build/$BUILD_DIR ]; then
  echo "***************** Create build folder for gcc11 *************************"
  mkdir -p build/$BUILD_DIR
fi

echo "********************** Configure CMake *************************************"
cmake -B build/$BUILD_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILD_WITH_DLT:BOOL=ON -DBUILD_WITH_TEST:BOOL=ON -DBUILD_DOXYGEN:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_EXAMPLES:BOOL=ON -DCMAKE_INSTALL_PREFIX:PATH=build/install -DCMAKE_TOOLCHAIN_FILE=toolchains/linux_gcc_11.cmake

echo "********************** Start Building **************************************"
cmake --build build/$BUILD_DIR --config $BUILD_TYPE
