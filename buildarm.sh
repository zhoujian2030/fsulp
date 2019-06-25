#!/bin/sh

export PLATFORM=arm
export CROSS_COMPILE=arm-linux-
export PATH=$PATH:/omap/eldk/usr/bin/
export PLATFORM_CXX=arm-linux-g++
export PLATFORM_CC=arm-linux-gcc
export PLATFORM_AR=arm-linux-ar
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include

export GTEST_SUPPORT=FALSE
export SCTP_SUPPORT=FALSE
export DB_SUPPORT=FALSE
export BUILD_TARGET=all

export PROJBASE=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
echo "Current directory: $PROJBASE"

COMMAND=$1

if [[ $COMMAND = "--nodpe" ]]; then
    export BUILD_TARGET=nodpe
fi

if [[ $COMMAND = "clean" ]]; then
    make clean
elif [[ $COMMAND = "install" ]]; then
    make install
else
    make
    cp exe/ulp/ulp /mnt/hgfs/c/share/ 
    cp exe/ulp/resource/ulp.conf /mnt/hgfs/c/share/ 
    if [[ $COMMAND != "--nodpe" ]]; then
        cp exe/dpe/dpe /mnt/hgfs/c/share/ 
        cp exe/dpe/resource/dpe.conf /mnt/hgfs/c/share/ 
        cp exe/dpe/resource/initdpe.sh /mnt/hgfs/c/share/ 
    fi
    cp exe/deamon/deamon /mnt/hgfs/c/share/
fi
 