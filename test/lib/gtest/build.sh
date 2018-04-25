#!/bin/sh

echo "start to build gtest lib"

GTEST_PATH=googletest-release-1.7.0
tar zxvf release-1.7.0.tar.gz > /dev/null 2>&1
cd $GTEST_PATH
cmake -DBUILD_SHARED_LIBS=OFF
make
cmake -DBUILD_SHARED_LIBS=ON
make

mkdir -p ../lib
mkdir -p ../include
cp libgtest.so libgtest_main.so ../lib
cp libgtest.a libgtest_main.a ../lib
cp -a include/gtest ../include

cd ..
rm -rf $GTEST_PATH

