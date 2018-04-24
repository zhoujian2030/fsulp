#!/bin/sh

export PLATFORM=x86
export PLATFORM_CXX=g++
export PLATFORM_CC=gcc
export PLATFORM_AR=ar

export GTEST_SUPPORT=TRUE
export SCTP_SUPPORT=TRUE
export DB_SUPPORT=FALSE
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/log4cplusx86/include:/usr/local/boostx86/include


export PROJBASE=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)
echo "Current directory: $PROJBASE"

COMMAND=$1

if [[ $COMMAND = "clean" ]]; then
    make clean
elif [[ $COMMAND = "install" ]]; then
    make install
else
    make
fi

