/*
 * lteRrc.c
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#include <string.h>
#include "lteRrc.h"
#include "baseType.h"
#include "lteRrcPdcpInterface.h"
#include "mempool.h"
#include "asn1.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"
#include "lteKpi.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gUlRRcMsgName, ".ulpata");
#endif
UInt8 gUlRRcMsgName[17][50] = {
    "Csfb Params Request CDMA2000",
    "Measurement Report",
    "RRC Connection Reconfig Complete",
    "RRC Connection Reestablish Complete",
    "RRC Connection Setup Complete",
    "Security Mode Complete",
    "Security Mode Failure",
    "UE Capability Info",
    "UL Handover Prepare Transfer",
    "UL Information Transfer",
    "Counter Check Response",
    "UE Information Response",
    "Proximity Indication",
    "RN Reconfig Complete",
    "Spare2",
    "Spare1",
    "N Items"};

void RrcParseUlDcchMsg(UInt16 rnti, UInt8* pData, UInt16 size);
void RrcParseUlCcchMsg(UInt16 rnti, UInt8* pData, UInt16 size);

// --------------------------------
void PdcpUeSrbDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    if ((pData == 0) || (size == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], pData = %p, rnti = %d, lcId = %d, data size = %d\n", __func__, pData, rnti, lcId, size);
        return;
    }

    LOG_DBG(ULP_LOGGER_NAME, "[%s], rnti = %d, lcId = %d, data size = %d\n", __func__, rnti, lcId, size);
    LOG_BUFFER(pData, size);

    if (lcId > 0) {
        RrcParseUlDcchMsg(rnti, pData, size);
    } else {
        RrcParseUlCcchMsg(rnti, pData, size);
    }
}

// --------------------------------
void RrcParseUlDcchMsg(UInt16 rnti, UInt8* pData, UInt16 size)
{
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rnti = %d, data size = %d\n", __func__, rnti, size);

    UInt8 rrcMsgType = (*pData >> 3) & 0x0f;
    UInt8 nasMsgType = 0xff;
    LIBLTE_MME_ID_RESPONSE_MSG_STRUCT idResp;
    UInt8 imsi[16];
    UInt32 i = 0;

    LOG_DBG(ULP_LOGGER_NAME, "[%s], rnti = %d, rrcMsgType = %d\n", __func__, rnti, rrcMsgType);
    if (rrcMsgType <= RRC_UL_DCCH_MSG_TYPE_N_ITEMS) {
        LOG_INFO(ULP_LOGGER_NAME, "[%s], UE ---> NB: %s (RNTI: %d)\n", __func__, gUlRRcMsgName[rrcMsgType], rnti);
    }

    if (RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER == rrcMsgType) {
        if (ASN1_SUCCES == parseUlDTMsg(pData, size, &nasMsgType, &idResp)) {

            IP_RRC_DECODE_RESULT(rnti, rrcMsgType, nasMsgType, &idResp);

            if (NAS_MSG_TYPE_IDENTITY_RESPONSE == nasMsgType) {
                LOG_INFO(ULP_LOGGER_NAME, "[%s], UE ---> NB: Identity Response (RNTI: %d)\n", __func__, rnti);
                gLteKpi.idResp++;
                if (LIBLTE_MME_MOBILE_ID_TYPE_IMSI == idResp.mobile_id.type_of_id) {
                    gLteKpi.imsi++;
                    for (i = 0; i < 15; i++) {
                        imsi[i] = idResp.mobile_id.imsi[i] + 0x30;
                    }
                    imsi[15] = '\0';
                    LOG_INFO(ULP_LOGGER_NAME, "[%s], rnti = %d, imsi = %s\n", __func__, rnti, imsi);
                } else {
                    LOG_INFO(ULP_LOGGER_NAME, "[%s], idResp.mobile_id.type_of_id = %d\n", __func__, idResp.mobile_id.type_of_id);
                }
            } else if (NAS_MSG_TYPE_ATTACH_REQUEST == nasMsgType) {
                LOG_INFO(ULP_LOGGER_NAME, "[%s], UE ---> NB: Attach Request (RNTI: %d)\n", __func__, rnti);
            } else if (NAS_MSG_TYPE_DETACH_REQUEST == nasMsgType) {
                LOG_INFO(ULP_LOGGER_NAME, "[%s], UE ---> NB: Detach Request (RNTI: %d)\n", __func__, rnti);
            } else if (NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST == nasMsgType) {
                LOG_INFO(ULP_LOGGER_NAME, "[%s], UE ---> NB: TAU Request (RNTI: %d)\n", __func__, rnti);
            } else {
                LOG_INFO(ULP_LOGGER_NAME, "[%s], nasMsgType = 0x%x\n", __func__, nasMsgType);
            }
        } else {
            IP_RRC_DECODE_RESULT(rnti, rrcMsgType, 0xff, 0);
        }
    } else {
        IP_RRC_DECODE_RESULT(rnti, rrcMsgType, 0xff, 0);
    }

    MemFree(pData);
}

// --------------------------------
void RrcParseUlCcchMsg(UInt16 rnti, UInt8* pData, UInt16 size)
{
    LOG_WARN(ULP_LOGGER_NAME, "[%s], TODO, rnti = %d, data size = %d\n", __func__, rnti, size);

    if (pData != 0) {
        MemFree(pData);
    }
}
