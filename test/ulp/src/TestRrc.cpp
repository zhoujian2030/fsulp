/*
 * TestRrc.cpp
 *
 *  Created on: May 05, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestRrc.h"
#include "lteRrc.h"
#include "mempool.h"
#include "lteIntegrationPoint.h"
#include "lteRrcPdcpInterface.h"
#include "UlpTestCommon.h"
#include "lteKpi.h"
#include "asn1.h"

using namespace std;

extern unsigned int gLogLevel;

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_IdResp) {
    gLogLevel = 0;
    gCallRrcDataInd = 0;
    KpiInit();
    InitMemPool();

    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rrcMsg[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };

    RrcUeDataInd_test* pRrcUeDataInd;
    unsigned char expectImsiStr[] = "460041143702947";
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 0);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    ASSERT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    ASSERT_EQ(pRrcUeDataInd->identityResp.mobile_id.type_of_id, LIBLTE_MME_MOBILE_ID_TYPE_IMSI);
    ASSERT_TRUE(memcmp(pRrcUeDataInd->identityResp.mobile_id.imsi, expectImsiStr, 15) == 0);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}

// -------------------------------
TEST_F(TestRrc, Interface_PdcpUeSrbDataInd_LcId_1_RrcSetupCompl) {
    gLogLevel = 0;
    gCallRrcDataInd = 0;
    KpiInit();
    InitMemPool();

    unsigned short rnti = 102;
    unsigned short lcId = 1;
    unsigned char rrcMsg[] = {
        0x20, 0x20, 0x80, 0x01, 0x00, 0x59, 0x17, 0x39, 0x45, 
        0xE5, 0x34, 0x0B, 0x07, 0x41, 0x02, 0x0B, 0xF6, 0x03, 0x02,
        0x27, 0x80, 0x01, 0x00, 0xD0, 0xCC, 0x71, 0x51, 0x04, 0xE0,
        0xE0, 0xC0, 0x40, 0x00, 0x21, 0x02, 0x03, 0xD0, 0x11, 0xD1, 
        0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00, 0x10, 
        0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };

    RrcUeDataInd_test* pRrcUeDataInd;

    unsigned short dataSize = sizeof(rrcMsg);
    unsigned char* pPdcpDataInd = (unsigned char*)MemAlloc(dataSize);
    memcpy(pPdcpDataInd, rrcMsg, dataSize);

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.mem, 1);

    PdcpUeSrbDataInd(rnti, lcId, pPdcpDataInd, dataSize);

    KpiRefresh();
    ASSERT_EQ(gRrcUeDataInd.numUe, 1);
    ASSERT_EQ((int)gLteKpi.mem, 0);
    pRrcUeDataInd = &gRrcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRrcUeDataInd->rnti, rnti);
    ASSERT_EQ((int)pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
}