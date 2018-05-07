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

extern List gRlcUeContextList;

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Single_Rlc_SDU_Seg_Id_Resp) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    // Create test data
    unsigned short rnti = 101;
    unsigned short lcId = 1;
    unsigned char rlcSdu[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    MacUeDataInd_t* pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    RlcUlData* pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    RlcUlDataInfo* pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSdu);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    RlcUeContext* pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx == 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    ASSERT_EQ((int)gLteKpi.mem, 3);

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1); // add one for RLC context
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 PDCP buffer 
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    ASSERT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
    ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Single_Rlc_SDU_Seg_RRC_Setup_Compl) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    // Create test data
    unsigned short rnti = 102;
    unsigned short lcId = 1;
    unsigned char rlcSdu[] = {
        0xA0, 0x00, 0x00, 0x22, 0x00, 0x78, 0x0E, 0x82, 0xE2, 0x10, 
        0x92, 0x0C, 0x80, 0x22, 0x68, 0x0F, 0x24, 0xE8, 0x09, 0xC1, 
        0xC1, 0x81, 0x80, 0x00, 0x46, 0x04, 0x03, 0xA0, 0x62, 0x4E, 
        0x3B, 0x01, 0x00, 0x42, 0x20, 0x02, 0x02, 0x00, 0x21, 0x02, 
        0x0C, 0x00, 0x00, 0x00, 0x01, 0x06, 0x0C, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x14, 0x00, 
        0xB8, 0x40, 0x00, 0xBA, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00
    };
    MacUeDataInd_t* pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    RlcUlData* pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    RlcUlDataInfo* pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSdu);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    RlcUeContext* pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx == 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 3);

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 PDCP buffer 
    unsigned char expectPdcpPdu[] = {
        0x00, 0x22, 0x00, 0x78, 0x0E, 0x82, 0xE2, 0x10, 
        0x92, 0x0C, 0x80, 0x22, 0x68, 0x0F, 0x24, 0xE8, 0x09, 0xC1, 
        0xC1, 0x81, 0x80, 0x00, 0x46, 0x04, 0x03, 0xA0, 0x62, 0x4E, 
        0x3B, 0x01, 0x00, 0x42, 0x20, 0x02, 0x02, 0x00, 0x21, 0x02, 
        0x0C, 0x00, 0x00, 0x00, 0x01, 0x06, 0x0C, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x14, 0x00, 
        0xB8, 0x40, 0x00, 0xBA, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00};
    ASSERT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
    ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer      
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Two_Rlc_SDU_Seg_Id_Resp) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    // RLC Segment 1
    // Create test data
    unsigned short rnti = 103;
    unsigned short lcId = 1;
    unsigned char rlcSduSeg1[] = {
        0x88, 0x02, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8};
    MacUeDataInd_t* pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    RlcUlData* pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    RlcUlDataInfo* pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSduSeg1);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg1, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    RlcUeContext* pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx == 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 for MacUeDataInd_t, 1 for RlcUlDataInfo, 1 for rlcdataBuffer

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 first RLC segment data
    ASSERT_EQ(gRlcUeDataInd.numUe, 0);

    // RLC Segment 2
    unsigned char rlcSduSeg2[] = {
        0xB0, 0x03, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 
        0x00, 0x00};
    
    pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSduSeg2);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg2, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1); // 10 for mempool, 1 for RLC context list, 1 for RLC context
    ASSERT_EQ((int)gLteKpi.mem, 6); // 1 RLC ctx, 1 AM Entity, 1 first RLC segment data, 1 for MacUeDataInd_t, 1 for RlcUlDataInfo, 1 for rlcdataBuffer

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 pdcp data
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    ASSERT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
    ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
    ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Muti_Ue_Single_Rlc_Sdu_Seg) {
    gLogLevel = 3;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;
    unsigned short rnti = 200;
    unsigned short lcId = 1;
    unsigned int i;
    unsigned numOfUeTest = 2048;
    unsigned char rlcSdu[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    
    RlcPdcpUeDataInd_test* pRlcUeDataInd;
    unsigned int numMemCreated = 0;
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};

    for (i=0; i<numOfUeTest; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSdu);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx == 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+i); // 10 for mempool, 1 for RLC context list
        numMemCreated = 3 + i * 3; // 3 for test data, i*3 for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i+1);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+(i+1));
        numMemCreated = (i+1) * 3; // for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 1);
        pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
        ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
        ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
        ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
        ASSERT_TRUE(pRlcUeDataInd->pData != 0);
        ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
        delete pRlcUeDataInd->pData;
        gRlcUeDataInd.numUe = 0;
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

        rnti++;
    }
    
    ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
    ASSERT_EQ(ListCount(&gRlcUeContextList), numOfUeTest);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest*3);

    pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while(pRlcUeCtx != 0) {
        RlcDeleteUeContext(pRlcUeCtx);
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    }

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest); // PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Muti_Ue_Two_Rlc_Sdu_Seg) {
    gLogLevel = 3;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;    
    unsigned short firstRnti = 300;
    unsigned short rnti = firstRnti;
    unsigned short lcId = 1;
    unsigned int i;
    unsigned numOfUeTest = 2048;
    unsigned char rlcSduSeg1[] = {
        0x88, 0x02, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8};
    unsigned char rlcSduSeg2[] = {
        0xB0, 0x03, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 
        0x00, 0x00};

    RlcPdcpUeDataInd_test* pRlcUeDataInd;
    unsigned int numMemCreated = 0;
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};

    // 1st segment
    for (i=0; i<numOfUeTest; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSduSeg1);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg1, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx == 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+i); // 10 for mempool, 1 for RLC context list
        numMemCreated = 3 + i * 3; // 3 for test data, i*3 for rlc ctx + AM entity + 1st RLC SDU seg
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i+1);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+(i+1));
        numMemCreated = (i+1) * 3; // for rlc ctx + AM entity + 1st RLC SDU seg
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 0);

        rnti++;
    }

    rnti = firstRnti; 
    ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
    ASSERT_EQ(ListCount(&gRlcUeContextList), numOfUeTest);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest*3);

    // last segment
    for (i=0; i<numOfUeTest; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSduSeg2);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg2, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+numOfUeTest); // 10 for mempool, 1 for RLC context list
        numMemCreated = 3 + numOfUeTest * 3; // 3 for test data
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+numOfUeTest);
        numMemCreated = numOfUeTest * 3; // for rlc ctx + AM entity + 1st RLC SDU seg
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 1);
        pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
        ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
        ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
        ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
        ASSERT_TRUE(pRlcUeDataInd->pData != 0);
        ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
        delete pRlcUeDataInd->pData;
        gRlcUeDataInd.numUe = 0;
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

        rnti++;
    }

    ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
    ASSERT_EQ(ListCount(&gRlcUeContextList), numOfUeTest);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest*3);

    pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while(pRlcUeCtx != 0) {
        RlcDeleteUeContext(pRlcUeCtx);
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    }

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest); // PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_2_Single_Rlc_Sdu_Seg) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;
    unsigned short rnti = 400;
    unsigned short lcId = 2;
    unsigned int i;
    unsigned numOfUeTest = 1;
    unsigned char rlcSdu[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    
    RlcPdcpUeDataInd_test* pRlcUeDataInd;
    unsigned int numMemCreated = 0;
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};

    for (i=0; i<numOfUeTest; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSdu);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx == 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+i); // 10 for mempool, 1 for RLC context list
        numMemCreated = 3 + i * 3; // 3 for test data, i*3 for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i+1);
        ASSERT_EQ(gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+(i+1)); 
        numMemCreated = (i+1) * 3; // for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 1);
        pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
        ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
        ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
        ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
        ASSERT_TRUE(pRlcUeDataInd->pData != 0);
        ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
        delete pRlcUeDataInd->pData;
        gRlcUeDataInd.numUe = 0;
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

        rnti++;
    }
    
    ASSERT_EQ(gLteKpi.activeRlcCtx, numOfUeTest);
    ASSERT_EQ(ListCount(&gRlcUeContextList), numOfUeTest);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest*3);

    pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while(pRlcUeCtx != 0) {
        RlcDeleteUeContext(pRlcUeCtx);
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    }

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ(gLteKpi.mem, numOfUeTest); // PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_3_Single_Rlc_Sdu_Seg) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;
    unsigned short rnti = 401;
    unsigned short lcId = 3;
    unsigned int i;
    unsigned numOfUeTest = 1;
    unsigned char rlcSdu[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    
    unsigned int numMemCreated = 0;

    for (i=0; i<numOfUeTest; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSdu);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx == 0);
        ASSERT_EQ(gLteKpi.activeRlcCtx, i);
        ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
        numMemCreated = 3 + i * 3; // 3 for test data, i*3 for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx == 0);
        ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
        ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
        numMemCreated = 0; // for rlc ctx + AM entity + pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 0);

        rnti++;
    }
    
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
    ASSERT_EQ((int)ListCount(&gRlcUeContextList), 0);
    ASSERT_EQ((int)gLteKpi.mem, 0);
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Multi_Single_Rlc_Sdu_Seg) {
    gLogLevel = 3;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;
    unsigned short rnti = 400;
    unsigned short lcId = 2;
    unsigned int i;
    unsigned numOfRlcSeg = 1024;
    unsigned char rlcSdu[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    
    RlcPdcpUeDataInd_test* pRlcUeDataInd;
    unsigned int numMemCreated = 0;
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};

    for (i=0; i<numOfRlcSeg; i++) {
        // Create test data
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = sizeof(rlcSdu);
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSdu, pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        if (i == 0) {
            ASSERT_TRUE(pRlcUeCtx == 0);
            ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
            ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
            numMemCreated = 3;
        } else {            
            ASSERT_TRUE(pRlcUeCtx != 0);
            ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
            ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1); // 10 for mempool, 1 for RLC context list, 1 for RLC Context
            numMemCreated = 3 + 2 + i; // 3 for test data, 2 for rlc ctx and AM entity, i for pdcp buffer
        }
        ASSERT_EQ(gLteKpi.mem, numMemCreated);

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
        ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
        numMemCreated = 2 + (1 + i); // 2 for rlc ctx + AM entity, (1+i) for pdcp buffer
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 
        ASSERT_EQ(gRlcUeDataInd.numUe, 1);
        pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
        ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
        ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
        ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
        ASSERT_TRUE(pRlcUeDataInd->pData != 0);
        ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
        delete pRlcUeDataInd->pData;
        gRlcUeDataInd.numUe = 0;
        memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
    }
    
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)ListCount(&gRlcUeContextList), 1);
    ASSERT_EQ(gLteKpi.mem, (2 + numOfRlcSeg));

    pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while(pRlcUeCtx != 0) {
        RlcDeleteUeContext(pRlcUeCtx);
        pRlcUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    }

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ(gLteKpi.mem, numOfRlcSeg); // PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Rlc_Sdu_Seg_Sn_Not_Consecutive) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;    
    unsigned short firstRnti = 500;
    unsigned short rnti = firstRnti;
    unsigned short lcId = 1;
    unsigned char rlcSduSeg1[] = {
        0x88, 0x02, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8}; // sn = 02
    unsigned char rlcSduSeg2[] = {
        0xB0, 0x04, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 
        0x00, 0x00}; // sn = 04

    unsigned int numMemCreated = 0;
    
    // Create test data
    pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSduSeg1);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg1, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx == 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    numMemCreated = 3 ; // 3 for test data, i*3 for rlc ctx + AM entity + 1st RLC SDU seg
    ASSERT_EQ(gLteKpi.mem, numMemCreated);

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
    numMemCreated = 3; // for rlc ctx + AM entity + 1st RLC SDU seg
    ASSERT_EQ(gLteKpi.mem, numMemCreated); 
    ASSERT_EQ(gRlcUeDataInd.numUe, 0);

    // rlc segment 2
    pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    ASSERT_TRUE(pMacDataInd != 0);
    pMacDataInd->rnti = rnti;
    pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
    pRlcUlData = pMacDataInd->rlcData;
    ASSERT_TRUE(pRlcUlData != 0);
    pRlcUlData->numLCInfo = 1;
    pLcIdInfo = &pRlcUlData->rlcDataArray[0];
    pLcIdInfo->lcId = lcId;
    pLcIdInfo->length = sizeof(rlcSduSeg2);
    pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
    memcpy(pLcIdInfo->rlcdataBuffer, rlcSduSeg2, pLcIdInfo->length);

    // Pre-check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1); // 10 for mempool, 1 for RLC context list, 1 for RLC Context
    numMemCreated = 3 + 3; // 3 for test data
    ASSERT_EQ(gLteKpi.mem, numMemCreated);

    MacUeDataInd(pMacDataInd);

    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
    numMemCreated = 2; // for rlc ctx + AM entity 
    ASSERT_EQ(gLteKpi.mem, numMemCreated); 

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 0);   
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Multi_Rlc_SDU_Seg_Rrc_Setup_Compl) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

    unsigned char rlcSduSeg1[] = {
        0x88, 0x00, 0x00, 0x20
        };
    unsigned char rlcSduSeg2[] = {
        0x98, 0x01, 0x20, 0x80, 0x01, 0x00, 0x59, 0x17
        };
    unsigned char rlcSduSeg3[] = {
        0x98, 0x02, 0x39, 0x45, 0xE5, 0x34, 0x0B, 0x07
        };
    unsigned char rlcSduSeg4[] = {
        0x98, 0x03, 0x41, 0x02, 0x0B, 0xF6, 0x03, 0x02
        };
    unsigned char rlcSduSeg5[] = {
        0x98, 0x04, 0x27, 0x80, 0x01, 0x00, 0xD0, 0xCC
        };
    unsigned char rlcSduSeg6[] = {
        0x98, 0x05, 0x71, 0x51, 0x04, 0xE0
        };
    unsigned char rlcSduSeg7[] = {
        0x98, 0x06, 0xE0, 0xC0, 0x40, 0x00, 0x21, 0x02
        };
    unsigned char rlcSduSeg8[] = {
        0x98, 0x07, 0x03, 0xD0, 0x11, 0xD1, 0x27, 0x1A
        };
    unsigned char rlcSduSeg9[] = {
        0x98, 0x08, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00
        };
    unsigned char rlcSduSeg10[] = {
        0xB0, 0x09, 0x00, 0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 
        0x83, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00 
        };

    unsigned short rnti = 603;
    unsigned short lcId = 1;
    MacUeDataInd_t* pMacDataInd;
    RlcUlData* pRlcUlData;
    RlcUlDataInfo* pLcIdInfo;
    RlcUeContext* pRlcUeCtx;
    unsigned char* rlcSegData[10];
    unsigned int rlcSegSize[10];
    rlcSegData[0] = rlcSduSeg1;
    rlcSegSize[0] = sizeof(rlcSduSeg1);
    rlcSegData[1] = rlcSduSeg2;
    rlcSegSize[1] = sizeof(rlcSduSeg2);
    rlcSegData[2] = rlcSduSeg3;
    rlcSegSize[2] = sizeof(rlcSduSeg3);
    rlcSegData[3] = rlcSduSeg4;
    rlcSegSize[3] = sizeof(rlcSduSeg4);
    rlcSegData[4] = rlcSduSeg5;
    rlcSegSize[4] = sizeof(rlcSduSeg5);
    rlcSegData[5] = rlcSduSeg6;
    rlcSegSize[5] = sizeof(rlcSduSeg6);
    rlcSegData[6] = rlcSduSeg7;
    rlcSegSize[6] = sizeof(rlcSduSeg7);
    rlcSegData[7] = rlcSduSeg8;
    rlcSegSize[7] = sizeof(rlcSduSeg8);
    rlcSegData[8] = rlcSduSeg9;
    rlcSegSize[8] = sizeof(rlcSduSeg9);
    rlcSegData[9] = rlcSduSeg10;
    rlcSegSize[9] = sizeof(rlcSduSeg10);
    unsigned int numRlcSeg = 10;
    unsigned int i;   

    RlcPdcpUeDataInd_test* pRlcUeDataInd;
    unsigned int numMemCreated = 0;
    unsigned char expectPdcpPdu[] = {
        0x00, 0x20, 0x20, 0x80, 0x01, 0x00, 0x59, 0x17, 0x39, 0x45, 
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

    for (i=0; i<numRlcSeg; i++) {
        // RLC Segment 1
        pMacDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
        ASSERT_TRUE(pMacDataInd != 0);
        pMacDataInd->rnti = rnti;
        pMacDataInd->rlcData = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        pRlcUlData = pMacDataInd->rlcData;
        ASSERT_TRUE(pRlcUlData != 0);
        pRlcUlData->numLCInfo = 1;
        pLcIdInfo = &pRlcUlData->rlcDataArray[0];
        pLcIdInfo->lcId = lcId;
        pLcIdInfo->length = rlcSegSize[i];
        pLcIdInfo->rlcdataBuffer = MemAlloc(pLcIdInfo->length);
        memcpy(pLcIdInfo->rlcdataBuffer, rlcSegData[i], pLcIdInfo->length);

        // Pre-check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        if (i == 0) {
            ASSERT_TRUE(pRlcUeCtx == 0);
            ASSERT_EQ((int)gLteKpi.activeRlcCtx, 0);
            ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
            numMemCreated = 3; // 1 for MacUeDataInd_t, 1 for RlcUlDataInfo, 1 for rlcdataBuffer
        } else {
            ASSERT_TRUE(pRlcUeCtx != 0);
            ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);    
            ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1); // 10 for mempool, 1 for RLC context list, 1 for RLC Context
            numMemCreated = 3 + 2 + 1; // 1 for saved rlc incomplete SDU 
        }
        ASSERT_EQ(gLteKpi.mem, numMemCreated); 

        MacUeDataInd(pMacDataInd);

        // Check status
        KpiRefresh();
        pRlcUeCtx = RlcGetUeContext(rnti);
        ASSERT_TRUE(pRlcUeCtx != 0);
        ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
        ASSERT_EQ((int)gLteKpi.activeRlcCtx, 1);
        ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1+1);
        numMemCreated = 3;
        ASSERT_EQ(gLteKpi.mem, numMemCreated); // 1 RLC ctx, 1 AM Entity, 1 RLC segment data/pdcp data
        if (i < (numRlcSeg - 1)) {
            ASSERT_EQ(gRlcUeDataInd.numUe, 0);
        } else {
            ASSERT_EQ(gRlcUeDataInd.numUe, 1);
            pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
            ASSERT_EQ(pRlcUeDataInd->rnti, rnti);
            ASSERT_EQ(pRlcUeDataInd->lcId, lcId);
            ASSERT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
            ASSERT_TRUE(pRlcUeDataInd->pData != 0);
            ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
            delete pRlcUeDataInd->pData;
            gRlcUeDataInd.numUe = 0;
            memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));
        }
    }

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer     
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Rlc_Pdu_Segment) {


}