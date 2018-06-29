#!/bin/sh

export PLATFORM=arm
export PATH=$PATH:/omap/eldk/usr/bin/
export PLATFORM_CXX=arm-linux-g++
export PLATFORM_CC=arm-linux-gcc
export PLATFORM_AR=arm-linux-ar
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include

export GTEST_SUPPORT=FALSE
export SCTP_SUPPORT=FALSE
export DB_SUPPORT=FALSE

export PROJBASE=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
echo "Current directory: $PROJBASE"

COMMAND=$1

if [[ $COMMAND = "clean" ]]; then
    make clean
elif [[ $COMMAND = "install" ]]; then
    make install
else
    make
    cp exe/ulp/obj/ulp /mnt/hgfs/c/share/ 
    cp exe/ulp/resource/ulp.conf /mnt/hgfs/c/share/ 
    cp exe/dpe/obj/dpe /mnt/hgfs/c/share/ 
    cp exe/dpe/resource/dpe.conf /mnt/hgfs/c/share/ 
    cp exe/ulpdeamon/obj/ulpdeamon /mnt/hgfs/c/share/ 
fi


