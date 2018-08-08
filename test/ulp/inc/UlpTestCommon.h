/*
 * UlpTestCommon.h
 *
 *  Created on: Apr 28, 2018
 *      Author: j.zh
 */

#ifndef ULP_TEST_COMMON_H
#define ULP_TEST_COMMON_H

// #ifdef __cplusplus
// extern "C" {
// #endif

#include "asn1.h"
#include "lteRlcMacInterface.h"
#include "lteRrc.h"
#include "lteCommon.h"

typedef struct {
    unsigned short rnti;
    unsigned short lcId;
    unsigned short size;
    UlReportInfoList ulRptInfoList;
    unsigned char* pData;
} RlcPdcpUeDataInd_test;

typedef struct {
    unsigned short rnti;
    unsigned char rrcMsgType;
    unsigned char nasMsgType;
    RrcUeContext ueContext;
} RrcUeDataInd_test;

typedef struct {
    int numUe;
    MacUeDataInd_t ueDataIndArray[32];
} MacUeDataInd_Test_Array;

typedef struct {
    unsigned short rnti;
    unsigned short length;
    unsigned char* pData;
} MacUeCcchInd;

typedef struct {
    int numUe;
    MacUeCcchInd ccchDataIndArray[32];
} MacCcchDataInd_Test_Array;

typedef struct {
    int numUe;
    RlcPdcpUeDataInd_test ueDataIndArray[32];
} RlcPdcpUeDataInd_Test_Array;

typedef struct {
    int numUe;
    RrcUeDataInd_test ueDataIndArray[32];
} RrcUeDataInd_Test_Array;

extern unsigned char gMsgBuffer[2048];

// extern MacUeDataInd_t gMacUeDataInd;
// extern RlcPdcpUeDataInd_test gRlcUeDataInd;
// extern RlcPdcpUeDataInd_test gPdcpUeDataInd;
// extern RrcUeDataInd_test gRrcUeDataInd;

extern MacUeDataInd_Test_Array gMacUeDataInd;
extern MacCcchDataInd_Test_Array gMacUeCcchDataInd;
extern RlcPdcpUeDataInd_Test_Array gRlcUeDataInd;
extern RlcPdcpUeDataInd_Test_Array gPdcpUeDataInd;
extern RrcUeDataInd_Test_Array gRrcUeDataInd;

extern unsigned int gCallMacDataInd;
extern unsigned int gCallMacCcchDataInd;
extern unsigned int gCallRlcDataInd;
extern unsigned int gCallPdcpDataInd;
extern unsigned int gCallRrcDataInd;

#endif

// #ifdef __cplusplus
// }
// #endif
