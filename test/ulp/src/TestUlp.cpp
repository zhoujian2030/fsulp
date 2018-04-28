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
#include "lteIntegrationPoint.h"
#include "lteRlcMacInterface.h"
#include "asn1.h"

using namespace std;

extern unsigned int gLogLevel;

unsigned char gMsgBuffer[2048];

typedef struct {
    unsigned short rnti;
    unsigned short lcId;
    unsigned short size;
    unsigned char* pData;
} RlcPdcpUeDataInd_test;

typedef struct {
    unsigned short rnti;
    unsigned char rrcMsgType;
    unsigned char nasMsgType;
    LIBLTE_MME_ID_RESPONSE_MSG_STRUCT identityResp;
} RrcUeDataInd_test;

MacUeDataInd_t gMacUeDataInd = {0};
RlcPdcpUeDataInd_test gRlcUeDataInd = {0};
RlcPdcpUeDataInd_test gPdcpUeDataInd = {0};
RrcUeDataInd_test gRrcUeDataInd = {0};

// --------------------------------------------------
extern "C" {
// ----------------------
void IP_Call_Mac_Data_Ind(void* pData)
{
    printf("IP_Call_Mac_Data_Ind, pMacDataInd = %p\n", pData);

    MacUeDataInd_t *pMacDataInd = (MacUeDataInd_t*)pData;

    ASSERT_TRUE(pMacDataInd != 0);
    ASSERT_TRUE(pMacDataInd->rlcData != 0);
    gMacUeDataInd.rnti = pMacDataInd->rnti;
    gMacUeDataInd.rlcData = new RlcUlData();
    gMacUeDataInd.rlcData->numLCInfo = pMacDataInd->rlcData->numLCInfo;
    unsigned int i = 0;
    for (i=0; i<pMacDataInd->rlcData->numLCInfo; i++) {
        gMacUeDataInd.rlcData->rlcDataArray[i].lcId = pMacDataInd->rlcData->rlcDataArray[i].lcId;
        gMacUeDataInd.rlcData->rlcDataArray[i].length = pMacDataInd->rlcData->rlcDataArray[i].length;
        ASSERT_TRUE(pMacDataInd->rlcData->rlcDataArray[i].rlcdataBuffer != 0);
        gMacUeDataInd.rlcData->rlcDataArray[i].rlcdataBuffer = new unsigned char[pMacDataInd->rlcData->rlcDataArray[i].length];
        memcpy(gMacUeDataInd.rlcData->rlcDataArray[i].rlcdataBuffer, pMacDataInd->rlcData->rlcDataArray[i].rlcdataBuffer,
            pMacDataInd->rlcData->rlcDataArray[i].length);
    }
}

// -----------------------
void IP_Call_Rlc_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    printf("IP_Call_Rlc_Data_Ind, rnti = %d, lcId = %d, pData = %p, size = %d\n", rnti, lcId, pData, size);

    ASSERT_TRUE(pData != 0);
    gRlcUeDataInd.rnti = rnti;
    gRlcUeDataInd.lcId = lcId;
    gRlcUeDataInd.size = size;
    gRlcUeDataInd.pData = new unsigned char[size];
    memcpy(gRlcUeDataInd.pData, pData, size);
}

// -----------------------
void IP_Call_Pdcp_Srb_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    printf("IP_Call_Pdcp_Srb_Data_Ind, rnti = %d, lcId = %d, pData = %p, size = %d\n", rnti, lcId, pData, size);
    ASSERT_TRUE(pData != 0);
    gPdcpUeDataInd.rnti = rnti;
    gPdcpUeDataInd.lcId = lcId;
    gPdcpUeDataInd.size = size;
    gPdcpUeDataInd.pData = new unsigned char[size];
    memcpy(gPdcpUeDataInd.pData, pData, size);
}

// -----------------------
void IP_Rrc_Decode_Result(unsigned short rnti, unsigned char rrcMsgType, unsigned char nasMsgType, void* pData)
{
    printf("IP_Rrc_Decode_Result, rnti = %d, rrcMsgType = 0x%02x, nasMsgType = 0x%02x, pData = %p\n", rnti, rrcMsgType, nasMsgType, pData);
    gRrcUeDataInd.rnti = rnti;
    gRrcUeDataInd.rrcMsgType = rrcMsgType;
    gRrcUeDataInd.nasMsgType = nasMsgType;
    if (pData != 0) {
        memcpy((void*)&gRrcUeDataInd.identityResp, pData, sizeof(LIBLTE_MME_ID_RESPONSE_MSG_STRUCT));
    }
}

} // end of extern C

TEST_F(TestUlp, Rlc_Reassamble_Single_SDU) {
    gLogLevel = 2;
    InitMemPool();
    InitRlcLayer();

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

    MacUlSchDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcPdu1[] = {0x00, 0x04};
    unsigned char expectRlcPdu2[] = {
        0xA0, 0x01, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 
        0x00};
    EXPECT_EQ(gMacUeDataInd.rnti, 124);
    ASSERT_TRUE(gMacUeDataInd.rlcData != 0);
    EXPECT_EQ(gMacUeDataInd.rlcData->numLCInfo, 2);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].length, sizeof(expectRlcPdu1));
    ASSERT_TRUE(memcmp(gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcPdu1, sizeof(expectRlcPdu1)) == 0);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[1].lcId, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[1].length, sizeof(expectRlcPdu2));
    ASSERT_TRUE(memcmp(gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer, expectRlcPdu2, sizeof(expectRlcPdu2)) == 0);
    delete gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer;
    delete gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer;
    delete gMacUeDataInd.rlcData;
    gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer = 0;
    gMacUeDataInd.rlcData = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_t));

    // check RLC to PDCP indication
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.rnti, 124);
    EXPECT_EQ(gRlcUeDataInd.lcId, 1);
    EXPECT_EQ(gRlcUeDataInd.size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(gRlcUeDataInd.pData != 0);
    ASSERT_TRUE(memcmp(gRlcUeDataInd.pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete gRlcUeDataInd.pData;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_test));

    // check PDCP to RRC indication
    unsigned char expectRlcMsgData[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 0x26,
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gPdcpUeDataInd.rnti, 124);
    EXPECT_EQ(gPdcpUeDataInd.lcId, 1);
    EXPECT_EQ(gPdcpUeDataInd.size, sizeof(expectRlcMsgData));
    ASSERT_TRUE(gPdcpUeDataInd.pData != 0);
    ASSERT_TRUE(memcmp(gPdcpUeDataInd.pData, expectRlcMsgData, sizeof(expectRlcMsgData)) == 0);
    delete gPdcpUeDataInd.pData;
    memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_test));

    // check RRC decode result
    unsigned char expectImsiStr[] = "460041143702947";
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }
    EXPECT_EQ(gRrcUeDataInd.rnti, 124);
    EXPECT_EQ(gRrcUeDataInd.rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    EXPECT_EQ(gRrcUeDataInd.nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    EXPECT_EQ(gRrcUeDataInd.identityResp.mobile_id.type_of_id, LIBLTE_MME_MOBILE_ID_TYPE_IMSI);
    ASSERT_TRUE(memcmp(gRrcUeDataInd.identityResp.mobile_id.imsi, expectImsiStr, 15) == 0);
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_test));
}

TEST_F(TestUlp, Rlc_Reassamble_2_SDU_Segment) {
    gLogLevel = 1;
    InitMemPool();
    InitRlcLayer();

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

    MacUlSchDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcStatusPdu[] = {0x00, 0x04};
    unsigned char expectRlcSeg1Pdu[] = {
        0x88, 0x02, 0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 
        0xC8};
    EXPECT_EQ(gMacUeDataInd.rnti, 125);
    ASSERT_TRUE(gMacUeDataInd.rlcData != 0);
    EXPECT_EQ(gMacUeDataInd.rlcData->numLCInfo, 2);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].length, sizeof(expectRlcStatusPdu));
    ASSERT_TRUE(memcmp(gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcStatusPdu, sizeof(expectRlcStatusPdu)) == 0);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[1].lcId, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[1].length, sizeof(expectRlcSeg1Pdu));
    ASSERT_TRUE(memcmp(gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer, expectRlcSeg1Pdu, sizeof(expectRlcSeg1Pdu)) == 0);
    delete gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer;
    delete gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer;
    delete gMacUeDataInd.rlcData;
    gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    gMacUeDataInd.rlcData->rlcDataArray[1].rlcdataBuffer = 0;
    gMacUeDataInd.rlcData = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_t));

    // no RLC/PDCP/RRC indication
    EXPECT_EQ(gRlcUeDataInd.rnti, 0);
    EXPECT_EQ(gRlcUeDataInd.lcId, 0);
    EXPECT_EQ(gRlcUeDataInd.size, 0);
    ASSERT_TRUE(gRlcUeDataInd.pData == 0);

    EXPECT_EQ(gPdcpUeDataInd.rnti, 0);
    EXPECT_EQ(gPdcpUeDataInd.lcId, 0);
    EXPECT_EQ(gPdcpUeDataInd.size, 0);
    ASSERT_TRUE(gPdcpUeDataInd.pData == 0);

    EXPECT_EQ(gRrcUeDataInd.rnti, 0);
    EXPECT_EQ(gRrcUeDataInd.rrcMsgType, 0);
    EXPECT_EQ(gRrcUeDataInd.nasMsgType, 0);

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

    MacUlSchDataInd(gMsgBuffer, length);

    // check MAC to RLC indication
    unsigned char expectRlcSeg2Pdu[] = {
        0xB0, 0x03, 0x02, 0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 
        0x00, 0x00};
    EXPECT_EQ(gMacUeDataInd.rnti, 125);
    ASSERT_TRUE(gMacUeDataInd.rlcData != 0);
    EXPECT_EQ(gMacUeDataInd.rlcData->numLCInfo, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].lcId, 1);
    EXPECT_EQ(gMacUeDataInd.rlcData->rlcDataArray[0].length, sizeof(expectRlcSeg2Pdu));
    ASSERT_TRUE(memcmp(gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer, expectRlcSeg2Pdu, sizeof(expectRlcSeg2Pdu)) == 0);
    delete gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer;
    delete gMacUeDataInd.rlcData;
    gMacUeDataInd.rlcData->rlcDataArray[0].rlcdataBuffer = 0;
    gMacUeDataInd.rlcData = 0;
    memset((void*)&gMacUeDataInd, 0, sizeof(MacUeDataInd_t));

    // check RLC to PDCP indication
    unsigned char expectPdcpPdu[] = {
        0x01, 0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 
        0x26, 0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gRlcUeDataInd.rnti, 125);
    EXPECT_EQ(gRlcUeDataInd.lcId, 1);
    EXPECT_EQ(gRlcUeDataInd.size, sizeof(expectPdcpPdu));
    ASSERT_TRUE(gRlcUeDataInd.pData != 0);
    ASSERT_TRUE(memcmp(gRlcUeDataInd.pData, expectPdcpPdu, sizeof(expectPdcpPdu)) == 0);
    delete gRlcUeDataInd.pData;
    memset((void*)&gRlcUeDataInd, 0, sizeof(RlcPdcpUeDataInd_test));

    // check PDCP to RRC indication
    unsigned char expectRlcMsgData[] = {
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8, 0x02, 0x26,
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(gPdcpUeDataInd.rnti, 125);
    EXPECT_EQ(gPdcpUeDataInd.lcId, 1);
    EXPECT_EQ(gPdcpUeDataInd.size, sizeof(expectRlcMsgData));
    ASSERT_TRUE(gPdcpUeDataInd.pData != 0);
    ASSERT_TRUE(memcmp(gPdcpUeDataInd.pData, expectRlcMsgData, sizeof(expectRlcMsgData)) == 0);
    delete gPdcpUeDataInd.pData;
    memset((void*)&gPdcpUeDataInd, 0, sizeof(RlcPdcpUeDataInd_test));

    // check RRC decode result
    unsigned char expectImsiStr[] = "460041143702947";
    for (unsigned i=0; i<15; i++) {
        expectImsiStr[i] -= 0x30;
    }
    EXPECT_EQ(gRrcUeDataInd.rnti, 125);
    EXPECT_EQ(gRrcUeDataInd.rrcMsgType, RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER);
    EXPECT_EQ(gRrcUeDataInd.nasMsgType, NAS_MSG_TYPE_IDENTITY_RESPONSE);
    EXPECT_EQ(gRrcUeDataInd.identityResp.mobile_id.type_of_id, LIBLTE_MME_MOBILE_ID_TYPE_IMSI);
    ASSERT_TRUE(memcmp(gRrcUeDataInd.identityResp.mobile_id.imsi, expectImsiStr, 15) == 0);
    memset((void*)&gRrcUeDataInd, 0, sizeof(RrcUeDataInd_test));
}