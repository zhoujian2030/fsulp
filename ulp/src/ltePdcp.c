/*
 * ltePdcp.c
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#include "ltePdcp.h"
#include "baseType.h"
#include "ltePdcpRlcInterface.h"
#include "mempool.h"
#ifdef OS_LINUX
#include "CLogger.h"
#else

#endif

static void PdcpProcessRxSrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size);
static void PdcpProcessRxDrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size);

// -----------------------------
void RlcUeDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    if ((pData == 0) || (size == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], pData = %p, rnti = %d, lcId = %d, data size = %d\n", __func__, pData, rnti, lcId, size);
        return;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rnti = %d, lcId = %d, data size = %d\n", __func__, rnti, lcId, size);
    LOG_BUFFER(pData, size);

    if (lcId <= 2) {
        PdcpProcessRxSrb(rnti, lcId, pData, size);
    } else {
        PdcpProcessRxDrb(rnti, lcId, pData, size);
    }
}

// -----------------------------
static void PdcpProcessRxSrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size)
{
    UInt16 sn = pData[0] & PDCP_TS_PDU_SRB_SN_MASK;
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rnti = %d, lcId = %d, data size = %d, sn = %d\n", __func__, rnti, lcId, size, sn);

    if (lcId < 2) {
        // remove one byte header and send to RLC
        memmove(pData, pData+1, size-1);
        PdcpUeSrbDataInd(rnti, lcId, pData, size-1);
    } else {
        // data is ciphered
        LOG_TRACE(ULP_LOGGER_NAME, "[%s], TODO, data is ciphered ??\n", __func__);
    }
}

// -----------------------------
static void PdcpProcessRxDrb(UInt16 rnti, UInt16 lcId, UInt8* pData, UInt16 size)
{
    
}