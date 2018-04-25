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

using namespace std;

unsigned char gLogLevel = 0;

unsigned char gMsgBuffer[2048];

TEST_F(TestUlp, Rlc_Reassamble_Single_SDU) {

    void InitRlcLayer();

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

    MacUlSchDataInd(gMsgBuffer, length);
}

TEST_F(TestUlp, Rlc_Reassamble_2_SDU_Segment) {
    void InitRlcLayer();

    // Identity response, imsi = 460041143702947
    unsigned char macPduRlcSeg1[] = {
        0x3D, 0x21, 0x02, 0x01, 0x01, 0x00, 0x04, 0x88, 0x02, 0x01, 
        0x48, 0x01, 0x60, 0xEA, 0xC1, 0x09, 0x20, 0xC8
    };
    unsigned char macPduRlcSeg2[] = {
        0x3F, 0x3E, 0x01, 0x00, 0x00, 0x00, 0xB0, 0x03, 0x02, 0x26, 
        0x80, 0xF2, 0x4E, 0x80, 0x00, 0x00, 0x00, 0x00
    };   
    
    // send segment 1
    unsigned short length = 0;
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

    MacUlSchDataInd(gMsgBuffer, length);

    usleep(5000);

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

    MacUlSchDataInd(gMsgBuffer, length);
}