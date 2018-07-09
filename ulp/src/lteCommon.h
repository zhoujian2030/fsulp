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

#define INTEGRATE_PHYxx

#ifdef TI_DSP
#include "baseType.h"
#elif defined INTEGRATE_PHY
#include <xdc/std.h>

typedef unsigned char   BOOL;
typedef signed int      SInt32;

#else

typedef unsigned char   UInt8;
typedef unsigned short  UInt16;
typedef unsigned int    UInt32;
typedef unsigned long   UInt64;
typedef unsigned long   ULong32;
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
    UInt16  year;          /* �� */
    UInt16  month;         /* �� */
    UInt16  day;           /* �� */
    UInt16  hour;          /* ʱ */
    UInt16  minute;        /* �� */
    UInt16  second;        /* �� */
    UInt16  millisecond;   /* ���� */
} SystemTime;

typedef struct {
	UInt16 sfn;
	UInt8 sf;
} SystemSfnSf;

extern SystemTime gSystemTime;

extern void UpdateSystemTime();
extern void UpdateSfnSf(UInt16 sfn, UInt8 sf);
extern void GetSfnAndSf(UInt16* pSfn, UInt8* pSf);

typedef struct  {
    unsigned char rbNum;
    short ta;
    int prbPower;
} UlReportInfo;

#define MAX_RLC_SEGMENTS_NUM    10
typedef struct {
    unsigned char count;
    UlReportInfo  ulRptInfo[MAX_RLC_SEGMENTS_NUM];
} UlReportInfoList;

#ifdef __cplusplus
}
#endif

#endif
