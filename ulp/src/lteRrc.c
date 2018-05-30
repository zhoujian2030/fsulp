/*
 * lteRrc.c
 *
 *  Created on: Apr 24, 2018
 *      Author: j.zh
 */

#include <string.h>
#include "lteRrc.h"
#include "lteCommon.h"
#include "lteRrcPdcpInterface.h"
#include "mempool.h"
#include "asn1.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"
#include "lteKpi.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gUlRRcMsgName, ".ulpdata");
#pragma DATA_SECTION(gRrcUeContextList, ".ulpdata");
#endif
List gRrcUeContextList;
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
unsigned int RrcParseNasMsg(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff);

// --------------------------------
void InitRrcLayer()
{
    ListInit(&gRrcUeContextList, 1);
}

// --------------------------------
RrcUeContext* RrcGetUeContext(UInt16 rnti)
{
    RrcUeContext* pUeCtx = (RrcUeContext*)ListGetFirstNode(&gRrcUeContextList);
    while (pUeCtx != 0) {
        if (pUeCtx->rnti == rnti) {
            return pUeCtx;
        } else {
            pUeCtx = (RrcUeContext*)ListGetNextNode(&pUeCtx->node);
        }
    }

    LOG_TRACE(ULP_LOGGER_NAME, "UE context NOT exists, rnti = %d\n", rnti);
    return 0;    
}

// --------------------------------
RrcUeContext* RrcCreateUeContext(UInt16 rnti)
{
    RrcUeContext* pUeCtx = (RrcUeContext*)MemAlloc(sizeof(RrcUeContext));
    if (pUeCtx == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for rrc ue context\n");
        return 0;
    }
    LOG_INFO(ULP_LOGGER_NAME, "pUeCtx = %p, rnti = %d\n", pUeCtx, rnti);
    memset(pUeCtx, 0, sizeof(RrcUeContext));
    pUeCtx->rnti = rnti; 
    // SemInit(&pUeCtx->lockOfCount, 1);
    // pUeCtx->idleCount = 0;
    ListPushNode(&gRrcUeContextList, &pUeCtx->node);
    // KpiCountRrcUeCtx(TRUE);

    return pUeCtx;
}

// --------------------------------
void RrcDeleteUeContext(RrcUeContext* pRrcUeCtx)
{
    if (pRrcUeCtx != 0) {
        LOG_INFO(ULP_LOGGER_NAME, "pRrcUeCtx = %p, rnti = %d\n", pRrcUeCtx, pRrcUeCtx->rnti);
        // KpiCountRrcUeCtx(FALSE);
        // SemDestroy(&pRlcUeCtx->lockOfCount);
        ListDeleteNode(&gRrcUeContextList, &pRrcUeCtx->node);
        MemFree(pRrcUeCtx);
    }    
}

// --------------------------------
int RrcGetUeContextCount()
{
    return ListCount(&gRrcUeContextList);
}

// --------------------------------
void PdcpUeSrbDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size)
{
    if ((pData == 0) || (size == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "pData = %p, rnti = %d, lcId = %d, data size = %d\n", pData, rnti, lcId, size);
        return;
    }

    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, data size = %d\n", rnti, lcId, size);
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
    LOG_TRACE(ULP_LOGGER_NAME, "rnti = %d, data size = %d\n", rnti, size);

    UInt8 rrcMsgType = (*pData >> 3) & 0x0f;
    UInt8 nasMsgType = 0xff;
    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, rrcMsgType = %d\n", rnti, rrcMsgType);

    if (rrcMsgType <= RRC_UL_DCCH_MSG_TYPE_N_ITEMS) {
        LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: %s (RNTI: %d)\n", gUlRRcMsgName[rrcMsgType], rnti);
    }

    if (RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER == rrcMsgType) {
        LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT* pUlInfoTransMsg = 
            (LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT*)MemAlloc(sizeof(LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT));
        if (pUlInfoTransMsg != 0) {
            if (ASN1_SUCCES == Asn1ParseUlInfoTransMsg(pData, size, pUlInfoTransMsg)) {
                if (pUlInfoTransMsg->dedicated_info_type == LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS) {
                    nasMsgType = RrcParseNasMsg(rnti, &pUlInfoTransMsg->dedicated_info);
                } else {
                    LOG_WARN(ULP_LOGGER_NAME, "dedicated_info_type = %d, rnti = %d\n", pUlInfoTransMsg->dedicated_info_type, rnti);
                }
            } else {
                LOG_ERROR(ULP_LOGGER_NAME, "Asn1ParseUlInfoTransMsg error, rnti = %d\n", rnti);
            }
            MemFree(pUlInfoTransMsg);
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT, rnti = %d\n", rnti);
        }
    } else if (RRC_UL_DCCH_MSG_TYPE_RRC_CON_SETUP_COMPLETE == rrcMsgType) {
        gLteKpi.rrcSetupCompl++;
        LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT* pRrcSetupComplMsg = 
            (LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT*)MemAlloc(sizeof(LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT));
        if (pRrcSetupComplMsg != 0) {
            if (ASN1_SUCCES == Asn1ParseRrcSetupComplMsg(pData, size, pRrcSetupComplMsg)) {
                nasMsgType = RrcParseNasMsg(rnti, &pRrcSetupComplMsg->dedicated_info_nas);
            } else {
                LOG_ERROR(ULP_LOGGER_NAME, "Asn1ParseRrcSetupComplMsg error, rnti = %d\n", rnti);
            }  
            MemFree(pRrcSetupComplMsg);   
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT, rnti = %d\n", rnti);
        }  
    }
    
    IP_RRC_DECODE_RESULT(rnti, rrcMsgType, nasMsgType, 0);

    MemFree(pData);
}

// --------------------------------
void RrcParseUlCcchMsg(UInt16 rnti, UInt8* pData, UInt16 size)
{
    LOG_WARN(ULP_LOGGER_NAME, "TODO, rnti = %d, data size = %d\n", rnti, size);

    if (pData != 0) {
        MemFree(pData);
    }
}

// --------------------------------
void RrcDecodeIdentityResponse(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Identity Response (RNTI: %d)\n", rnti);
    RrcUeContext* pUeCtx;
    UInt32 i;
    LIBLTE_MME_ID_RESPONSE_MSG_STRUCT* pIdResp = (LIBLTE_MME_ID_RESPONSE_MSG_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_ID_RESPONSE_MSG_STRUCT));
    if (pIdResp == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_MME_ID_RESPONSE_MSG_STRUCT, rnti = %d\n", rnti);
        return;
    }

    if (ASN1_SUCCES == liblte_mme_unpack_identity_response_msg(pNasMsgBuff, pIdResp)) {
        gLteKpi.idResp++;

        if (LIBLTE_MME_MOBILE_ID_TYPE_IMSI == pIdResp->mobile_id.type_of_id) {
            pUeCtx = RrcGetUeContext(rnti);
            if (pUeCtx == 0) {
                pUeCtx = RrcCreateUeContext(rnti);
                if (pUeCtx == 0) {
                    LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                    MemFree(pIdResp);
                    return;
                }
            }
            gLteKpi.imsi++;
            if (pUeCtx->ueIdentity.imsiPresent) {
                LOG_WARN(ULP_LOGGER_NAME, "update imsi, rnti = %d\n", rnti);
            } else {
                pUeCtx->ueIdentity.imsiPresent = TRUE;
            }
            memcpy(pUeCtx->ueIdentity.imsi, pIdResp->mobile_id.imsi, 15);

            // for print test
#ifdef OS_LINUX
            UInt8 imsi[16];
            for (i = 0; i < 15; i++) {
                imsi[i] = pIdResp->mobile_id.imsi[i] + 0x30;
            }
            imsi[15] = '\0'; 

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, imsi);
#else
            UInt8* imsiOctect = pIdResp->mobile_id.imsi;
            i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                LOG_INFO(ULP_LOGGER_NAME, "both M-MSI and IMSI are collected, rnti = %d, M-TMSI = %d\n", rnti, pUeCtx->ueIdentity.mTmsi);
            } else {
                LOG_INFO(ULP_LOGGER_NAME, "Miss M-TMSI, rnti = %d\n", rnti);
            }
            
            // TODO
            // send to OAM ??
            RrcUeDataInd(pUeCtx);
        } else {
            LOG_INFO(ULP_LOGGER_NAME, "pIdResp->mobile_id.type_of_id = %d, rnti = %d\n", pIdResp->mobile_id.type_of_id, rnti);
        }
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_identity_response_msg error, rnti = %d\n", rnti);
    }

    MemFree(pIdResp);
}

// --------------------------------
void RrcDecodeAttachReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Attach Request (RNTI: %d)\n", rnti);
    RrcUeContext* pUeCtx;
    UInt32 i;
    LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT* pAttachReq = (LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT));
    if (pAttachReq == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_MME_ATTACH_REQUEST_MSG_STRUCT, rnti = %d\n", rnti);
        return;
    }

    if (ASN1_SUCCES == liblte_mme_unpack_attach_request_msg(pNasMsgBuff, pAttachReq)) {
        gLteKpi.attachReq++;

        pUeCtx = RrcGetUeContext(rnti);
        if (pUeCtx == 0) {
            pUeCtx = RrcCreateUeContext(rnti);
            if (pUeCtx == 0) {
                LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                MemFree(pAttachReq);
                return;
            }
        }

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == pAttachReq->eps_mobile_id.type_of_id) {
            gLteKpi.mTmsi++;
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                LOG_WARN(ULP_LOGGER_NAME, "update M-TMSI, rnti = %d\n", rnti);
            } else {
                pUeCtx->ueIdentity.mTmsiPresent = TRUE;
            }
            pUeCtx->ueIdentity.mTmsi = pAttachReq->eps_mobile_id.guti.m_tmsi;

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pAttachReq->eps_mobile_id.guti.m_tmsi,
                pAttachReq->eps_mobile_id.guti.mcc,
                pAttachReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pAttachReq->eps_mobile_id.type_of_id) {
            gLteKpi.imsi++;
            if (pUeCtx->ueIdentity.imsiPresent) {
                LOG_WARN(ULP_LOGGER_NAME, "update imsi, rnti = %d\n", rnti);
            } else {
                pUeCtx->ueIdentity.imsiPresent = TRUE;
            }
            memcpy(pUeCtx->ueIdentity.imsi, pAttachReq->eps_mobile_id.imsi, 15);

#ifdef OS_LINUX
            UInt8 imsi[16];
            // for test print
            for (i = 0; i < 15; i++) {
                imsi[i] = pAttachReq->eps_mobile_id.imsi[i] + 0x30;
            }
            imsi[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, imsi);
#else
            UInt8* imsiOctect = pAttachReq->eps_mobile_id.imsi;
            i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            for (i = 0; i < 15; i++) {
                imei[i] = pAttachReq->eps_mobile_id.imei[i] + 0x30;
            }
            imei[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "TODO, rnti = %d, imei = %s\n", rnti, imei);
        }

        // TODO
        RrcUeDataInd(pUeCtx);
    } else {                        
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_attach_request_msg error, rnti = %d\n", rnti);
    }

    MemFree(pAttachReq);
}

// --------------------------------
void RrcDecodeDetachReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Detach Request (RNTI: %d)\n", rnti);
    RrcUeContext* pUeCtx;
    LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT* pDetachReq = (LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT));
    UInt32 i;
    if (pDetachReq == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT, rnti = %d\n", rnti);
        return;
    }

    if (ASN1_SUCCES == liblte_mme_unpack_detach_request_msg(pNasMsgBuff, pDetachReq)) {
        gLteKpi.detachReq++;

        pUeCtx = RrcGetUeContext(rnti);
        if (pUeCtx == 0) {
            pUeCtx = RrcCreateUeContext(rnti);
            if (pUeCtx == 0) {
                LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                MemFree(pDetachReq);
                return;
            }
        }

        pUeCtx->ueIdentity.detachFlag = TRUE;

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == pDetachReq->eps_mobile_id.type_of_id) {
            gLteKpi.mTmsi++;
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                if (pUeCtx->ueIdentity.mTmsi != pDetachReq->eps_mobile_id.guti.m_tmsi) {
                    LOG_WARN(ULP_LOGGER_NAME, "update M-TMSI, rnti = %d\n", rnti);
                }
            } else {
                pUeCtx->ueIdentity.mTmsiPresent = TRUE;
            }
            pUeCtx->ueIdentity.mTmsi = pDetachReq->eps_mobile_id.guti.m_tmsi;

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pDetachReq->eps_mobile_id.guti.m_tmsi,
                pDetachReq->eps_mobile_id.guti.mcc,
                pDetachReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pDetachReq->eps_mobile_id.type_of_id) {
            gLteKpi.imsi++;
            if (pUeCtx->ueIdentity.imsiPresent) {
                if (memcmp(pUeCtx->ueIdentity.imsi, pDetachReq->eps_mobile_id.imsi, 15) != 0) {
                    LOG_WARN(ULP_LOGGER_NAME, "update imsi, rnti = %d\n", rnti);
                }
            } else {
                pUeCtx->ueIdentity.imsiPresent = TRUE;
            }
            memcpy(pUeCtx->ueIdentity.imsi, pDetachReq->eps_mobile_id.imsi, 15);

#ifdef OS_LINUX
            UInt8 imsi[16];
            // for test print
            for (i = 0; i < 15; i++) {
                imsi[i] = pDetachReq->eps_mobile_id.imsi[i] + 0x30;
            }
            imsi[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, imsi);
#else
            UInt8* imsiOctect = pDetachReq->eps_mobile_id.imsi;
            i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            for (i = 0; i < 15; i++) {
                imei[i] = pDetachReq->eps_mobile_id.imei[i] + 0x30;
            }
            imei[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "TODO, rnti = %d, imei = %s\n", rnti, imei);
        }

        RrcUeDataInd(pUeCtx);
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_detach_request_msg error, rnti = %d\n", rnti);
    }

    MemFree(pDetachReq);
}

// --------------------------------
void RrcDecodeExtServiceReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Extended Service Request (RNTI: %d)\n", rnti);
    LOG_BUFFER(pNasMsgBuff->msg, pNasMsgBuff->N_bytes);

    RrcUeContext* pUeCtx;
    LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT* pExtServReq = (LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT));
    UInt32 i;
    if (pExtServReq == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT, rnti = %d\n", rnti);
        return;
    }

    if (ASN1_SUCCES == liblte_mme_unpack_ext_service_request_msg(pNasMsgBuff, pExtServReq)) {
        gLteKpi.extServReq++;

        pUeCtx = RrcGetUeContext(rnti);
        if (pUeCtx == 0) {
            pUeCtx = RrcCreateUeContext(rnti);
            if (pUeCtx == 0) {
                LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                MemFree(pExtServReq);
                return;
            }
        }

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_TMSI == pExtServReq->eps_mobile_id.type_of_id) {
            gLteKpi.mTmsi++;
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                if (pUeCtx->ueIdentity.mTmsi != pExtServReq->eps_mobile_id.m_tmsi) {
                    LOG_WARN(ULP_LOGGER_NAME, "update M-TMSI, rnti = %d\n", rnti);
                }
            } else {
                pUeCtx->ueIdentity.mTmsiPresent = TRUE;
            }
            pUeCtx->ueIdentity.mTmsi = pExtServReq->eps_mobile_id.m_tmsi;

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x\n", rnti, pExtServReq->eps_mobile_id.m_tmsi);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pExtServReq->eps_mobile_id.type_of_id) {
            gLteKpi.imsi++;
            if (pUeCtx->ueIdentity.imsiPresent) {
                if (memcmp(pUeCtx->ueIdentity.imsi, pExtServReq->eps_mobile_id.imsi, 15) != 0) {
                    LOG_WARN(ULP_LOGGER_NAME, "update imsi, rnti = %d\n", rnti);
                }
            } else {
                pUeCtx->ueIdentity.imsiPresent = TRUE;
            }
            memcpy(pUeCtx->ueIdentity.imsi, pExtServReq->eps_mobile_id.imsi, 15);

#ifdef OS_LINUX
            UInt8 imsi[16];
            // for test print
            for (i = 0; i < 15; i++) {
                imsi[i] = pExtServReq->eps_mobile_id.imsi[i] + 0x30;
            }
            imsi[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, imsi);
#else
            UInt8* imsiOctect = pExtServReq->eps_mobile_id.imsi;
            i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            for (i = 0; i < 15; i++) {
                imei[i] = pExtServReq->eps_mobile_id.imei[i] + 0x30;
            }
            imei[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "TODO, rnti = %d, imei = %s\n", rnti, imei);
        }

        RrcUeDataInd(pUeCtx);
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_ext_service_request_msg error, rnti = %d\n", rnti);
    }

    MemFree(pExtServReq);
}

// --------------------------------
void RrcDecodeTauReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: TAU Request (RNTI: %d)\n", rnti);

    RrcUeContext* pUeCtx;
    UInt32 i;
    LIBLTE_MME_TAU_REQ_STRUCT* pTauReq = (LIBLTE_MME_TAU_REQ_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_TAU_REQ_STRUCT));
    if (pTauReq == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_MME_TAU_REQ_STRUCT, rnti = %d\n", rnti);
        return;
    }

    if (ASN1_SUCCES == liblte_mme_unpack_tau_request_msg(pNasMsgBuff, pTauReq)) {
        gLteKpi.tauReq++;

        pUeCtx = RrcGetUeContext(rnti);
        if (pUeCtx == 0) {
            pUeCtx = RrcCreateUeContext(rnti);
            if (pUeCtx == 0) {
                LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                MemFree(pTauReq);
                return;
            }
        }

        if(LIBLTE_MME_EPS_MOBILE_ID_TYPE_GUTI == pTauReq->eps_mobile_id.type_of_id) {
            gLteKpi.mTmsi++;
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                LOG_WARN(ULP_LOGGER_NAME, "update M-TMSI, rnti = %d\n", rnti);
            } else {
                pUeCtx->ueIdentity.mTmsiPresent = TRUE;
            }
            pUeCtx->ueIdentity.mTmsi = pTauReq->eps_mobile_id.guti.m_tmsi;

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pTauReq->eps_mobile_id.guti.m_tmsi,
                pTauReq->eps_mobile_id.guti.mcc,
                pTauReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pTauReq->eps_mobile_id.type_of_id) {
            gLteKpi.imsi++;
            if (pUeCtx->ueIdentity.imsiPresent) {
                LOG_WARN(ULP_LOGGER_NAME, "update imsi, rnti = %d\n", rnti);
            } else {
                pUeCtx->ueIdentity.imsiPresent = TRUE;
            }
            memcpy(pUeCtx->ueIdentity.imsi, pTauReq->eps_mobile_id.imsi, 15);

#ifdef OS_LINUX
            UInt8 imsi[16];
            // for test print
            for (i = 0; i < 15; i++) {
                imsi[i] = pTauReq->eps_mobile_id.imsi[i] + 0x30;
            }
            imsi[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, imsi);
#else
            UInt8* imsiOctect = pTauReq->eps_mobile_id.imsi;
            i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            for (i = 0; i < 15; i++) {
                imei[i] = pTauReq->eps_mobile_id.imei[i] + 0x30;
            }
            imei[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "TODO, rnti = %d, imei = %s\n", rnti, imei);
        }

        // TODO
        RrcUeDataInd(pUeCtx);
    } else {                        
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_tau_request_msg error, rnti = %d\n", rnti);
    }

    MemFree(pTauReq);
}

// --------------------------------
unsigned int RrcParseNasMsg(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    UInt8 pd;
    UInt8 msgType = 0xff; 

    if (pNasMsgBuff == 0) {
        return msgType;
    }    

    if (ASN1_ERROR == liblte_mme_parse_msg_header(pNasMsgBuff, &pd, &msgType)) {
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_parse_msg_header error, rnti = %d\n", rnti);
        return msgType;
    }

    switch (msgType) {
        case NAS_MSG_TYPE_IDENTITY_RESPONSE:
        {
            RrcDecodeIdentityResponse(rnti, pNasMsgBuff);
            break;
        }

        case NAS_MSG_TYPE_ATTACH_REQUEST:
        {
            RrcDecodeAttachReq(rnti, pNasMsgBuff);
            break;
        }

        case NAS_MSG_TYPE_TRACKING_AREA_UPDATE_REQUEST:
        {
            RrcDecodeTauReq(rnti, pNasMsgBuff);
            break;
        }

        case NAS_MSG_TYPE_DETACH_REQUEST:
        {
            RrcDecodeDetachReq(rnti, pNasMsgBuff);
            break;
        }

        case NAS_MSG_TYPE_EXTENDED_SERVICE_REQUEST:
        {
            RrcDecodeExtServiceReq(rnti, pNasMsgBuff);
            break;
        }

        default:
        {
            LOG_WARN(ULP_LOGGER_NAME, "msgType = 0x%02x, rnti = %d\n", msgType, rnti);
            break;
        }
    }

    return msgType;
}

// --------------------------------
void RrcUeDataInd(RrcUeContext* pRrcUeCtx)
{
    if (pRrcUeCtx == 0) {
        return;
    }

    LOG_INFO(ULP_LOGGER_NAME, "imsiPresent = %d, mTmsiPresent = %d, detachFlag = %d, rnti = %d\n", 
        pRrcUeCtx->ueIdentity.imsiPresent, pRrcUeCtx->ueIdentity.mTmsiPresent, 
        pRrcUeCtx->ueIdentity.detachFlag, pRrcUeCtx->rnti);
    
    LOG_INFO(ULP_LOGGER_NAME, "KPI: idResp = %d, attachReq = %d\n", gLteKpi.idResp, gLteKpi.attachReq);
    
    if (!IP_RRC_DATA_IND(pRrcUeCtx)) {
        return;
    }

    // TODO send data to OAM

    // if send data to OAM, delete the UE context
    RrcDeleteUeContext(pRrcUeCtx);
}
