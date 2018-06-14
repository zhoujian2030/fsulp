/*
 * lteRlc.c
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#include "lteRlc.h"
#include "lteCommon.h"
#include "lteRlcMacInterface.h"
#include "ltePdcpRlcInterface.h"
#include "mempool.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"
#include "lteKpi.h"
#include "sync.h"
#include <string.h>

RxAMEntity* RlcGetRxAmEntity(RlcUeContext* pUeCtx, UInt16 lcId);
RxAMEntity* RlcCreateRxAmEntity(RlcUeContext* pUeCtx, UInt16 lcId);
void RlcDeleteRxAmEntity(RxAMEntity* pRxAmEntity);

#define RLC_GET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn) (pRxAmEntity->amdPduRing.rNodeArray[ringSn].status)
#define RLC_GET_RX_AMD_PDU(pRxAmEntity, ringSn) ((AmdPdu*)pRxAmEntity->amdPduRing.rNodeArray[ringSn].data)
#define RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn, rnStatus) {\
    pRxAmEntity->amdPduRing.rNodeArray[ringSn].status = rnStatus;}
#define RLC_SET_RX_AMD_PDU(pRxAmEntity, ringSn, pAmdPdu) {\
    pRxAmEntity->amdPduRing.rNodeArray[ringSn].data = (void*)pAmdPdu;}

AmdPdu* RlcCreateAmdPdu(RxAMEntity* pRxAmEntity, UInt16 ringSn);
void RlcDeleteAmdPdu(AmdPdu* pAmdPdu);

void RlcDeleteAmdPduSegment(AmdPduSegment* pAmdPduSeg);

BOOL isSNEqual(UInt16 x, UInt16 y);
void RlcProcessAMRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti);
void RlcProcessUmOrTmRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti);
BOOL RlcDecodeAmdHeader(RlcUlDataInfo* pRlcDataInfo, AmdHeader* pAmdHeader);
BOOL RlcDecodeAmdPdu(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo);
BOOL RlcProcessAmdPduSegment(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo);
void RlcSetAmdPduAndDfeStatus(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, AmdDFE* pAmdDfe);
void RlcReassembleRlcSdu(UInt16 sn, RxAMEntity* pRxAmEntity);
void RlcReassembleAmdDfeQ(UInt16 sn, RxAMEntity* pRxAmEntity, AmdPduSegment* pAmdPduSeg);
void RlcReassembleInCmpAMSdu(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe);
void RlcReassembleFirstSduSegment(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe);
void RlcDeliverAmSduToPdcp(RxAMEntity* pRxAmEntity, RlcAmBuffer* pAmBuffer);
unsigned int RlcGetConcatenationHeaderInfo(RlcUlDataInfo* pRlcDataInfo, RlcConcatenatePduHeader *pPduHeader);
unsigned int RlcDecodeAndEnqueueAMSegment(List *pDfeQ, AmdDFE *pAmdDfe, RlcConcatenatePduHeader *pPduHeader, unsigned char fi);

#ifndef OS_LINUX
#pragma DATA_SECTION(gRlcUeContextList, ".ulpdata");
#endif
List gRlcUeContextList;

// -----------------------------------
void InitRlcLayer()
{
    ListInit(&gRlcUeContextList, TRUE);
}

// -----------------------------------
RlcUeContext* RlcGetUeContext(unsigned short rnti)
{
    RlcUeContext* pUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while (pUeCtx != 0) {
        if (pUeCtx->rnti == rnti) {
            return pUeCtx;
        } else {
            pUeCtx = (RlcUeContext*)ListGetNextNode(&pUeCtx->node);
        }
    }

    LOG_TRACE(ULP_LOGGER_NAME, "UE context NOT exists, rnti = %d\n", rnti);
    return 0;
}

// -----------------------------------
RlcUeContext* RlcCreateUeContext(unsigned short rnti)
{
    RlcUeContext* pUeCtx = (RlcUeContext*)MemAlloc(sizeof(RlcUeContext));
    if (pUeCtx == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for rlc ue context\n");
        return 0;
    }
    LOG_INFO(ULP_LOGGER_NAME, "pUeCtx = %p, rnti = %d\n", pUeCtx, rnti);
    memset(pUeCtx, 0, sizeof(RlcUeContext));
    pUeCtx->rnti = rnti; 
    SemInit(&pUeCtx->lockOfCount, 1);
    pUeCtx->idleCount = 0;
    ListPushNode(&gRlcUeContextList, &pUeCtx->node);
    KpiCountRlcUeCtx(TRUE);

    return pUeCtx;
}

// -----------------------------------
void RlcDeleteUeContext(RlcUeContext* pRlcUeCtx)
{
    if (pRlcUeCtx != 0) {
        LOG_INFO(ULP_LOGGER_NAME, "pRlcUeCtx = %p, rnti = %d\n", pRlcUeCtx, pRlcUeCtx->rnti);
        KpiCountRlcUeCtx(FALSE);
        unsigned int i;
        for (i=0; i<MAX_LC_ID; i++) {
            RlcDeleteRxAmEntity(pRlcUeCtx->rxAMEntityArray[i]);
            pRlcUeCtx->rxAMEntityArray[i] = 0;
        }
        SemDestroy(&pRlcUeCtx->lockOfCount);
        ListDeleteNode(&gRlcUeContextList, &pRlcUeCtx->node);
        MemFree(pRlcUeCtx);
    }
}

// -----------------------------------
int RlcGetUeContextCount()
{
    return ListCount(&gRlcUeContextList);
}

// -----------------------------------
void RlcUpdateUeContextTime(RlcUeContext* pRlcUeCtx, unsigned int value)
{
    if (pRlcUeCtx != 0) {
        SemWait(&pRlcUeCtx->lockOfCount);
        if (value == 0) {
            pRlcUeCtx->idleCount = 0;
        } else {
            pRlcUeCtx->idleCount += value;
        }
        SemPost(&pRlcUeCtx->lockOfCount);
    }
}

// -----------------------------------
RxAMEntity* RlcGetRxAmEntity(RlcUeContext* pUeCtx, UInt16 lcId) {
    if (pUeCtx != 0 && lcId <MAX_LC_ID) {
        RlcUpdateUeContextTime(pUeCtx, 0);
        return pUeCtx->rxAMEntityArray[lcId];
    }

    return 0;
}

// -----------------------------------
RxAMEntity* RlcCreateRxAmEntity(RlcUeContext* pUeCtx, UInt16 lcId) {
    RxAMEntity* pRxAmEntity = 0;
    if (pUeCtx != 0 && lcId <MAX_LC_ID) {
        pRxAmEntity = (RxAMEntity*)MemAlloc(sizeof(RxAMEntity));
        if (pRxAmEntity == 0) {            
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for RxAMEntity, rnti = %d, lcId = %d\n", 
                pUeCtx->rnti, lcId);
            return 0;
        }
        LOG_INFO(ULP_LOGGER_NAME, "pRxAmEntity = %p, rnti = %d, lcId = %d\n", 
            pRxAmEntity, pUeCtx->rnti, lcId);
        memset((void*)pRxAmEntity, 0, sizeof(RxAMEntity));
        pUeCtx->rxAMEntityArray[lcId] = pRxAmEntity;
        pRxAmEntity->rnti = pUeCtx->rnti;
        pRxAmEntity->lcId = lcId;
        pRxAmEntity->amdPduRing.size = 512;
        UInt16 i;
        for (i=0; i<pRxAmEntity->amdPduRing.size; i++) {
            RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, i, RS_FREE);
            RLC_SET_RX_AMD_PDU(pRxAmEntity, i, 0);
        }

        RlcUpdateUeContextTime(pUeCtx, 0);
    }

    return pRxAmEntity;
}

// -----------------------------------
void RlcDeleteRxAmEntity(RxAMEntity* pRxAmEntity) {
    if (pRxAmEntity != 0) {
        UInt16 i;  
        LOG_INFO(ULP_LOGGER_NAME, "pRxAmEntity = %p, rnti = %d, lcId = %d\n", 
            pRxAmEntity, pRxAmEntity->rnti, pRxAmEntity->lcId);
        for(i=0; i<pRxAmEntity->amdPduRing.size; i++) {
            if (RLC_GET_RX_AMD_PDU_STATUS(pRxAmEntity, i) != RS_FREE) {              
                RlcDeleteAmdPdu(RLC_GET_RX_AMD_PDU(pRxAmEntity, i));
            }
        }

        if (pRxAmEntity->rxRawSdu.rawSdu.pData != 0) {
            LOG_DBG(ULP_LOGGER_NAME, "pRxAmEntity->rxRawSdu.rawSdu.pData = %p\n", 
                pRxAmEntity->rxRawSdu.rawSdu.pData);
            MemFree(pRxAmEntity->rxRawSdu.rawSdu.pData);
        }
        MemFree(pRxAmEntity);
    }
}

// -----------------------------------
AmdPdu* RlcCreateAmdPdu(RxAMEntity* pRxAmEntity, UInt16 ringSn)
{
    if (pRxAmEntity == 0) {        
        LOG_ERROR(ULP_LOGGER_NAME, " pRxAmEntity is NULL, ringSn = %d\n", ringSn);
        return 0;
    }

    AmdPdu* pAmdPdu = (AmdPdu*)MemAlloc(sizeof(AmdPdu));
    if (pAmdPdu == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for AM PDU, rnti = %d, ringSn = %d\n", pRxAmEntity->rnti, ringSn);
        return 0;
    } 
    LOG_DBG(ULP_LOGGER_NAME, "create AM PDU, rnti = %d, ringSn = %d\n", pRxAmEntity->rnti, ringSn);
    ListInit(&pAmdPdu->segList, 0);
    return pAmdPdu;
}

// -----------------------------------
void RlcDeleteAmdPdu(AmdPdu* pAmdPdu)
{
    if (pAmdPdu != 0) {
        LOG_DBG(ULP_LOGGER_NAME, "pAmdPdu = %p\n", pAmdPdu);
        if (ListCount(&pAmdPdu->segList) > 0) {
            AmdPduSegment* pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
            while (pAmdPduSeg != 0) {
                RlcDeleteAmdPduSegment(pAmdPduSeg);
                pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
            }
        }

        ListDeInit(&pAmdPdu->segList);
        MemFree(pAmdPdu);
    }
}

// ----------------------------------
void RlcDeleteAmdPduSegment(AmdPduSegment* pAmdPduSeg)
{
    if (pAmdPduSeg != 0) {
        if (ListCount(&pAmdPduSeg->dfeQ) > 0) {
            LOG_DBG(ULP_LOGGER_NAME, "pAmdPduSeg = %p\n", pAmdPduSeg);
            AmdDFE* pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
            while (pAmdDfe != 0) {
                LOG_DBG(ULP_LOGGER_NAME, "pAmdDfe = %p, pAmdDfe->buffer.pData = %p\n", 
                    pAmdDfe, pAmdDfe->buffer.pData);
                if (pAmdDfe->buffer.pData != 0) {
                    MemFree(pAmdDfe->buffer.pData);
                }
                MemFree((void*)pAmdDfe);
                pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
            }
        }

        ListDeInit(&pAmdPduSeg->dfeQ);
        MemFree(pAmdPduSeg);
    }
}

// -----------------------------------
void MacUeDataInd(MacUeDataInd_t* pMacDataInd) 
{
    if (pMacDataInd == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer pMacDataInd\n");
        return;
    }

    if (pMacDataInd->rlcData == 0) {
        // LOG_ERROR(ULP_LOGGER_NAME, "NULL pointer pMacDataInd->rlcData\n");
        MemFree(pMacDataInd);
        return;
    }

    RlcUlData* pRlcData = pMacDataInd->rlcData;
    RlcUlDataInfo* pRlcDataInfo = 0;
    UInt32 i;

    LOG_TRACE(ULP_LOGGER_NAME, "rnti = %d, numLCInfo = %d\n", pMacDataInd->rnti, pRlcData->numLCInfo);
    for (i=0; i<pRlcData->numLCInfo; i++) {
        pRlcDataInfo = &pRlcData->rlcDataArray[i];
        LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, length = %d\n", pMacDataInd->rnti, pRlcDataInfo->lcId, pRlcDataInfo->length);
        LOG_BUFFER(pRlcDataInfo->rlcdataBuffer, pRlcDataInfo->length);

        if (pRlcDataInfo->lcId <= SRB_2_LCID) {
            RlcProcessAMRxPacket(pRlcDataInfo, pMacDataInd->rnti);            
        } else {
            RlcProcessUmOrTmRxPacket(pRlcDataInfo, pMacDataInd->rnti);
        }        
    }

    MemFree(pRlcData);
    MemFree(pMacDataInd);
}

// ---------------------------------
BOOL isSNEqual(UInt16 x, UInt16 y)
{
    return ((x & 1023) == (y & 1023));
}

// ---------------------------------
void RlcProcessUmOrTmRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti)
{
    if (pRlcDataInfo->lcId <= SRB_2_LCID) {
        LOG_WARN(ULP_LOGGER_NAME, "unsupport lcId = %d, rnti = %d\n", pRlcDataInfo->lcId, rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    LOG_TRACE(ULP_LOGGER_NAME, "TODO, DRB data, lcId = %d, rnti = %d\n", pRlcDataInfo->lcId, rnti);

    //TODO
    MemFree(pRlcDataInfo->rlcdataBuffer);
}

// ---------------------------------
void RlcProcessAMRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti)
{
    BOOL ret = RLC_FAILURE;
    AmdHeader amdHeader = {0};
    RlcUeContext* pUeCtx = 0;
    RxAMEntity* pRxAmEntity = 0;
    UInt16 ringSn = 0;
    AmdPdu* pAmdPdu = 0;

    if (pRlcDataInfo->lcId > SRB_2_LCID) {
        LOG_WARN(ULP_LOGGER_NAME, "unsupport lcId = %d, rnti = %d\n", pRlcDataInfo->lcId, rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    ret = RlcDecodeAmdHeader(pRlcDataInfo, &amdHeader);

    if (ret == RLC_FAILURE) {
        LOG_ERROR(ULP_LOGGER_NAME, "fail to decode AMD, rnti = %d\n", rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    if (!amdHeader.dc) {
        LOG_DBG(ULP_LOGGER_NAME, "ignore RLC status PDU, rnti = %d\n", rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    // create UE context if not exists
    pUeCtx = RlcGetUeContext(rnti);
    if (pUeCtx == 0) {
        pUeCtx = RlcCreateUeContext(rnti);
        if (pUeCtx == 0) {      
            MemFree(pRlcDataInfo->rlcdataBuffer);
            return;
        } else {
            LOG_DBG(ULP_LOGGER_NAME, "create rlc ue context, rnti = %d, pUeCtx = %p\n", rnti, pUeCtx);
        }
    }

    // create RX AM entity if not exists
    pRxAmEntity = RlcGetRxAmEntity(pUeCtx, pRlcDataInfo->lcId);
    if (pRxAmEntity == 0) {        
        LOG_DBG(ULP_LOGGER_NAME, "create rlc AM entity, rnti = %d, lcId = %d\n", rnti, pRlcDataInfo->lcId);
        pRxAmEntity = RlcCreateRxAmEntity(pUeCtx, pRlcDataInfo->lcId);
        if (pRxAmEntity == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for RxAMEntity, rnti = %d, lcId = %d\n", rnti, pRlcDataInfo->lcId);
            MemFree(pRlcDataInfo->rlcdataBuffer);
            return;
        }
    }

    // check if receive segment before
    ringSn = amdHeader.sn & 511;
    pAmdPdu = RLC_GET_RX_AMD_PDU(pRxAmEntity, ringSn); 
    
    // new seq num 
    if (RLC_GET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn) == RS_FREE) 
    {
        if (pAmdPdu == 0) {
            pAmdPdu = RlcCreateAmdPdu(pRxAmEntity, ringSn);
            if (pAmdPdu == 0) {
                MemFree(pRlcDataInfo->rlcdataBuffer);
                return;
            }
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "TODO, should not happen, rnti = %d, pAmdPdu = %p\n", rnti, pAmdPdu);

            RlcDeleteAmdPdu(pAmdPdu);
            RLC_SET_RX_AMD_PDU(pRxAmEntity, ringSn, 0);
            pAmdPdu = RlcCreateAmdPdu(pRxAmEntity, ringSn);
            if (pAmdPdu == 0) {
                MemFree(pRlcDataInfo->rlcdataBuffer);
                return;
            }
        }

        if (amdHeader.rf) {
            // handle AMD PDU segment
            LOG_INFO(ULP_LOGGER_NAME, "TODO, handle first AMD PDU segment, rnti = %d\n", rnti);
            ret = RlcProcessAmdPduSegment(pAmdPdu, &amdHeader, pRlcDataInfo);
            // TODO
        } else {
            ret = RlcDecodeAmdPdu(pAmdPdu, &amdHeader, pRlcDataInfo);
            if (ret == RLC_SUCCESS) {
                pAmdPdu->status = PDU_AM_COMPLETE;
            }          
        }

        if (ret == RLC_SUCCESS) {
            RLC_SET_RX_AMD_PDU(pRxAmEntity, ringSn, pAmdPdu);
            RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn, RS_IN_USE);
        } else {
            RlcDeleteAmdPdu(pAmdPdu);
        }
    } 
    // new PDU segment
    else 
    {
        if (pAmdPdu == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "pAmdPdu is null, could not happen! rnti = %d\n", rnti);
            // any resource to free?
            MemFree(pRlcDataInfo->rlcdataBuffer);
            return;
        }

        LOG_TRACE(ULP_LOGGER_NAME, "receive rlc segment, rnti = %d\n", rnti);

        if (amdHeader.rf) {
            // handle AMD segment
            LOG_INFO(ULP_LOGGER_NAME, "TODO handle AMD segment, rnti = %d\n", rnti);
            ret = RlcProcessAmdPduSegment(pAmdPdu, &amdHeader, pRlcDataInfo);
            // TODO
        } else {
            LOG_INFO(ULP_LOGGER_NAME, "TODO handle last AMD PDU segment, rnti = %d\n", rnti);
            pAmdPdu->status = PDU_AM_COMPLETE;            
        }

        // todo
        MemFree(pRlcDataInfo->rlcdataBuffer); // temp solution
    }

    if (ret == RLC_SUCCESS) {
        RlcReassembleRlcSdu(amdHeader.sn, pRxAmEntity);
    }
}
 
// ---------------------------------
BOOL RlcDecodeAmdHeader(RlcUlDataInfo* pRlcDataInfo, AmdHeader* pAmdHeader)
{
    if (pRlcDataInfo->rlcdataBuffer == 0 || pRlcDataInfo->length == 0) {
        return FALSE;
    }

    if (pRlcDataInfo->length < 2) {
        LOG_WARN(ULP_LOGGER_NAME, "invalid length = %d\n", pRlcDataInfo->length);
        return FALSE;
    }

    UInt32 i = 0;
    UInt8* data = pRlcDataInfo->rlcdataBuffer;

    pAmdHeader->dc = ( data[i] >> 7) & 0x1;

    if (pAmdHeader->dc) {
        pAmdHeader->rf =  (data[i] >> 6 ) & 0x01U;
        pAmdHeader->p  =  (data[i] >> 5 ) & 0x01U;
        pAmdHeader->fi =  (data[i] >> 3 ) & 0x03U;
        pAmdHeader->e  =  (data[i] >> 2 ) & 0x01U;
        pAmdHeader->sn = ( ( (data[i] & 0x03U) << 8 )| (data[i + 1] & 0xFFU) );

        LOG_TRACE(ULP_LOGGER_NAME, "p = %d, sn = %d, e = %d, fi = %d\n",
            pAmdHeader->p, pAmdHeader->sn, pAmdHeader->e, pAmdHeader->fi);

        /*Decode AMD PDU Segment*/    
        if ( pAmdHeader->rf ) {
            if (pRlcDataInfo->length < 4) {
                LOG_WARN(ULP_LOGGER_NAME, "invalid length = %d\n", pRlcDataInfo->length);
                return FALSE;
            }
            /* Get Last Segment Flag (LSF) field */
            pAmdHeader->lsf =  (data[i + 2] >> 7 ) & 0x01U;
            /* Get Segment Offset (SO) field */
            pAmdHeader->so  = ( ((UInt16)(data[i + 2] & 0x7FU ) << 8) | (data[i + 3] & 0xFFU )); 
            LOG_TRACE(ULP_LOGGER_NAME, "lsf = %d, so = %d\n", pAmdHeader->lsf, pAmdHeader->so);
        }
    } else {
        UInt8 cpt = (data[i] >> 4) & 0x07;
        UInt16 ackSn = 0;
    	if (cpt == 0) {
    		ackSn = ((data[i] & 0x0f) << 6) | ((data[i+1] >> 2) &0x3f);
        	LOG_TRACE(ULP_LOGGER_NAME, "ackSn = %d\n", ackSn);
    	}
    }

    return TRUE;
}

// ---------------------------------
BOOL RlcDecodeAmdPdu(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo)
{
    LOG_INFO(ULP_LOGGER_NAME, "sn = %d, fi = %d, e = %d, length = %d\n", 
        pAmdHeader->sn, pAmdHeader->fi, pAmdHeader->e, pRlcDataInfo->length);

    BOOL ret = RLC_SUCCESS;
    AmdDFE *pDfe = 0 ;
    unsigned int size = pRlcDataInfo->length - 2; // 2 bytes header
    unsigned int segCount = ListCount(&pAmdPdu->segList);

    if (segCount) {
        LOG_ERROR(ULP_LOGGER_NAME, "segCount = %d\n", segCount);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return RLC_FAILURE;
    }    

    // init pdu segment
    AmdPduSegment* pAmdPduSegment = (AmdPduSegment*)MemAlloc(sizeof(AmdPduSegment));
    if (pAmdPduSegment == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "Fail to create AmdPduSegment\n");
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return RLC_FAILURE;
    }
    memset(pAmdPduSegment, 0, sizeof(AmdPduSegment));
    ListInit(&pAmdPduSegment->dfeQ, 0);
    pAmdPduSegment->soStart = 0;
    pAmdPduSegment->soEnd = 0xFFFF;

    if (pAmdHeader->e == 0) {
        pDfe = (AmdDFE*)MemAlloc(sizeof(AmdDFE));
        if (pDfe == 0) {            
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for AmdDFE\n");
            MemFree(pRlcDataInfo->rlcdataBuffer);
            RlcDeleteAmdPduSegment(pAmdPduSegment);
            return RLC_FAILURE;
        }
        
        memset(pDfe, 0, sizeof(AmdDFE));
        pDfe->status = AM_PDU_MAP_SDU_FULL;
        pDfe->buffer.size = size;
        pDfe->buffer.pData = pRlcDataInfo->rlcdataBuffer;
        // memmove(pDfe->buffer.pData, (pDfe->buffer.pData+2), pDfe->buffer.size);
        MemRemove(pDfe->buffer.pData, 0, 2);
        LOG_BUFFER(pDfe->buffer.pData, pDfe->buffer.size);
        RlcSetAmdPduAndDfeStatus(pAmdPdu, pAmdHeader, pDfe);
        ListPushNode(&pAmdPduSegment->dfeQ, &pDfe->node);
    } else {
        // Concatenation
        LOG_DBG(ULP_LOGGER_NAME, "handle Concatenation\n");

        RlcConcatenatePduHeader pduHeader;        
        unsigned int dfeCount = RlcGetConcatenationHeaderInfo(pRlcDataInfo, &pduHeader);

        if (dfeCount == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "RlcGetConcatenationHeaderInfo error\n");
            MemFree(pRlcDataInfo->rlcdataBuffer);
            RlcDeleteAmdPduSegment(pAmdPduSegment);
            return RLC_FAILURE;
        }

        pDfe = (AmdDFE*)MemAlloc(sizeof(AmdDFE));
        if (pDfe == 0) {            
            LOG_ERROR(ULP_LOGGER_NAME, "2 fail to allocate memory for AmdDFE\n");
            MemFree(pRlcDataInfo->rlcdataBuffer);
            RlcDeleteAmdPduSegment(pAmdPduSegment);
            return RLC_FAILURE;
        }

        pDfe->buffer.size = pRlcDataInfo->length - pduHeader.hdrSize;
        pDfe->buffer.pData = pRlcDataInfo->rlcdataBuffer;
        MemRemove(pDfe->buffer.pData, 0, pduHeader.hdrSize);

        if(pAmdHeader->fi & 0x02) {
            pDfe->status = AM_PDU_MAP_SDU_END;
        } else {
            pDfe->status = AM_PDU_MAP_SDU_FULL;
        }
        ret = RlcDecodeAndEnqueueAMSegment(&pAmdPduSegment->dfeQ, pDfe, &pduHeader, pAmdHeader->fi);
        if (ret == RLC_FAILURE) {
            LOG_ERROR(ULP_LOGGER_NAME, "RlcDecodeAndEnqueueAMSegment error\n");
            RlcDeleteAmdPduSegment(pAmdPduSegment);
            return RLC_FAILURE;
        }
    }

    if (ret == RLC_SUCCESS) {
        ListPushNode(&pAmdPdu->segList, &pAmdPduSegment->node);
    }

    return ret;
}

#define MULTIPLY_BY_12(x) ( (x << 3) + (x << 2) )
#define DIVIDE_BY_8(x) (x >> 3)
#define MOD_BY_8(x) (x & 0x7)
#define GET_E_BIT( eliStart, eliOffset) ((eliStart[0] >> ( 7 - eliOffset )) & 0x01 )
#define GET_LI( eliStart, eliOffset ) ( (( eliStart[0] << ( 4 + eliOffset )) | (eliStart[1] >> ( 4 - eliOffset) )) & 0x7FF )
#define GET_VAR_HEADER_SIZE(coutSDU) (( ((coutSDU -1) << 3) + ((coutSDU -1) << 2) + 7 ) >> 3 )
// ---------------------------------
unsigned int RlcGetConcatenationHeaderInfo(RlcUlDataInfo* pRlcDataInfo, RlcConcatenatePduHeader *pPduHeader)
{
    if (pRlcDataInfo == 0 || pRlcDataInfo->rlcdataBuffer == 0 || pPduHeader == 0) {
        return 0;
    }

    UInt8 ext       = 1;
    UInt8 byteNum   = 0;
    UInt32 dfeCount = 1; /* Fixed Header*/
    UInt32 li       = 0;
    UInt8 *eliStart = 0;
    /* Value should be 0 and 4 as corressponding to E bit position*/
    UInt8 eliOffset = 0;

    UInt8 *baseStartWithVH = &pRlcDataInfo->rlcdataBuffer[2]; // the first bytes are RLC header

    pPduHeader->numDfe = 0;

    do  
    { /*  
          elifOffset = 0   elifOffset = 4 where E bit exist.

          0   1  2   3   4   5  6  7
       *****************************
       E              L               eliStart = 0
       *****************************
       L           E     L         eliStart = 1
       *****************************
       L
       *****************************
       E              L               eliStart = 3
       *****************************
       */
        /* Get the byte number where E bit exist 
           12 = E ( 1 bit) + L ( 11 bit ) 
           byteNum = 
           8 = 1 bytes
         */
        eliStart  = & (baseStartWithVH[ DIVIDE_BY_8( (MULTIPLY_BY_12( byteNum)) )]);
        /* Get the exact position of E in term of bits. i.e 0 or 4 */
        eliOffset = MOD_BY_8( (MULTIPLY_BY_12( ( byteNum) )) ) /* Mod 8*/;
        /* Get the value of extension i.e 0 or 1 */
        ext = GET_E_BIT( eliStart, eliOffset);
        /* Get the value of Length of the sdu*/
        li  = GET_LI( eliStart, eliOffset);
        /* li = 0 is reserved and if this value is received the PDU should be discard */
        if (0 == li) {
            LOG_ERROR(ULP_LOGGER_NAME, "li is 0\n");
            return 0;
        }

        LOG_TRACE(ULP_LOGGER_NAME, "dfeCount = %d, li = %d, eliOffset = %d, ext = %d, numDfe = %d\n", dfeCount, li, eliOffset, ext, pPduHeader->numDfe);

        pPduHeader->dfeLength[pPduHeader->numDfe] = li;
        pPduHeader->numDfe++;
        dfeCount++;
        byteNum++;


        if (pPduHeader->numDfe >= RLC_MAX_DFE_IN_UL_PDU) {
            LOG_ERROR(ULP_LOGGER_NAME, "pPduHeader->numDfe = %d\n", pPduHeader->numDfe);
            break;
        }

    }while ( 0 != ext );

    pPduHeader->hdrSize = (GET_VAR_HEADER_SIZE( pPduHeader->numDfe + 1) ) + 2;
    LOG_TRACE(ULP_LOGGER_NAME, "dfeCount = %d, hdrSize = %d\n", dfeCount, pPduHeader->hdrSize);

    return dfeCount;
}

// ---------------------------------
unsigned int RlcDecodeAndEnqueueAMSegment(
    List *pDfeQ, 
    AmdDFE *pAmdDfe, 
    RlcConcatenatePduHeader *pPduHeader, 
    unsigned char fi)
{
    UInt32 li          = 0;
    AmdDFE *pNextDfe   = 0;
    UInt32 i           = 0;

    do 
    {
        li =  pPduHeader->dfeLength[i];

        if ( li > pAmdDfe->buffer.size )
        {
       	    LOG_ERROR(ULP_LOGGER_NAME, "[%s], li = %d, pAmdDfe->buffer.size\n", li, pAmdDfe->buffer.size);
            MemFree(pAmdDfe->buffer.pData);
            MemFree(pAmdDfe);   
            return RLC_FAILURE;
        }

        pNextDfe = (AmdDFE*)MemAlloc(sizeof(AmdDFE));
        if (pNextDfe == 0) {            
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for AmdDFE\n");
            MemFree(pAmdDfe->buffer.pData);
            MemFree(pAmdDfe);    
            return RLC_FAILURE;
        }

        pNextDfe->buffer.pData = MemAlloc(pAmdDfe->buffer.size - li);
        if(pNextDfe->buffer.pData == 0) {
        	LOG_ERROR(ULP_LOGGER_NAME, "fail to alloc mem for AmdDFE buffer\n");
            MemFree(pAmdDfe->buffer.pData);
            MemFree(pNextDfe);
            MemFree(pAmdDfe);     
            return RLC_FAILURE;
        }

        memcpy(pNextDfe->buffer.pData, pAmdDfe->buffer.pData + li, pAmdDfe->buffer.size - li);  
        pNextDfe->buffer.size = pAmdDfe->buffer.size - li;

        pAmdDfe->buffer.size = li;
        ListPushNode(pDfeQ, &pAmdDfe->node);
        LOG_TRACE(ULP_LOGGER_NAME, "li = %d, data: \n", li);
        LOG_BUFFER(pAmdDfe->buffer.pData, pAmdDfe->buffer.size);

        pAmdDfe = pNextDfe;
        pAmdDfe->status = AM_PDU_MAP_SDU_FULL;
        
        i++;        
    } while ( i < pPduHeader->numDfe );

    pAmdDfe->status = (RlcSduStatus)(fi & 0x01);
    LOG_TRACE(ULP_LOGGER_NAME, "last li = %d, status = %d, data: \n", pAmdDfe->buffer.size, pAmdDfe->status);
    LOG_BUFFER(pAmdDfe->buffer.pData, pAmdDfe->buffer.size);
    /*enqueue last dfe into DFE queue */
    ListPushNode(pDfeQ, &pAmdDfe->node);

    return RLC_SUCCESS ;
}

// ---------------------------------
BOOL RlcProcessAmdPduSegment(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo)
{
    BOOL ret = RLC_FAILURE;

    // TODO

    MemFree(pRlcDataInfo->rlcdataBuffer);

    return ret;
}

// ---------------------------------
void RlcSetAmdPduAndDfeStatus(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, AmdDFE* pAmdDfe)
{
    if ( pAmdHeader->rf == 0x01 &&  pAmdHeader->lsf == 0x00) {
         pAmdPdu->status = PDU_AM_SEGMENT; 
    }
    else if ( pAmdHeader->rf == 0x01 && pAmdHeader->lsf == 0x01) {
         pAmdPdu->status = PDU_AM_SEGMENT_COMPLETE;
    }

    switch ( pAmdHeader->fi)
    {
         case 0x01:
            pAmdDfe->status = AM_PDU_MAP_SDU_START ;
             break;

         case 0x03:
            pAmdDfe->status = AM_PDU_MAP_SDU_MID ;
             break;

         case 0x02:
            pAmdDfe->status = AM_PDU_MAP_SDU_END ;
            break;

         default:
            break;
    }
}

// --------------------------------
void RlcReassembleRlcSdu(UInt16 sn, RxAMEntity* pRxAmEntity)
{
    LOG_DBG(ULP_LOGGER_NAME, "sn = %d, rnti = %d, lcId = %d\n", sn, pRxAmEntity->rnti, pRxAmEntity->lcId);

    UInt16 ringSn = sn & 511;
    RingNodeStatus rnStatus = RLC_GET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn);
    AmdPdu* pAmdPdu = 0;
    AmdPduSegment* pAmdPduSeg = 0;
    
    if (rnStatus == RS_IN_USE) {
        pAmdPdu = RLC_GET_RX_AMD_PDU(pRxAmEntity, ringSn);
        if (pAmdPdu == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "pAmdPdu is null, sn = %d, rnti = %d, lcId = %d\n", sn, pRxAmEntity->rnti, pRxAmEntity->lcId);
            RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn, RS_FREE);
        } else {
            if ((pAmdPdu->status == PDU_AM_COMPLETE) || (pAmdPdu->status == PDU_AM_SEGMENT_COMPLETE)) {
                RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn, RS_READY);

                pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
                while (pAmdPduSeg != 0) {
                    RlcReassembleAmdDfeQ(sn, pRxAmEntity, pAmdPduSeg);
                    ListDeInit(&pAmdPduSeg->dfeQ);
                    MemFree(pAmdPduSeg);
                    pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
                }

                ListDeInit(&pAmdPdu->segList);
                MemFree(pAmdPdu);
                RLC_SET_RX_AMD_PDU(pRxAmEntity, ringSn, 0);
                RLC_SET_RX_AMD_PDU_STATUS(pRxAmEntity, ringSn, RS_FREE);
            } else {
                LOG_TRACE(ULP_LOGGER_NAME, "sn = %d, rnti = %d, lcId = %d, pAmdPdu->status = %d\n",
                    sn, pRxAmEntity->rnti, pRxAmEntity->lcId, pAmdPdu->status);
            }
        }
    } else {
        LOG_TRACE(ULP_LOGGER_NAME, "sn = %d, rnti = %d, lcId = %d, ring node status = %d\n", 
            sn, pRxAmEntity->rnti, pRxAmEntity->lcId, rnStatus);
    }
}

// --------------------------------
void RlcReassembleAmdDfeQ(UInt16 sn, RxAMEntity* pRxAmEntity, AmdPduSegment* pAmdPduSeg)
{
    LOG_INFO(ULP_LOGGER_NAME, "sn = %d, rnti = %d, pAmdPduSeg = %p\n", sn, pRxAmEntity->rnti, pAmdPduSeg);

    RlcAmRawSdu* pRawSdu = &pRxAmEntity->rxRawSdu;

    AmdDFE* pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
    while (pAmdDfe != 0) {
        if (pAmdDfe->buffer.pData != 0) {
            if (pRawSdu->rawSdu.pData != 0) {
                RlcReassembleInCmpAMSdu(sn, pRxAmEntity, pRawSdu, pAmdDfe);
            } else {
                RlcReassembleFirstSduSegment(sn, pRxAmEntity, pRawSdu, pAmdDfe);
            }
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "pAmdDfe->buffer.pData is NULL\n");
        }
        MemFree((void*)pAmdDfe);
        pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
    } 
}

// --------------------------------
void RlcReassembleInCmpAMSdu(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe)
{
    LOG_TRACE(ULP_LOGGER_NAME, "sn = %d, rnti = %d, pAmdDfe->status = %d\n", sn, pRxAmEntity->rnti, pAmdDfe->status);

    RlcAmBuffer *pPrevBuffer = &(pRawSdu->rawSdu);
    RlcAmBuffer *pCurrBuffer = &(pAmdDfe->buffer);
    UInt8* pTmpBuffer = 0;

    if (isSNEqual(sn, pRawSdu->sn + 1) || isSNEqual(sn, pRawSdu->sn)) {
        switch(pAmdDfe->status) {
            case AM_PDU_MAP_SDU_END:
            {
                LOG_DBG(ULP_LOGGER_NAME, "receive last SDU segment, rnti = %d, sn = %d\n", pRxAmEntity->rnti, sn);

                pTmpBuffer = MemJoin(pPrevBuffer->pData, pCurrBuffer->pData);
                if (pTmpBuffer == 0) {
                    LOG_ERROR(ULP_LOGGER_NAME, "MemJoin failure, pPrevBuffer->pData = %p, pCurrBuffer->pData = %p\n", 
                        pPrevBuffer->pData, pCurrBuffer->pData);
                    return;
                }
                pPrevBuffer->size += pCurrBuffer->size;
                pPrevBuffer->pData = pTmpBuffer;

                RlcDeliverAmSduToPdcp(pRxAmEntity, pPrevBuffer);

                pPrevBuffer->pData = 0;
                pPrevBuffer->size = 0;

                break;
            }

            case AM_PDU_MAP_SDU_MID:
            {                
                LOG_DBG(ULP_LOGGER_NAME, "receive middle SDU segment, rnti = %d, sn = %d\n", pRxAmEntity->rnti, sn);

                if (!isSNEqual(sn, pRawSdu->sn)) {
                    pTmpBuffer = MemJoin(pPrevBuffer->pData, pCurrBuffer->pData);
                    if (pTmpBuffer == 0) {
                        LOG_ERROR(ULP_LOGGER_NAME, "AM_PDU_MAP_SDU_MID MemJoin failure, pPrevBuffer->pData = %p, pCurrBuffer->pData = %p\n", 
                            pPrevBuffer->pData, pCurrBuffer->pData);
                        return;
                    }
                    pPrevBuffer->size += pCurrBuffer->size;
                    pPrevBuffer->pData = pTmpBuffer;
                    pRawSdu->sn = sn;
                } else {
                    LOG_WARN(ULP_LOGGER_NAME, "sn is the same with previous sn, might be retransmit segment, drop it, sn = %d, pRawSdu->sn = %d, rnti = %d, status = %d\n",
                        sn, pRawSdu->sn, pRxAmEntity->rnti, pAmdDfe->status);
                    MemFree(pAmdDfe->buffer.pData);
                }               

                break;
            }

            case AM_PDU_MAP_SDU_FULL:
            {
                LOG_WARN(ULP_LOGGER_NAME, "receive single SDU, discard previous segments, rnti = %d, sn = %d\n", pRxAmEntity->rnti, sn);
                MemFree(pPrevBuffer->pData);
                pPrevBuffer->pData = 0;
                pPrevBuffer->size = 0;

                RlcDeliverAmSduToPdcp(pRxAmEntity, pPrevBuffer);

                break;
            }

            default:
            {
                LOG_WARN(ULP_LOGGER_NAME, "receive first SDU segment, discard previous segments, rnti = %d, sn = %d\n", pRxAmEntity->rnti, sn);
                MemFree(pPrevBuffer->pData);
                pPrevBuffer->pData = pCurrBuffer->pData;
                pPrevBuffer->size = pCurrBuffer->size;
                pRawSdu->sn = sn;

                break;
            }
        }
    } else {
        if (pAmdDfe->status == AM_PDU_MAP_SDU_FULL || pAmdDfe->status == AM_PDU_MAP_SDU_START) {
            // previous segment might be lost, so drop all previous incomplete segments
            LOG_WARN(ULP_LOGGER_NAME, "seq num not consecutive, discard previous received SDU segment, sn = %d, pRawSdu->sn = %d, rnti = %d\n",
                sn, pRawSdu->sn, pRxAmEntity->rnti);
            MemFree(pRawSdu->rawSdu.pData);
            pRawSdu->rawSdu.pData = 0;
            pRawSdu->rawSdu.size = 0;

            RlcReassembleFirstSduSegment(sn, pRxAmEntity, pRawSdu, pAmdDfe);
        } else {
            // this might be previous HARQ retransmit data or RLC ARQ retransmit data, so drop it
            LOG_WARN(ULP_LOGGER_NAME, "segment seq num not valid, drop this segment, sn = %d, pRawSdu->sn = %d, rnti = %d, status = %d\n",
                sn, pRawSdu->sn, pRxAmEntity->rnti, pAmdDfe->status);
            MemFree(pAmdDfe->buffer.pData);
        }
    }
}

// --------------------------------
void RlcReassembleFirstSduSegment(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe)
{
    LOG_TRACE(ULP_LOGGER_NAME, "sn = %d, rnti = %d, pAmdDfe->status = %d\n", sn, pRxAmEntity->rnti, pAmdDfe->status);

    switch(pAmdDfe->status) {
        case AM_PDU_MAP_SDU_FULL:
        {
            RlcDeliverAmSduToPdcp(pRxAmEntity, &pAmdDfe->buffer);
            break;
        }

        case AM_PDU_MAP_SDU_START:
        {
            LOG_DBG(ULP_LOGGER_NAME, "save first SDU segment, sn = %d, rnti = %d, data size = %d\n", sn, pRxAmEntity->rnti, pAmdDfe->buffer.size);
            pRawSdu->sn = sn;
            pRawSdu->rawSdu.size = pAmdDfe->buffer.size;
            pRawSdu->rawSdu.pData = pAmdDfe->buffer.pData;
            pAmdDfe->buffer.size = 0;
            pAmdDfe->buffer.pData = 0;
            break;
        }

        default:
        {
            LOG_WARN(ULP_LOGGER_NAME, "should not come here, could be HARQ retransmit or RLC ARQ retransmit PDU, status = %d, rnti = %d\n", pAmdDfe->status, pRxAmEntity->rnti);
            MemFree(pAmdDfe->buffer.pData);
            break;
        }
    }
}

// --------------------------------
void RlcDeliverAmSduToPdcp(RxAMEntity* pRxAmEntity, RlcAmBuffer* pAmBuffer)
{
    LOG_INFO(ULP_LOGGER_NAME, "rnti = %d, lcId = %d, data size = %d\n", pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->size);

    if (!IP_RLC_DATA_IND(pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->pData, pAmBuffer->size)) {
        return;
    }
    
    RlcUeDataInd(pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->pData, pAmBuffer->size);

    pAmBuffer->pData = 0;
    pAmBuffer->size = 0;
}
