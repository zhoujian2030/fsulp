#!/bin/sh

echo "build in platform " $PLATFORM

if [ $# == 1 ]; then
    libname="$1"
    echo "lib name is " $libname
else
    libname=libdb
fi

if [ "$PLATFORM" == "arm" ]; then
    cp lib/${libname}.a lib/${libname}_arm.a
elif [ "$PLATFORM" == "ppc" ]; then
    cp lib/${libname}.a lib/${libname}_ppc.a
fi

