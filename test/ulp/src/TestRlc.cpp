/*
 * TestRlc.cpp
 *
 *  Created on: Apr 28, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestRlc.h"
#include "lteRlc.h"
#include "mempool.h"
#include "lteIntegrationPoint.h"
#include "lteRlcMacInterface.h"
#include "UlpTestCommon.h"
#include "lteKpi.h"

using namespace std;

extern unsigned int gLogLevel;

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1) {
    InitMemPool();
    InitRlcLayer();

    // Create test data
    MacUeDataInd_t* pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = 101;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    RlcUlData* pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    RlcUlDataInfo* pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = 1;

    // Pre-check status
    RlcUeContext* pRlcUeCtx = GetRlcUeContext(101);
    ASSERT_TRUE(pRlcUeCtx == 0);
    // unsigned numSemCreated = 
}

