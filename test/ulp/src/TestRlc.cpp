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
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 0);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    EXPECT_EQ((int)gLteKpi.mem, 3);

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 1);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    EXPECT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 PDCP buffer 
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRlcUeDataInd->rnti, rnti);
    EXPECT_EQ(pRlcUeDataInd->lcId, lcId);
    EXPECT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    EXPECT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer     
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
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 0);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    EXPECT_EQ((int)gLteKpi.mem, 3);

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 1);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    EXPECT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 PDCP buffer 
    unsigned char expectPdcpPdu[] = {
        0x00, 0x22, 0x00, 0x78, 0x0E, 0x82, 0xE2, 0x10, 
        0x92, 0x0C, 0x80, 0x22, 0x68, 0x0F, 0x24, 0xE8, 0x09, 0xC1, 
        0xC1, 0x81, 0x80, 0x00, 0x46, 0x04, 0x03, 0xA0, 0x62, 0x4E, 
        0x3B, 0x01, 0x00, 0x42, 0x20, 0x02, 0x02, 0x00, 0x21, 0x02, 
        0x0C, 0x00, 0x00, 0x00, 0x01, 0x06, 0x0C, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x14, 0x00, 
        0xB8, 0x40, 0x00, 0xBA, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRlcUeDataInd->rnti, rnti);
    EXPECT_EQ(pRlcUeDataInd->lcId, lcId);
    EXPECT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    EXPECT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer      
}

// -------------------------------
TEST_F(TestRlc, Interface_MacUeDataInd_LcId_1_Two_Rlc_SDU_Seg_Id_Resp) {
    gLogLevel = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitRlcLayer();

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
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 0);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    EXPECT_EQ((int)gLteKpi.mem, 3); // 1 for MacUeDataInd_t, 1 for RlcUlDataInfo, 1 for rlcdataBuffer

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 1);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    EXPECT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 first RLC segment data
    EXPECT_EQ(gRlcUeDataInd.numUe, 0);

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
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 1);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1); // 10 for mempool, 1 for RLC context list
    EXPECT_EQ((int)gLteKpi.mem, 6); // 1 RLC ctx, 1 AM Entity, 1 first RLC segment data, 1 for MacUeDataInd_t, 1 for RlcUlDataInfo, 1 for rlcdataBuffer

    MacUeDataInd(pMacDataInd);

    // Check status
    KpiRefresh();
    pRlcUeCtx = RlcGetUeContext(rnti);
    ASSERT_TRUE(pRlcUeCtx != 0);
    ASSERT_TRUE(pRlcUeCtx->rxAMEntityArray[lcId] != 0);
    EXPECT_EQ((int)gLteKpi.activeRlcCtx, 1);
    EXPECT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE+1);
    EXPECT_EQ((int)gLteKpi.mem, 3); // 1 RLC ctx, 1 AM Entity, 1 pdcp data
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRlcUeDataInd->rnti, rnti);
    EXPECT_EQ(pRlcUeDataInd->lcId, lcId);
    EXPECT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    RlcDeleteUeContext(pRlcUeCtx);
    KpiRefresh();
    EXPECT_EQ((int)gLteKpi.mem, 1); // 1 PDCP buffer     
}