#!/bin/bash
BUILD_TYPE="Debug"
BUILD_DIR="gcc9"

if [ ! -d build/$BUILD_DIR ]; then
  echo "***************** Create build folder for clang 14 *************************"
  mkdir -p build/$BUILD_DIR
fi


echo "********************** Configure CMake *************************************"
cmake -B build/$BUILD_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILD_WITH_DLT:BOOL=ON -DBUILD_WITH_TEST:BOOL=ON -DBUILD_DOXYGEN:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_EXAMPLES:BOOL=ON -DCMAKE_INSTALL_PREFIX:PATH=build/install -DCMAKE_TOOLCHAIN_FILE=toolchains/linux_gcc_9.cmake

echo "********************** Start Building **************************************"
cmake --build build/$BUILD_DIR --config $BUILD_TYPE
