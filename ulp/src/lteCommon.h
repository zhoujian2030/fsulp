/*
 * lteCommon.h
 *
 *  Created on: Apr 17, 2018
 *      Author: j.zh
 */

#ifndef LTE_COMMON_H
#define LTE_COMMON_H

#ifdef RUN_ON_STANDALONE_CORE
#include "baseType.h"
#else
typedef unsigned char   UInt8;
typedef unsigned short  UInt16;
typedef unsigned int    UInt32;
typedef unsigned long long   UInt64;
typedef unsigned long    ULong32;
typedef unsigned char   UChar8;
typedef signed char   	SInt8;
typedef signed char   	Int8;
typedef char            Char8;
typedef signed short    SInt16;
typedef signed int      SInt32;
typedef signed short    Int16;
typedef signed int      Int32;
typedef unsigned char   BOOL;

#endif

#define TRUE  1
#define FALSE 0

#endif
