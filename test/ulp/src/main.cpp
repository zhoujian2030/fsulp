/*
 * main.c
 *
 *  Created on: Apr 16, 2018
 *      Author: j.zh
 */
#if 0 
#include "CLogger.h"
#include "lteMac.h"
#include "lteMacPhyInterface.h"
#include <unistd.h>
#include "lteRlc.h"

extern unsigned int gLogLevel;

unsigned char gMsgBuffer[2048];

void Test_Rlc_No_Segment();
void Test_Rlc_SDU_2_Segment();

// --------------------------
int main(int argc, char* argv[]) {
    LOG_DBG(ULP_LOGGER_NAME, "[%s], main entry\n", __func__);

    gLogLevel = 2

    void InitRlcLayer();

    // // --------------------------
    // unsigned char macPdu[] = { 
    //     0x3D, 0x21, 0x02, 0x21, 0x80, 0x17, 0x1F, // mac header
    //     0x00, // bsr value
    //     0x00, 0x08, // rlc status pdu
    //     0xa0, 0x01, // rlc data pdu header
    //     0x01,  // pdcp header
    //     0x48, 0x02, 0x22, 0xfd, 0xf9, 0x33, 0x37, 0xa2, 0xa0, 0xea, 
    //     0xc1, 0x09, 0x20, 0xc6, 0x02, 0x00, 0x00, 0x10, 0x24, 0xa0}; 

    // Test_Rlc_No_Segment();

    Test_Rlc_SDU_2_Segment();

    return 1;
}

// --------------------------
void Test_Rlc_No_Segment()
{
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

    PhyUlDataInd(gMsgBuffer, length);
}

// ----------------------------
void Test_Rlc_SDU_2_Segment()
{
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

    PhyUlDataInd(gMsgBuffer, length);

    usleep(5000);

    // send segment 1
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

    PhyUlDataInd(gMsgBuffer, length);
}

#endif