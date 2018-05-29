/*
 * TestUlp.cpp
 *
 *  Created on: Apr 25, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "TestUlp.h"
#include "lteMacPhyInterface.h"
#include "lteRlc.h"
#include "mempool.h"
#include "UlpTestCommon.h"
#include "lteUlpMgr.h"
#include "list.h"
#include "lteRrc.h"
#include "lteLogger.h"

using namespace std;

extern List gRrcUeContextList;

TEST_F(TestUlp, Rlc_Reassamble_Single_SDU) {
    LteLoggerSetLogLevel(1);
    gCallMacDataInd = 1;
    gCallRlcDataInd = 1;
    gCallPdcpDataInd = 1;
    gCallRrcDataInd = 0;
    InitUlpLayer(0, 0);

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

    PhyUlDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcPdu1[] = {0x00, 0x04};
    unsigned char expectRlcPdu2[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};        
    EXPECT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pMacUeDataInd->rnti, 124);
    ASSERT_TRUE(pMacUeDataInd->rlcData != 0);
    EXPECT_EQ(pMacUeDataInd->rlcData->numLCInfo, 2);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].length, sizeof(expectRlcPdu1));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcPdu1, sizeof(expectRlcPdu1)) == 0);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].lcId, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].length, sizeof(expectRlcPdu2));
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
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRlcUeDataInd->rnti, 124);
    EXPECT_EQ(pRlcUeDataInd->lcId, 1);
    EXPECT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    // check PDCP to RRC indication
    unsigned char expectRlcMsgData[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 0x26,
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gPdcpUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pPdcpUeDataInd = (RlcPdcpUeDataInd_test*)&gPdcpUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pPdcpUeDataInd->rnti, 124);
    EXPECT_EQ(pPdcpUeDataInd->lcId, 1);
    EXPECT_EQ(pPdcpUeDataInd->size, sizeof(expectRlcMsgData));
    ASSERT_TRUE(pPdcpUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pPdcpUeDataInd->pData, expectRlcMsgData, sizeof(expectRlcMsgData)) == 0);
    delete pPdcpUeDataInd->pData;
    memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    // check RRC decode result
    unsigned char expectImsiStr[] = "460041143702947";
    RrcUeContext* pRrcUeCtx;
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }
    EXPECT_EQ(gRrcUeDataInd.numUe, 1);
    RrcUeDataInd_test* pRrcUeDataInd = (RrcUeDataInd_test*)&gRrcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRrcUeDataInd->rnti, 124);
    EXPECT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    EXPECT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    // EXPECT_EQ(pRrcUeDataInd->identityResp.mobile_id.type_of_id, LIBLTE_MME_MOBILE_ID_TYPE_IMSI);
    // ASSERT_TRUE(memcmp(pRrcUeDataInd->identityResp.mobile_id.imsi, expectImsiStr, 15) == 0);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(124);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    ASSERT_EQ(RrcGetUeContextCount(), 0);
}

TEST_F(TestUlp, Rlc_Reassamble_2_SDU_Segment) {
    gCallMacDataInd = 1;
    gCallRlcDataInd = 1;
    gCallPdcpDataInd = 1;
    gCallRrcDataInd = 0;
    LteLoggerSetLogLevel(0);
    InitUlpLayer(0, 0);

    // Identity response, imsi = 460041143702947
    unsigned char macPduRlcSeg1[] = {
        0x3D, 0x21, 0x02, 0x01, 0x01, 0x00, 0x04, 0x88, 0x02, 0x01, 
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8
    };
    unsigned char macPduRlcSeg2[] = {
        0x3F, 0x3E, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x03, 0x02, 0x26, 
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };   

    unsigned short length = 0;

#ifdef MAC_PHY_INTF_NEW
    // send segment 1
    RxUlSchInd* pRxUlSchInd = (RxUlSchInd*)gMsgBuffer;
    pRxUlSchInd->sfn = 501;
    pRxUlSchInd->sf = 7;
    pRxUlSchInd->numOfPDUs = 1;
    length += UL_SCH_IND_MSG_HEADER_LEN;

    UlSchPdu* pUlSchPdu = (UlSchPdu*)pRxUlSchInd->buffer;
    pUlSchPdu->rnti = 124;
    pUlSchPdu->length = sizeof(macPduRlcSeg1);    
    length += UL_SCH_PDU_HEADER_LENGTH;

    memcpy(pUlSchPdu->buffer, macPduRlcSeg1, pUlSchPdu->length);
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
    pUlSchPduHead->RNTI = 125;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPduRlcSeg1) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPduRlcSeg1) << 3;
    memcpy(gMsgBuffer + length, macPduRlcSeg1, sizeof(macPduRlcSeg1));
    length += (pUlSchPduHead->wordLen << 2);
#endif

    PhyUlDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcStatusPdu[] = {0x00, 0x04};
    unsigned char expectRlcSeg1Pdu[] = {
        0x88, 0x02, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8};
    EXPECT_EQ(gMacUeDataInd.numUe, 1);
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pMacUeDataInd->rnti, 125);
    ASSERT_TRUE(pMacUeDataInd->rlcData != 0);
    EXPECT_EQ(pMacUeDataInd->rlcData->numLCInfo, 2);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].length, sizeof(expectRlcStatusPdu));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcStatusPdu, sizeof(expectRlcStatusPdu)) == 0);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].lcId, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[1].length, sizeof(expectRlcSeg1Pdu));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer, expectRlcSeg1Pdu, sizeof(expectRlcSeg1Pdu)) == 0);
    delete pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer;
    delete pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer;
    delete pMacUeDataInd->rlcData;
    pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData->rlcDataArray[1].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData = 0;
    gMacUeDataInd.numUe = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));

    // no RLC/PDCP/RRC indication
    EXPECT_EQ(gRlcUeDataInd.numUe, 0);
    EXPECT_EQ(gPdcpUeDataInd.numUe, 0);
    EXPECT_EQ(gRrcUeDataInd.numUe, 0);

    usleep(5000);

#ifdef MAC_PHY_INTF_NEW
    // send segment 2
    length = 0;
    pRxUlSchInd->sfn = 502;
    pRxUlSchInd->sf = 2;
    pRxUlSchInd->numOfPDUs = 1;
    length += UL_SCH_IND_MSG_HEADER_LEN;

    pUlSchPdu = (UlSchPdu*)pRxUlSchInd->buffer;
    pUlSchPdu->rnti = 124;
    pUlSchPdu->length = sizeof(macPduRlcSeg2);    
    length += UL_SCH_PDU_HEADER_LENGTH;

    memcpy(pUlSchPdu->buffer, macPduRlcSeg2, pUlSchPdu->length);
    length += pUlSchPdu->length;
#else 
    length = 0;
    pPhyMsgHead->opc = RX_ULSCH_INDICATION;
    length += sizeof(S_PhyHlMsgHead);

    length += sizeof(S_UlIndHead);
    pUlIndHead->sfn = 502;
    pUlIndHead->sf = 2;
    pUlIndHead->numOfPDUs = 1;

    length += sizeof(S_RxUlschIndHeadPdu);
    pUlSchPduHead->RNTI = 125;
    pUlSchPduHead->CRCFlag = 1;
    pUlSchPduHead->wordLen = (sizeof(macPduRlcSeg2) + 3) >> 2;
    pUlSchPduHead->bitLen = sizeof(macPduRlcSeg2) << 3;
    memcpy(gMsgBuffer + length, macPduRlcSeg2, sizeof(macPduRlcSeg2));
    length += (pUlSchPduHead->wordLen << 2);
#endif

    PhyUlDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcSeg2Pdu[] = {
        0xB0, 0x03, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 
        0x00, 0x00};
    EXPECT_EQ(gMacUeDataInd.numUe, 1);
    pMacUeDataInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pMacUeDataInd->rnti, 125);
    ASSERT_TRUE(pMacUeDataInd->rlcData != 0);
    EXPECT_EQ(pMacUeDataInd->rlcData->numLCInfo, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(pMacUeDataInd->rlcData->rlcDataArray[0].length, sizeof(expectRlcSeg2Pdu));
    ASSERT_TRUE(memcmp(pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcSeg2Pdu, sizeof(expectRlcSeg2Pdu)) == 0);
    delete pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer;
    delete pMacUeDataInd->rlcData;
    pMacUeDataInd->rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    pMacUeDataInd->rlcData = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_Test_Array));

    // check RLC to PDCP indication
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pRlcUeDataInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRlcUeDataInd->rnti, 125);
    EXPECT_EQ(pRlcUeDataInd->lcId, 1);
    EXPECT_EQ(pRlcUeDataInd->size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(pRlcUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pRlcUeDataInd->pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete pRlcUeDataInd->pData;
    gRlcUeDataInd.numUe = 0;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    // check PDCP to RRC indication
    unsigned char expectRlcMsgData[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 0x26,
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gPdcpUeDataInd.numUe, 1);
    RlcPdcpUeDataInd_test* pPdcpUeDataInd = (RlcPdcpUeDataInd_test*)&gPdcpUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pPdcpUeDataInd->rnti, 125);
    EXPECT_EQ(pPdcpUeDataInd->lcId, 1);
    EXPECT_EQ(pPdcpUeDataInd->size, sizeof(expectRlcMsgData));
    ASSERT_TRUE(pPdcpUeDataInd->pData != 0);
    ASSERT_TRUE(memcmp(pPdcpUeDataInd->pData, expectRlcMsgData, sizeof(expectRlcMsgData)) == 0);
    delete pPdcpUeDataInd->pData;
    gPdcpUeDataInd.numUe = 0;
    memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_Test_Array));

    // check RRC decode result
    unsigned char expectImsiStr[] = "460041143702947";
    RrcUeContext* pRrcUeCtx;
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }
    EXPECT_EQ(gRrcUeDataInd.numUe, 1);
    RrcUeDataInd_test* pRrcUeDataInd = (RrcUeDataInd_test*)&gRrcUeDataInd.ueDataIndArray[0];
    EXPECT_EQ(pRrcUeDataInd->rnti, 125);
    EXPECT_EQ(pRrcUeDataInd->rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    EXPECT_EQ(pRrcUeDataInd->nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    // EXPECT_EQ(pRrcUeDataInd->identityResp.mobile_id.type_of_id, LIBLTE_MME_MOBILE_ID_TYPE_IMSI);
    // ASSERT_TRUE(memcmp(pRrcUeDataInd->identityResp.mobile_id.imsi, expectImsiStr, 15) == 0);
    gRrcUeDataInd.numUe = 0;
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_Test_Array));
    ASSERT_EQ(RrcGetUeContextCount(), 1);
    pRrcUeCtx = RrcGetUeContext(125);
    ASSERT_TRUE(pRrcUeCtx != 0);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.imsiPresent, 1);
    ASSERT_EQ(pRrcUeCtx->ueIdentity.mTmsiPresent, 0);
    ASSERT_TRUE(memcmp(pRrcUeCtx->ueIdentity.imsi, expectImsiStr, 15) == 0);
    RrcDeleteUeContext(pRrcUeCtx);
    ASSERT_EQ(RrcGetUeContextCount(), 0);
}