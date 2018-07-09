/*
 * TestPdcp.cpp
 *
 *  Created on: May 04, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestPdcp.h"
#include "ltePdcp.h"
#include "mempool.h"
#include "lteIntegrationPoint.h"
#include "ltePdcpRlcInterface.h"
#include "UlpTestCommon.h"
#include "lteKpi.h"
#include "lteLogger.h"

using namespace std;

// -------------------------------
TEST_F(TestPdcp, Interface_RlcUeDataInd_SRB_1_Normal) {
    LteLoggerSetLogLevel(0);
    gCallPdcpDataInd = 0;
    KpiInit();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char pdcpData[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    unsigned char expectRrcData[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    RlcPdcpUeDataInd_test* pPdcpUeDataInd;

    unsigned short dataSize = sizeof(pdcpData);
    unsigned char* pRlcDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pRlcDataInd, pdcpData, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;
    ulRptInfoList.count = 1;
    ulRptInfoList.ulRptInfo[0].rbNum = 2;

    RlcUeDataInd(rnti, lcId, pRlcDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gPdcpUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 1);
    pPdcpUeDataInd = &gPdcpUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pPdcpUeDataInd->rnti, rnti);
    ASSERT_EQ(pPdcpUeDataInd->lcId, lcId);
    ASSERT_EQ(pPdcpUeDataInd->size, sizeof(expectRrcData));
    ASSERT_TRUE(pPdcpUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pPdcpUeDataInd->pData, expectRrcData, sizeof(expectRrcData)) == 0);
    delete pPdcpUeDataInd->pData;
    gPdcpUeDataInd.numUe = 0;
    memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
}

// -------------------------------
TEST_F(TestPdcp, Interface_RlcUeDataInd_SRB_2_Normal) {
    LteLoggerSetLogLevel(0);
    gCallPdcpDataInd = 0;
    KpiInit();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 2;
    unsigned char pdcpData[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    // unsigned char expectRrcData[] = {
    //     0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
    //     0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    // };
    // RlcPdcpUeDataInd_test* pPdcpUeDataInd;

    unsigned short dataSize = sizeof(pdcpData);
    unsigned char* pRlcDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pRlcDataInd, pdcpData, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;
    ulRptInfoList.count = 1;
    ulRptInfoList.ulRptInfo[0].rbNum = 2;

    RlcUeDataInd(rnti, lcId, pRlcDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gPdcpUeDataInd.numUe, 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestPdcp, Interface_RlcUeDataInd_DRB_Normal) {
    LteLoggerSetLogLevel(0);
    gCallPdcpDataInd = 0;
    KpiInit();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 3;
    unsigned char pdcpData[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };
    // unsigned char expectRrcData[] = {
    //     0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
    //     0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    // };
    // RlcPdcpUeDataInd_test* pPdcpUeDataInd;

    unsigned short dataSize = sizeof(pdcpData);
    unsigned char* pRlcDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pRlcDataInd, pdcpData, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    UlReportInfoList ulRptInfoList;
    ulRptInfoList.count = 1;
    ulRptInfoList.ulRptInfo[0].rbNum = 2;

    RlcUeDataInd(rnti, lcId, pRlcDataInd, dataSize, &ulRptInfoList);

    KpiRefresh();
    ASSERT_EQ(gPdcpUeDataInd.numUe, 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}
