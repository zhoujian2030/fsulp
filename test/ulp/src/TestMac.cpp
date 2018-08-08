/*
 * TestMac.h
 *
 *  Created on: May 08, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestMac.h"
#include "lteMacPhyInterface.h"
#include "lteRlc.h"
#include "mempool.h"
#include "UlpTestCommon.h"
#include "lteUlpMgr.h"
#include "lteMac.h"
#include "lteKpi.h"
#include "lteLogger.h"
#include "lteUlpWorker.h"

using namespace std;

extern List gRecvdPhyDataList;
extern unsigned char gUlpWorkerInitilized;

TEST_F(TestMac, Interface_PhyUlDataInd_Async_One_BSR_Two_LcId1_One_Padding) {
    LteLoggerSetLogLevel(0);
    gCallMacDataInd = 0;
    KpiInit();
    InitMemPool();
    InitMacLayer();
    InitUlpWorker(1);

    // Identity Response, imsi = 460041143702947
    unsigned char macPdu[] = {
        0x3D, 0x21, 0x02, 0x21, 0x15, 0x1F, 0x00, 0x00, 0x04, 0xA0, 
        0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8,
        0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x03, 0xA0, 0x62, 0x4E, 0x3B, 0x01, 0x00, 0x42, 0x20,
        0x02, 0x02, 0x00, 0x21, 0x02, 0x0C, 0x00, 0x00, 0x00, 0x01, 
        0x06, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
        0x06, 0x00, 0x00, 0x14, 0x00, 0xB8, 0x40, 0x00, 0xBA, 0x02, 
        0x08, 0x00, 0x00, 0x00, 0x00, 0x3B, 0x8E, 0x8A, 0x41, 0x2B,
        0xF3, 0x53, 0x24, 0x5A, 0x1F
    };

    unsigned short length = 0;

#ifdef MAC_PHY_INTF_NEW
    RxUlSchInd* pRxUlSchInd = (RxUlSchInd*)gMsgBuffer;
    pRxUlSchInd->sfn = 501;
    pRxUlSchInd->sf = 7;
    pRxUlSchInd->numOfPDUs = 1;
    length += UL_SCH_IND_MSG_HEADER_LEN;

    UlSchPdu* pUlSchPdu = (UlSchPdu*)pRxUlSchInd->buffer;
    pUlSchPdu->rnti = 124;
    pUlSchPdu->length = sizeof(macPdu);    
    length += UL_SCH_PDU_HEADER_LENGTH;

    memcpy(pUlSchPdu->buffer, macPdu, pUlSchPdu->length);
    length += pUlSchPdu->length;
#else
    S_PhyHlMsgHead* pPhyMsgHead = (S_PhyHlMsgHead*)gMsgBuffer;
    pPhyMsgHead->opc = RX_ULSCH_INDICATION;
    length += sizeof(S_PhyHlMsgHead);

    S_UlIndHead* pUlIndHead = (S_UlIndHead*)(gMsgBuffer + length);
    length += sizeof(S_UlIndHead);
    pUlIndHead->sfn = 501;
    pUlIndHead->sf = 7;
    pUlIndHead->numOfPDUs = 1;

    S_RxUlschIndHeadPdu* pUlSchPduHead = (S_RxUlschIndHeadPdu*)(gMsgBuffer + length);
    length += sizeof(S_RxUlschIndHeadPdu);
    pUlSchPduHead->RNTI = 124;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPdu) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPdu) << 3;
    memcpy(gMsgBuffer + length, macPdu, sizeof(macPdu));    
    length += (pUlSchPduHead->wordLen << 2);
#endif

    // Pre-check status
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); // 10 for mempool + 1 for MAC context
    ASSERT_EQ((int)gLteKpi.mem, 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);

    PhyUlDataInd(gMsgBuffer, length);
    usleep(10);

    // check before notify
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); 
    ASSERT_EQ((int)gLteKpi.mem, 2);
    ASSERT_EQ(gMacUeDataInd.numUe, 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 1);

    NotifyUlpWorker();
    usleep(10000);

    // check after notify
    unsigned char expectRlcPdu1[] = {0x00, 0x04};
    unsigned char expectRlcPdu2[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};     
    KpiRefresh();   
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); 
    ASSERT_EQ((int)gLteKpi.mem, 4); // 1 for MacUeDataInd_t, 1 for pMacUeDataInd->rlcData, 2 for pMacUeDataInd->rlcData->rlcDataArray[0]&[1]
    ASSERT_EQ((int)gLteKpi.lcIdArray[1], 2);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);
    ASSERT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pMacUeDataInd->rnti, 124);
    ASSERT_TRUE(pMacUeDataInd->rlcData != 0);
    ASSERT_EQ(pMacUeDataInd->rlcData->numLCInfo, 2);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].lcId, 1);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].length, sizeof(expectRlcPdu1));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcPdu1, sizeof(expectRlcPdu1)) == 0);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].lcId, 1);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].length, sizeof(expectRlcPdu2));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer, expectRlcPdu2, sizeof(expectRlcPdu2)) == 0);
    delete pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer;
    delete pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer;
    delete pMacUeDataInd->rlcData;
    pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData = 0;
    gMacUeDataInd.numUe = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));

    // check RLC to PDCP indication
    ASSERT_EQ(gRlcUeDataInd.numUe, 0);

    gUlpWorkerInitilized = FALSE;
}

TEST_F(TestMac, Interface_PhyUlDataInd_No_LcId_One_To_Ten) {
    LteLoggerSetLogLevel(0);
    gCallMacDataInd = 0;
    KpiInit();
    InitMemPool();
    InitMacLayer();

    // Identity Response, imsi = 460041143702947
    unsigned char macPdu[] = {
        0x3e, 0x1f, 0x00, 0x00, 0x00, 0x10, 0x00, 0xa0, 0x00
    };

    unsigned short length = 0;

    S_PhyHlMsgHead* pPhyMsgHead = (S_PhyHlMsgHead*)gMsgBuffer;
    pPhyMsgHead->opc = RX_ULSCH_INDICATION;
    length += sizeof(S_PhyHlMsgHead);

    S_UlIndHead* pUlIndHead = (S_UlIndHead*)(gMsgBuffer + length);
    length += sizeof(S_UlIndHead);
    pUlIndHead->sfn = 501;
    pUlIndHead->sf = 2;
    pUlIndHead->numOfPDUs = 1;

    S_RxUlschIndHeadPdu* pUlSchPduHead = (S_RxUlschIndHeadPdu*)(gMsgBuffer + length);
    length += sizeof(S_RxUlschIndHeadPdu);
    pUlSchPduHead->RNTI = 124;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPdu) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPdu) << 3;
    memcpy(gMsgBuffer + length, macPdu, sizeof(macPdu));    
    length += (pUlSchPduHead->wordLen << 2);

    // Pre-check status
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); // 10 for mempool + 1 for MAC ue context list
    ASSERT_EQ((int)gLteKpi.mem, 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);

    PhyUlDataInd(gMsgBuffer, length);

    // check after
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); 
    ASSERT_EQ((int)gLteKpi.mem, 1); // 1 for MacUeDataInd_t
    ASSERT_EQ((int)gLteKpi.lcIdArray[1], 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);
    ASSERT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pMacUeDataInd->rnti, 124);
    ASSERT_TRUE(pMacUeDataInd->rlcData == 0);
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));
}

TEST_F(TestMac, Interface_PhyUlDataInd_Only_LcId_1_In_Mac_Pdu_IdResp) {
    LteLoggerSetLogLevel(0);
    gCallMacDataInd = 0;
    gCallRlcDataInd = 0;
    KpiInit();
    InitMemPool();
    InitMacLayer();

    // Identity Response, imsi = 460041143702947
    unsigned char macPdu[] = {
        0x01, 0xa0, 0x01, 0x01, 0x48, 0x02, 0x22, 0xfa, 0xb3, 0x98, 
        0xa8, 0x21, 0x40, 0xea, 0xc1, 0x09, 0x20, 0xc4, 0x05, 0x28,
        0x06, 0xea, 0x26, 0x20, 0x00, 0x00, 0x00, 0x00
    };

    unsigned short length = 0;

    S_PhyHlMsgHead* pPhyMsgHead = (S_PhyHlMsgHead*)gMsgBuffer;
    pPhyMsgHead->opc = RX_ULSCH_INDICATION;
    length += sizeof(S_PhyHlMsgHead);

    S_UlIndHead* pUlIndHead = (S_UlIndHead*)(gMsgBuffer + length);
    length += sizeof(S_UlIndHead);
    pUlIndHead->sfn = 501;
    pUlIndHead->sf = 2;
    pUlIndHead->numOfPDUs = 1;

    S_RxUlschIndHeadPdu* pUlSchPduHead = (S_RxUlschIndHeadPdu*)(gMsgBuffer + length);
    length += sizeof(S_RxUlschIndHeadPdu);
    pUlSchPduHead->RNTI = 124;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPdu) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPdu) << 3;
    memcpy(gMsgBuffer + length, macPdu, sizeof(macPdu));    
    length += (pUlSchPduHead->wordLen << 2);

    // Pre-check status
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); // 10 for mempool + 1 for MAC UE context list
    ASSERT_EQ((int)gLteKpi.mem, 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);

    PhyUlDataInd(gMsgBuffer, length);

    // check after
    unsigned char expectRlcPdu[] = {
        0xa0, 0x01, 0x01, 0x48, 0x02, 0x22, 0xfa, 0xb3, 0x98, 
        0xa8, 0x21, 0x40, 0xea, 0xc1, 0x09, 0x20, 0xc4, 0x05, 0x28,
        0x06, 0xea, 0x26, 0x20, 0x00, 0x00, 0x00, 0x00}; 
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); 
    ASSERT_EQ((int)gLteKpi.mem, 3); // 1 for MacUeDataInd_t, 1 for pMacUeDataInd->rlcData, 1 for pMacUeDataInd->rlcData->rlcDataArray[0]
    ASSERT_EQ((int)gLteKpi.lcIdArray[1], 1);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);
    ASSERT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(pMacUeDataInd->rnti, 124);
    ASSERT_TRUE(pMacUeDataInd->rlcData != 0);
    ASSERT_EQ(pMacUeDataInd->rlcData->numLCInfo, 1);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].lcId, 1);
    ASSERT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].length, sizeof(expectRlcPdu));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcPdu, sizeof(expectRlcPdu)) == 0);
    delete pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer;
    delete pMacUeDataInd->rlcData;
    pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData = 0;
    gMacUeDataInd.numUe = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));
}

TEST_F(TestMac, MAC_Process_CCCH_SDU) {
    LteLoggerSetLogLevel(1);
    gCallMacDataInd = 0;
    gCallMacCcchDataInd = 0;
    KpiInit();
    InitMemPool();
    InitMacLayer();

    unsigned char macPdu[] = {     
        0x3d, 0x20, 0x06, 0x1f, 0x00, 0x53, 0xbe, 0x68, 0x92, 0xb8, 
        0x36, 0xdb, 0x63, 0x1c, 0xd6, 0x2b, 0x8f, 0x30
    };

    unsigned short length = 0;
    unsigned short rnti = 124;

    S_PhyHlMsgHead* pPhyMsgHead = (S_PhyHlMsgHead*)gMsgBuffer;
    pPhyMsgHead->opc = RX_ULSCH_INDICATION;
    length += sizeof(S_PhyHlMsgHead);

    S_UlIndHead* pUlIndHead = (S_UlIndHead*)(gMsgBuffer + length);
    length += sizeof(S_UlIndHead);
    pUlIndHead->sfn = 501;
    pUlIndHead->sf = 7;
    pUlIndHead->numOfPDUs = 1;

    S_RxUlschIndHeadPdu* pUlSchPduHead = (S_RxUlschIndHeadPdu*)(gMsgBuffer + length);
    length += sizeof(S_RxUlschIndHeadPdu);
    pUlSchPduHead->RNTI = rnti;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPdu) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPdu) << 3;
    memcpy(gMsgBuffer + length, macPdu, sizeof(macPdu));    
    length += (pUlSchPduHead->wordLen << 2);

    // Pre-check status
    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); // 10 for mempool + 1 for MAC UE context list
    ASSERT_EQ((int)gLteKpi.mem, 0);
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);

    PhyUlDataInd(gMsgBuffer, length);

    unsigned char expectedRrcMsg[] = {
        0x53, 0xbe, 0x68, 0x92, 0xb8, 0x36
    };

    KpiRefresh();
    ASSERT_EQ((int)gLteKpi.semLock, MAX_NUM_POOL_SIZE + 1); 
    ASSERT_EQ((int)ListCount(&gRecvdPhyDataList), 0);
    ASSERT_EQ(2, (int)gLteKpi.mem); // 1 for MacUeDataInd_t, 1 for ccch data ind
    ASSERT_EQ(1, (int)gLteKpi.lcIdArray[0]); 

    ASSERT_EQ(1, gMacUeCcchDataInd.numUe);
    MacUeCcchInd* pCcchInd = (MacUeCcchInd*)&gMacUeCcchDataInd.ccchDataIndArray[0];
    ASSERT_EQ(rnti, pCcchInd->rnti);
    ASSERT_EQ(sizeof(expectedRrcMsg), pCcchInd->length);
    ASSERT_EQ(0, memcmp((void*)expectedRrcMsg, pCcchInd->pData, 0));
    memset((void*)&gMacUeCcchDataInd, 0, sizeof(MacCcchDataInd_Test_Array));

    ASSERT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    ASSERT_EQ(rnti, pMacUeDataInd->rnti);
    ASSERT_TRUE(pMacUeDataInd->rlcData == 0);
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));
}
