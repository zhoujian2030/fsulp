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
#include "lteRrcPdcpInterface.h"
#include "mempool.h"
#include "asn1.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"
#include "lteKpi.h"
#include "sync.h"
#include "target.h"

#ifndef OS_LINUX
#pragma DATA_SECTION(gUlRRcMsgName, ".ulpdata");
#pragma DATA_SECTION(gRrcUeContextList, ".ulpdata");
#pragma DATA_SECTION(gReadyRrcUeContextList, ".ulpdata");
#endif
List gRrcUeContextList;
List gReadyRrcUeContextList;
UInt8 gUlRRcMsgName[17][50] = {
    "Csfb Params Req CDMA2000",
    "Measurement Report",
    "RRC Conn Reconfig Compl",
    "RRC Conn Reestab Compl",
    "RRC Conn Setup Compl",
    "Sec Mode Compl",
    "Sec Mode Failure",
    "UE Cap Info",
    "UL HO Prep Trans",
    "UL Info Trans",
    "Counter Check Resp",
    "UE Info Resp",
    "Proximity Ind",
    "RN Reconfig Compl",
    "Spare2",
    "Spare1",
    "N Items"};

void RrcParseUlDcchMsg(UInt16 rnti, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList);
void RrcParseUlCcchMsg(UInt16 rnti, UInt8* pData, UInt16 size);
unsigned int RrcParseNasMsg(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff);
RrcUeContext* RrcUpdateImsi(RrcUeContext* pUeCtx, UInt8* imsi);
RrcUeContext* RrcUpdateMTmsi(RrcUeContext* pUeCtx, UInt32 mTmsi);

// --------------------------------
void InitRrcLayer()
{
    ListInit(&gRrcUeContextList, 1);
    ListInit(&gReadyRrcUeContextList, 1);    
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
    LOG_DBG(ULP_LOGGER_NAME, "pUeCtx = %p, rnti = %d\n", pUeCtx, rnti);
    memset(pUeCtx, 0, sizeof(RrcUeContext));
    pUeCtx->rnti = rnti; 
    // SemInit(&pUeCtx->lockOfCount, 1);
    pUeCtx->idleCount = 0;
    pUeCtx->deleteFlag = 0;
    ListPushNode(&gRrcUeContextList, &pUeCtx->node);
    // KpiCountRrcUeCtx(TRUE);

    return pUeCtx;
}

// --------------------------------
void RrcDeleteUeContext(RrcUeContext* pRrcUeCtx)
{
    if (pRrcUeCtx != 0) {
        LOG_DBG(ULP_LOGGER_NAME, "pRrcUeCtx = %p, rnti = %d\n", pRrcUeCtx, pRrcUeCtx->rnti);
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

// -----------------------------------
void RrcUpdateUeContextTime(RrcUeContext* pRrcUeCtx, unsigned int value)
{
    if (pRrcUeCtx != 0) {
        // SemWait(&pRlcUeCtx->lockOfCount);
        if (value == 0) {
            pRrcUeCtx->idleCount = 0;
        } else {
            pRrcUeCtx->idleCount += value;
        }
        // SemPost(&pRlcUeCtx->lockOfCount);
    }
}

// --------------------------------
void PdcpUeSrbDataInd(unsigned short rnti, unsigned short lcId, unsigned char* pData, unsigned short size, UlReportInfoList* pUlRptInfoList)
{
    if ((pData == 0) || (size == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "pData = %p, rnti = %d, lcId = %d, data size = %d\n", pData, rnti, lcId, size);
        return;
    }

    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, data size = %d\n", rnti, lcId, size);
    LOG_BUFFER(pData, size);

    if (lcId > 0) {
        RrcParseUlDcchMsg(rnti, pData, size, pUlRptInfoList);
    } else {
        RrcParseUlCcchMsg(rnti, pData, size);
    }
}

// --------------------------------
void MacUeCcchDataInd(unsigned short rnti, unsigned char* pData, unsigned short size, UlReportInfoList* pUlRptInfoList)
{
    if ((pData == 0) || (size == 0)) {
        LOG_ERROR(ULP_LOGGER_NAME, "pData = %p, rnti = %d, data size = %d\n", pData, rnti, size);
        return;
    }

    RrcParseUlCcchMsg(rnti, pData, size);
}

// --------------------------------
void RrcParseUlDcchMsg(UInt16 rnti, UInt8* pData, UInt16 size, UlReportInfoList* pUlRptInfoList)
{
    LOG_TRACE(ULP_LOGGER_NAME, "rnti = %d, data size = %d\n", rnti, size);

    UInt8 rrcMsgType = (*pData >> 3) & 0x0f;
    UInt8 nasMsgType = 0xff;
    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, rrcMsgType = %d\n", rnti, rrcMsgType);

    if (RRC_UL_DCCH_MSG_TYPE_UL_INFO_TRANSFER == rrcMsgType) {
        LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT* pUlInfoTransMsg = 
            (LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT*)MemAlloc(sizeof(LIBLTE_RRC_UL_INFORMATION_TRANSFER_STRUCT));
        if (pUlInfoTransMsg != 0) {
            if (ASN1_SUCCES == Asn1ParseUlInfoTransMsg(pData, size, pUlInfoTransMsg)) {
                if (pUlInfoTransMsg->dedicated_info_type == LIBLTE_RRC_UL_INFORMATION_TRANSFER_TYPE_NAS) {
                    nasMsgType = RrcParseNasMsg(rnti, &pUlInfoTransMsg->dedicated_info);
#ifdef PHY_DEBUG
                    if (NAS_MSG_TYPE_IDENTITY_RESPONSE == nasMsgType) {
                        unsigned int i; 
                        for (i=0; i<pUlRptInfoList->count; i++) {
                            if (pUlRptInfoList->ulRptInfo[i].rbNum <= 19) {
                                gLteKpi.idRespRbNum[pUlRptInfoList->ulRptInfo[i].rbNum - 1]++;
                            }
                        }
                    }
#endif
#ifdef DPE
#ifdef TARGET_LOCATION
                    if (NAS_MSG_TYPE_IDENTITY_RESPONSE == nasMsgType) {
                        RrcUeContext* pUeCtx = RrcGetUeContext(rnti);
                        if (pUeCtx != 0 && pUeCtx->rrcSetupComplRecvdFlag == 0) {
                            LOG_DBG(ULP_LOGGER_NAME, "recv identity resp but miss rrc setup complte, rnti = %d\n", rnti);
                            TgtProcessUeEstablishInfo(rnti, pUlRptInfoList);
                        }
                    }
#endif
#endif
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
        LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT* pRrcSetupComplMsg = 
            (LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT*)MemAlloc(sizeof(LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT));
        if (pRrcSetupComplMsg != 0) {
            if (ASN1_SUCCES == Asn1ParseRrcSetupComplMsg(pData, size, pRrcSetupComplMsg)) {
                LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: RRC Conn Setup Compl (%d)\n", rnti);
                gLteKpi.rrcSetupCompl++;
                nasMsgType = RrcParseNasMsg(rnti, &pRrcSetupComplMsg->dedicated_info_nas);
#ifdef PHY_DEBUG
                unsigned int i;
                for (i=0; i<pUlRptInfoList->count; i++) {
                    if (pUlRptInfoList->ulRptInfo[i].rbNum <= 19) {
                        gLteKpi.rrcSetupComplRbNum[pUlRptInfoList->ulRptInfo[i].rbNum - 1]++;
                    }
                }
#endif

#ifdef DPE
#ifdef TARGET_LOCATION
                // for find target
                if (0xff != nasMsgType) {
                    RrcUeContext* pUeCtx = RrcGetUeContext(rnti);
                    if (pUeCtx == 0) {
                        pUeCtx = RrcCreateUeContext(rnti);
                        if (pUeCtx != 0) {
                            pUeCtx->rrcSetupComplRecvdFlag = 1;
                        } else {
                            LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                        }
                    }
                    if (LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST == nasMsgType ||
                        NAS_MSG_TYPE_EXTENDED_SERVICE_REQUEST == nasMsgType) 
                    {                    
                        TgtProcessUeEstablishInfo(rnti, pUlRptInfoList);
                    }
                }
#endif
#endif
            } else {
                LOG_ERROR(ULP_LOGGER_NAME, "Asn1ParseRrcSetupComplMsg error, rnti = %d\n", rnti);
            }  
            MemFree(pRrcSetupComplMsg);   
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc memory for LIBLTE_RRC_CONNECTION_SETUP_COMPLETE_STRUCT, rnti = %d\n", rnti);
        }  
    } else {
        if (rrcMsgType <= RRC_UL_DCCH_MSG_TYPE_N_ITEMS) {
            LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: %s (%d)\n", gUlRRcMsgName[rrcMsgType], rnti);
        }
    }
    
    IP_RRC_DECODE_RESULT(rnti, rrcMsgType, nasMsgType, 0);

    MemFree(pData);
}

// --------------------------------
void RrcParseUlCcchMsg(UInt16 rnti, UInt8* pData, UInt16 size)
{
    if (pData == 0 || size == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pData = %p, rnti = %d, data size = %d\n", pData, rnti, size);
        return;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "rnti = %d, data size = %d\n", rnti, size);
    LOG_BUFFER(pData, size);

    UInt8 rrcCcchMsgType = (*pData >> 6) & 0x01;
    if (rrcCcchMsgType == LIBLTE_RRC_UL_CCCH_MSG_TYPE_RRC_CON_REQ) { 
        LIBLTE_RRC_CONNECTION_REQUEST_STRUCT* pRrcReqMsg = (LIBLTE_RRC_CONNECTION_REQUEST_STRUCT*)MemAlloc(sizeof(LIBLTE_RRC_CONNECTION_REQUEST_STRUCT));
        if (pRrcReqMsg != 0) {
            if (ASN1_SUCCES == Asn1ParseRrcConnReqMsg(pData, size, pRrcReqMsg)) {
                gLteKpi.rrcConnReq++;

                LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: RRC Conn Req (%d)\n", rnti);

                if (pRrcReqMsg->ue_id_type == LIBLTE_RRC_CON_REQ_UE_ID_TYPE_S_TMSI) {
                    LOG_INFO(ULP_LOGGER_NAME, "M-TMSI = 0x%x\n", pRrcReqMsg->ue_id.s_tmsi.m_tmsi);
                    RrcUeContext* pUeCtx = RrcGetUeContext(rnti);
                    if (pUeCtx == 0) {
                        pUeCtx = RrcCreateUeContext(rnti);
                        if (pUeCtx == 0) {
                            LOG_ERROR(ULP_LOGGER_NAME, "fail to create ue context, rnti = %d\n", rnti);
                            MemFree(pRrcReqMsg);
                            return;
                        }
                    }

                    pUeCtx = RrcUpdateMTmsi(pUeCtx, pRrcReqMsg->ue_id.s_tmsi.m_tmsi);
                    RrcUeDataInd(pUeCtx);
                } else {
                    LOG_DBG(ULP_LOGGER_NAME, "random value = %llu\n", pRrcReqMsg->ue_id.random);
                }
            } else {
                LOG_ERROR(ULP_LOGGER_NAME, "Asn1ParseRrcConnReqMsg error, rnti = %d\n", rnti);
            }

            MemFree(pRrcReqMsg);
        }
    } else {
        LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: RRC Conn Reestablish Req (%d)\n", rnti);
    }

    IP_RRC_DECODE_RESULT(rnti, rrcCcchMsgType, 0xff, 0);

    MemFree(pData);
}

// --------------------------------
void RrcDecodeIdentityResponse(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Id Resp (%d)\n", rnti);
    RrcUeContext* pUeCtx;
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
            pUeCtx = RrcUpdateImsi(pUeCtx, pIdResp->mobile_id.imsi);
            if (pUeCtx == 0) {
                MemFree(pIdResp);
                return;
            }

            // for print test
#ifdef OS_LINUX
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, pUeCtx->ueIdentity.imsi);
#else
            UInt8* imsiOctect = pIdResp->mobile_id.imsi;
            UInt32 i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
            if (pUeCtx->ueIdentity.mTmsiPresent) {
                LOG_DBG(ULP_LOGGER_NAME, "both M-MSI and IMSI are collected, rnti = %d, M-TMSI = 0x%x\n", rnti, pUeCtx->ueIdentity.mTmsi);
            } else {
                LOG_DBG(ULP_LOGGER_NAME, "Miss M-TMSI, rnti = %d\n", rnti);
            }
            
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
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Attach Req (%d)\n", rnti);
    RrcUeContext* pUeCtx;
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
            pUeCtx = RrcUpdateMTmsi(pUeCtx, pAttachReq->eps_mobile_id.guti.m_tmsi);

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pAttachReq->eps_mobile_id.guti.m_tmsi,
                pAttachReq->eps_mobile_id.guti.mcc,
                pAttachReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pAttachReq->eps_mobile_id.type_of_id) {
            pUeCtx = RrcUpdateImsi(pUeCtx, pAttachReq->eps_mobile_id.imsi);
            if (pUeCtx == 0) {
                MemFree(pAttachReq);
                return;
            }

#ifdef OS_LINUX
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, pUeCtx->ueIdentity.imsi);
#else
            UInt8* imsiOctect = pAttachReq->eps_mobile_id.imsi;
            UInt32 i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            UInt32 i;
            for (i = 0; i < 15; i++) {
                imei[i] = pAttachReq->eps_mobile_id.imei[i] + 0x30;
            }
            imei[15] = '\0';
            LOG_INFO(ULP_LOGGER_NAME, "TODO, rnti = %d, imei = %s\n", rnti, imei);
        }

        RrcUeDataInd(pUeCtx);
    } else {                        
        LOG_ERROR(ULP_LOGGER_NAME, "liblte_mme_unpack_attach_request_msg error, rnti = %d\n", rnti);
    }

    MemFree(pAttachReq);
}

// --------------------------------
void RrcDecodeDetachReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Detach Req (%d)\n", rnti);
    RrcUeContext* pUeCtx;
    LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT* pDetachReq = (LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_DETACH_REQUEST_MSG_STRUCT));
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
            pUeCtx = RrcUpdateMTmsi(pUeCtx, pDetachReq->eps_mobile_id.guti.m_tmsi);

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pDetachReq->eps_mobile_id.guti.m_tmsi,
                pDetachReq->eps_mobile_id.guti.mcc,
                pDetachReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pDetachReq->eps_mobile_id.type_of_id) {
            pUeCtx = RrcUpdateImsi(pUeCtx, pDetachReq->eps_mobile_id.imsi);
            if (pUeCtx == 0) {
                MemFree(pDetachReq);
                return;
            }

#ifdef OS_LINUX
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, pDetachReq->eps_mobile_id.imsi);
#else
            UInt8* imsiOctect = pDetachReq->eps_mobile_id.imsi;
            UInt32 i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            UInt32 i;
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
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Ext Serv Req (%d)\n", rnti);
    LOG_BUFFER(pNasMsgBuff->msg, pNasMsgBuff->N_bytes);

    RrcUeContext* pUeCtx;
    LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT* pExtServReq = (LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT*)MemAlloc(sizeof(LIBLTE_MME_EXTENDED_SERVICE_REQUEST_STRUCT));
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
            pUeCtx = RrcUpdateMTmsi(pUeCtx, pExtServReq->eps_mobile_id.m_tmsi);

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x\n", rnti, pExtServReq->eps_mobile_id.m_tmsi);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pExtServReq->eps_mobile_id.type_of_id) {
            pUeCtx = RrcUpdateImsi(pUeCtx, pExtServReq->eps_mobile_id.imsi);
            if (pUeCtx == 0) {
                MemFree(pExtServReq);
                return;
            }

#ifdef OS_LINUX
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, pExtServReq->eps_mobile_id.imsi);
#else
            UInt8* imsiOctect = pExtServReq->eps_mobile_id.imsi;
            UInt32 i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            UInt32 i;
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
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: TAU Req (%d)\n", rnti);

    RrcUeContext* pUeCtx;
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
            pUeCtx = RrcUpdateMTmsi(pUeCtx, pTauReq->eps_mobile_id.guti.m_tmsi);

            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, m_tmsi = 0x%x, mcc = %d, mnc = %d\n", rnti, 
                pTauReq->eps_mobile_id.guti.m_tmsi,
                pTauReq->eps_mobile_id.guti.mcc,
                pTauReq->eps_mobile_id.guti.mnc);
            
        } else if (LIBLTE_MME_EPS_MOBILE_ID_TYPE_IMSI == pTauReq->eps_mobile_id.type_of_id) {
            pUeCtx = RrcUpdateImsi(pUeCtx, pTauReq->eps_mobile_id.imsi);
            if (pUeCtx == 0) {
                MemFree(pTauReq);
                return;
            }

#ifdef OS_LINUX
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %s\n", rnti, pTauReq->eps_mobile_id.imsi);
#else
            UInt8* imsiOctect = pTauReq->eps_mobile_id.imsi;
            UInt32 i = 0;
            UInt32 imsi0, imsi1;
            imsi0 = (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
			imsi1 = (imsiOctect[i++] << 28) | (imsiOctect[i++] << 24) | (imsiOctect[i++] << 20) | (imsiOctect[i++] << 16) | (imsiOctect[i++] << 12) | (imsiOctect[i++] << 8) | (imsiOctect[i++] << 4) | imsiOctect[i++];
            LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, imsi = %07x%08x\n", rnti, imsi0, imsi1);
#endif
        } else {
            UInt8 imei[16];
            UInt32 i;
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
void RrcDecodeServiceReq(UInt16 rnti, LIBLTE_SIMPLE_BYTE_MSG_STRUCT* pNasMsgBuff)
{
    LOG_INFO(ULP_LOGGER_NAME, "UE ---> NB: Serv Req (%d)\n", rnti);
    LOG_BUFFER(pNasMsgBuff->msg, pNasMsgBuff->N_bytes);
    gLteKpi.servReq++;
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

        case LIBLTE_MME_SECURITY_HDR_TYPE_SERVICE_REQUEST:
        {
            RrcDecodeServiceReq(rnti, pNasMsgBuff);
            break;
        }

        default:
        {
            LOG_WARN(ULP_LOGGER_NAME, "NAS msgType = 0x%02x, rnti = %d\n", msgType, rnti);
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

    LOG_DBG(ULP_LOGGER_NAME, "imsiPresent = %d, mTmsiPresent = %d, detachFlag = %d, rnti = %d\n", 
        pRrcUeCtx->ueIdentity.imsiPresent, pRrcUeCtx->ueIdentity.mTmsiPresent, 
        pRrcUeCtx->ueIdentity.detachFlag, pRrcUeCtx->rnti);
    
    if (!IP_RRC_DATA_IND(pRrcUeCtx)) {
        return;
    }

    // if send data to OAM, delete the UE context
    // RrcDeleteUeContext(pRrcUeCtx);
}

// --------------------------------
RrcUeContext* RrcUpdateImsi(RrcUeContext* pUeCtx, UInt8* imsi)
{
    UInt32 i;
    UInt8 tmpImsi[16];
    for (i=0; i<15; i++) {
        tmpImsi[i] = imsi[i] + 0x30;
    }
    tmpImsi[15] = '\0';

    if (pUeCtx->deleteFlag) {
        LOG_WARN(ULP_LOGGER_NAME, "pUeCtx = %p is pending deletion, rnti = %d\n", pUeCtx, pUeCtx->rnti);
        pUeCtx = RrcCreateUeContext(pUeCtx->rnti);
        if (pUeCtx == 0) {             
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for new rrc ue context, rnti = %d, drop new imsi: %s\n", pUeCtx->rnti, tmpImsi);
            return 0;
        }
    }

    if (pUeCtx->ueIdentity.imsiPresent) {
        if (memcmp(pUeCtx->ueIdentity.imsi, tmpImsi, 15) == 0) {
            LOG_WARN(ULP_LOGGER_NAME, "imsi [%s] is not change, could be retransmit, rnti = %d\n", pUeCtx->ueIdentity.imsi, pUeCtx->rnti);
            return pUeCtx;
        }

        LOG_WARN(ULP_LOGGER_NAME, "old imsi = %s, rnti = %d\n", pUeCtx->ueIdentity.imsi, pUeCtx->rnti);
        RrcUeContext* pOldUeCtx = (RrcUeContext*)MemAlloc(sizeof(RrcUeContext));
        if (pOldUeCtx != 0) {
            memcpy((void*)pOldUeCtx, (void*)pUeCtx, sizeof(RrcUeContext));
            ListPushNode(&gReadyRrcUeContextList, &pOldUeCtx->node);
        } else {                
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for rrc ue context, rnti = %d, drop this imsi: %s\n", pUeCtx->rnti, pUeCtx->ueIdentity.imsi);
        }  
    }

    gLteKpi.imsi++;
    pUeCtx->ueIdentity.imsiPresent = TRUE;
    memcpy(pUeCtx->ueIdentity.imsi, tmpImsi, 16);  

    return pUeCtx;
}

// --------------------------------
RrcUeContext* RrcUpdateMTmsi(RrcUeContext* pUeCtx, UInt32 mTmsi)
{ 
    if (pUeCtx->deleteFlag) {
        LOG_WARN(ULP_LOGGER_NAME, "pUeCtx = %p is pending deletion, rnti = %d\n", pUeCtx, pUeCtx->rnti);
        pUeCtx = RrcCreateUeContext(pUeCtx->rnti);
        if (pUeCtx == 0) {             
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for new rrc ue context, rnti = %d, drop new m-tmsi: 0x%x\n", pUeCtx->rnti, mTmsi);
            return 0;
        }
    }  

    if (pUeCtx->ueIdentity.mTmsiPresent) {
        if (pUeCtx->ueIdentity.mTmsi == mTmsi) {
            LOG_WARN(ULP_LOGGER_NAME, "mTmsi [0x%x] is not change, could be retransmit, rnti = %d\n", mTmsi, pUeCtx->rnti);
            return pUeCtx;
        }

        LOG_WARN(ULP_LOGGER_NAME, "old mTmsi = 0x%x, rnti = %d\n", pUeCtx->ueIdentity.mTmsi, pUeCtx->rnti);
        RrcUeContext* pOldUeCtx = (RrcUeContext*)MemAlloc(sizeof(RrcUeContext));
        if (pOldUeCtx != 0) {
            memcpy((void*)pOldUeCtx, (void*)pUeCtx, sizeof(RrcUeContext));
            ListPushNode(&gReadyRrcUeContextList, &pOldUeCtx->node);
        } else {                
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for rrc ue context, rnti = %d, drop this mTmsi: %s\n", pUeCtx->rnti, pUeCtx->ueIdentity.mTmsi);
        }  
    }

    gLteKpi.mTmsi++;
    pUeCtx->ueIdentity.mTmsiPresent = TRUE;
    pUeCtx->ueIdentity.mTmsi = mTmsi;

    return pUeCtx;
}

