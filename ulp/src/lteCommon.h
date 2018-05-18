/*
 * lteCommon.h
 *
 *  Created on: Apr 17, 2018
 *      Author: j.zh
 */

#ifndef LTE_COMMON_H
#define LTE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

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

#define TRUE  1
#define FALSE 0

#endif

typedef struct {
    UInt16  year;          /* 年 */
    UInt16  month;         /* 月 */
    UInt16  day;           /* 日 */
    UInt16  hour;          /* 时 */
    UInt16  minute;        /* 分 */
    UInt16  second;        /* 秒 */
    UInt16  millisecond;   /* 毫秒 */
} SystemTime;

typedef struct {
	UInt16 sfn;
	UInt8 sf;
} SystemSfnSf;

extern SystemTime gSystemTime;

extern void UpdateSystemTime();
extern void UpdateSfnSf(UInt16 sfn, UInt8 sf);
extern void GetSfnAndSf(UInt16* pSfn, UInt8* pSf);

#ifdef __cplusplus
}
#endif

#endif
