/*
 * UlpTestCommon.cpp
 *
 *  Created on: Apr 28, 2018
 *      Author: j.zh
 */

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "UlpTestCommon.h"
#include "lteIntegrationPoint.h"
#include "TestSuite.h"

unsigned char gMsgBuffer[2048];

// MacUeDataInd_t gMacUeDataInd = {0};
// RlcPdcpUeDataInd_test gRlcUeDataInd = {0};
// RlcPdcpUeDataInd_test gPdcpUeDataInd = {0};
// RrcUeDataInd_test gRrcUeDataInd = {0};

MacUeDataInd_Test_Array gMacUeDataInd = {0};
RlcPdcpUeDataInd_Test_Array gRlcUeDataInd = {0};
RlcPdcpUeDataInd_Test_Array gPdcpUeDataInd = {0};
RrcUeDataInd_Test_Array gRrcUeDataInd = {0};

unsigned int gCallMacDataInd = 1;
unsigned int gCallRlcDataInd = 1;
unsigned int gCallPdcpDataInd = 1;
unsigned int gCallRrcDataInd = 1;

// --------------------------------------------------
extern "C" {
// ----------------------
int IP_Call_Mac_Data_Ind(void* pData)
{
    // printf("IP_Call_Mac_Data_Ind, pMacDataInd = %p\n", pData);

    MacUeDataInd_t* pMacDataInd = (MacUeDataInd_t*)pData;
    if (pMacDataInd == 0) {
        return gCallMacDataInd;
    }

    MacUeDataInd_t* pUeInd = (MacUeDataInd_t*)&gMacUeDataInd.ueDataIndArray[gMacUeDataInd.numUe];
    gMacUeDataInd.numUe++;

    pUeInd->rnti = pMacDataInd->rnti;
    if (pMacDataInd->rlcData != 0) {
        pUeInd->rlcData = new RlcUlData();
        pUeInd->rlcData->numLCInfo = pMacDataInd->rlcData->numLCInfo;
        unsigned int i = 0;
        for (i=0; i<pMacDataInd->rlcData->numLCInfo; i++) {
            pUeInd->rlcData->rlcDataArray[i].lcId = pMacDataInd->rlcData->rlcDataArray[i].lcId;
            pUeInd->rlcData->rlcDataArray[i].length = pMacDataInd->rlcData->rlcDataArray[i].length;
            if (pMacDataInd->rlcData->rlcDataArray[i].rlcdataBuffer == 0) {
                return gCallMacDataInd;
            }
            pUeInd->rlcData->rlcDataArray[i].rlcdataBuffer = new unsigned char[pMacDataInd->rlcData->rlcDataArray[i].length];
            memcpy(pUeInd->rlcData->rlcDataArray[i].rlcdataBuffer, pMacDataInd->rlcData->rlcDataArray[i].rlcdataBuffer,
                pMacDataInd->rlcData->rlcDataArray[i].length);
        }
    }

    return gCallMacDataInd;
}

// -----------------------
int IP_Call_Rlc_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size, void* ulRptInfoList)
{
    // printf("IP_Call_Rlc_Data_Ind, rnti = %d, lcId = %d, pData = %p, size = %d\n", rnti, lcId, pData, size);

    if (pData == 0) {
        return gCallRlcDataInd;
    }

    RlcPdcpUeDataInd_test* pRlcUeInd = (RlcPdcpUeDataInd_test*)&gRlcUeDataInd.ueDataIndArray[gRlcUeDataInd.numUe];
    gRlcUeDataInd.numUe++;
    pRlcUeInd->rnti = rnti;
    pRlcUeInd->lcId = lcId;
    pRlcUeInd->size = size;
    if (ulRptInfoList) {
        memcpy((void*)&pRlcUeInd->ulRptInfoList, ulRptInfoList, sizeof(UlReportInfoList));
    }
    pRlcUeInd->pData = new unsigned char[size];
    memcpy(pRlcUeInd->pData, pData, size);

    return gCallRlcDataInd;
}

// -----------------------
int IP_Call_Pdcp_Srb_Data_Ind(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    // printf("IP_Call_Pdcp_Srb_Data_Ind, rnti = %d, lcId = %d, pData = %p, size = %d\n", rnti, lcId, pData, size);
    if (pData == 0) {
        return gCallPdcpDataInd;
    }

    RlcPdcpUeDataInd_test* pPdcpUeInd = (RlcPdcpUeDataInd_test*)&gPdcpUeDataInd.ueDataIndArray[gPdcpUeDataInd.numUe];
    gPdcpUeDataInd.numUe++;
    pPdcpUeInd->rnti = rnti;
    pPdcpUeInd->lcId = lcId;
    pPdcpUeInd->size = size;
    pPdcpUeInd->pData = new unsigned char[size];
    memcpy(pPdcpUeInd->pData, pData, size);

    return gCallPdcpDataInd;
}

// -----------------------
int IP_Call_Rrc_Data_Ind(void* pData)
{
    printf("IP_Call_Rrc_Data_Ind\n");
    RrcUeDataInd_test* pRrcUeInd = (RrcUeDataInd_test*)&gRrcUeDataInd.ueDataIndArray[gRrcUeDataInd.numUe];
    memcpy((void*)&pRrcUeInd->ueContext, pData, sizeof(RrcUeContext));
    return gCallRrcDataInd;
}

// -----------------------
int IP_Rrc_Decode_Result(unsigned short rnti, unsigned char rrcMsgType, unsigned char nasMsgType, void* pData)
{
    printf("IP_Rrc_Decode_Result, rnti = %d, rrcMsgType = 0x%02x, nasMsgType = 0x%02x, pData = %p\n", rnti, rrcMsgType, nasMsgType, pData);

    RrcUeDataInd_test* pRrcUeInd = (RrcUeDataInd_test*)&gRrcUeDataInd.ueDataIndArray[gRrcUeDataInd.numUe];
    gRrcUeDataInd.numUe++;
    pRrcUeInd->rnti = rnti;
    pRrcUeInd->rrcMsgType = rrcMsgType;
    pRrcUeInd->nasMsgType = nasMsgType;
    // if (pData != 0) {
    //     memcpy((void*)&pRrcUeInd->identityResp, pData, sizeof(LIBLTE_MME_ID_RESPONSE_MSG_STRUCT));
    // }

    return gCallRrcDataInd;
}

} // end of extern C