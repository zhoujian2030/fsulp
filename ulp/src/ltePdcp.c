/*
 * ltePdcp.c
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#include "ltePdcp.h"
#include "lteCommon.h"
#include "ltePdcpRlcInterface.h"
#include "lteRrcPdcpInterface.h"
#include "mempool.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"

void PdcpProcessRxSrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList);
void PdcpProcessRxDrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList);

// -----------------------------
void RlcUeDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size, UlReportInfoList* pUlRptInfoList)
{
    if ((pData == 0) || (size == 0) || (pUlRptInfoList == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "pData = %p, rnti = %d, lcId = %d, data size = %d\n", pData, rnti, lcId, size);
        return;
    }

    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, data size = %d, numRlcSeg = %d\n", rnti, lcId, size, pUlRptInfoList->count);
#if 0
    unsigned int i;
    for (i=0; i<pUlRptInfoList->count; i++) {
        LOG_DBG(ULP_LOGGER_NAME, "ulRptInfo[%d].rbNum = %d\n", i, pUlRptInfoList->ulRptInfo[i].rbNum)
    }
#endif
    LOG_BUFFER(pData, size);

    if (lcId <= 2) {
        PdcpProcessRxSrb(rnti, lcId, pData, size, pUlRptInfoList);
    } else {
        PdcpProcessRxDrb(rnti, lcId, pData, size, pUlRptInfoList);
    }
}

// -----------------------------
void PdcpProcessRxSrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList)
{
    UInt16 sn = pData[0] & PDCP_TS_PDU_SRB_SN_MASK;
    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, data size = %d, sn = %d\n", rnti, lcId, size, sn);

    if (lcId < 2) {
        // remove one byte header and send to RRC
        MemRemove(pData, 0, 1);
        if (!IP_PDCP_SRB_DATA_IND(rnti, lcId, pData, size-1)) {
            return;
        }
        PdcpUeSrbDataInd(rnti, lcId, pData, size-1, pUlRptInfoList);
    } else {
        // data is ciphered
        LOG_TRACE(ULP_LOGGER_NAME, "TODO, data is ciphered ??\n");
        if (pData != 0) {
            MemFree(pData);
        }
    }
}

// -----------------------------
void PdcpProcessRxDrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList)
{
    // LOG_TRACE(ULP_LOGGER_NAME, "TODO, rnti = %d, lcId = %d, data size = %d\n", rnti, lcId, size);


    if (pData != 0) {
        MemFree(pData);
    }
}
