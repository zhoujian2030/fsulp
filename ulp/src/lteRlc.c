/*
 * lteRlc.c
 *
 *  Created on: Apr 18, 2018
 *      Author: j.zh
 */

#include "lteRlc.h"
#include "baseType.h"
#include "lteRlcMacInterface.h"
#include "ltePdcpRlcInterface.h"
#include "mempool.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"

BOOL isSNEqual(UInt16 x, UInt16 y);
void RlcProcessAMRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti);
BOOL RlcDecodeAmdHeader(RlcUlDataInfo* pRlcDataInfo, AmdHeader* pAmdHeader);
BOOL RlcDecodeAmdPdu(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo);
BOOL RlcProcessAmdPduSegment(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo);
void RlcSetAmdPduAndDfeStatus(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, AmdDFE* pAmdDfe);
void RlcReassembleRlcSdu(UInt16 sn, RxAMEntity* pRxAmEntity);
void RlcReassembleAmdDfeQ(UInt16 sn, RxAMEntity* pRxAmEntity, AmdPduSegment* pAmdPduSeg);
void RlcReassembleInCmpAMSdu(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe);
void RlcReassembleFirstSduSegment(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe);
void RlcDeliverAmSduToPdcp(RxAMEntity* pRxAmEntity, RlcAmBuffer* pAmBuffer);

List gRlcUeContextList;

// -----------------------------------
void InitRlcLayer()
{
    ListInit(&gRlcUeContextList, TRUE);
}

// -----------------------------------
RlcUeContext* GetRlcUeContext(unsigned rnti)
{
    RlcUeContext* pUeCtx = (RlcUeContext*)ListGetFirstNode(&gRlcUeContextList);
    while (pUeCtx != 0) {
        if (pUeCtx->rnti == rnti) {
            return pUeCtx;
        } else {
            pUeCtx = (RlcUeContext*)ListGetNextNode(&pUeCtx->node);
        }
    }

    LOG_TRACE(ULP_LOGGER_NAME, "[%s], UE context NOT exists, rnti = %d\n", __func__, rnti);
    return 0;
}

// -----------------------------------
void SaveRlcUeContext(RlcUeContext* pRlcUeCtx)
{
    if (pRlcUeCtx != 0) {
        ListPushNode(&gRlcUeContextList, &pRlcUeCtx->node);
    } else {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], pRlcUeCtx null\n", __func__);
    }
}

// -----------------------------------
void DeleteRlcUeContext(RlcUeContext* pRlcUeCtx)
{
    if (pRlcUeCtx != 0) {
        ListDeleteNode(&gRlcUeContextList, &pRlcUeCtx->node);
    }
}

// -----------------------------------
void MacUeDataInd(MacUeDataInd_t* pMacDataInd) 
{
    if (pMacDataInd == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], NULL pointer pMacDataInd\n", __func__);
        return;
    }

    if (pMacDataInd->rlcData == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], NULL pointer pMacDataInd->rlcData\n", __func__);
        return;
    }

    RlcUlData* pRlcData = pMacDataInd->rlcData;
    RlcUlDataInfo* pRlcDataInfo = 0;
    UInt32 i=0;

    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rnti = %d, numLCInfo = %d\n", __func__, pMacDataInd->rnti, pRlcData->numLCInfo);
    for (i=0; i<pRlcData->numLCInfo; i++) {
        pRlcDataInfo = &pRlcData->rlcDataArray[i];
        LOG_DBG(ULP_LOGGER_NAME, "[%s], lcId = %d, length = %d\n", __func__, pRlcDataInfo->lcId, pRlcDataInfo->length);
        LOG_BUFFER(pRlcDataInfo->rlcdataBuffer, pRlcDataInfo->length);

        if (pRlcDataInfo->lcId <= SRB_2_LCID) {
            RlcProcessAMRxPacket(pRlcDataInfo, pMacDataInd->rnti);            
        }

        // leave this memory released in upper layer
        // MemFree(pRlcDataInfo->rlcdataBuffer);
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
void RlcProcessAMRxPacket(RlcUlDataInfo* pRlcDataInfo, UInt16 rnti)
{
    BOOL ret = RLC_FAILURE;
    AmdHeader amdHeader = {0};
    RlcUeContext* pUeCtx = 0;
    RxAMEntity* pRxAmEntity = 0;
    UInt16 ringSn = 0;
    UInt32 i = 0;
    AmdPdu* pAmdPdu = 0;

    if (pRlcDataInfo->lcId > SRB_2_LCID) {
        LOG_WARN(ULP_LOGGER_NAME, "[%s], unsupport lcId = %d, rnti = %d\n", __func__, pRlcDataInfo->lcId, rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    ret = RlcDecodeAmdHeader(pRlcDataInfo, &amdHeader);

    if (ret == RLC_FAILURE) {
        LOG_TRACE(ULP_LOGGER_NAME, "[%s], fail to decode AMD, rnti = %d\n", __func__, rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    if (!amdHeader.dc) {
        LOG_DBG(ULP_LOGGER_NAME, "[%s], receive RLC status PDU, ignore it, rnti = %d\n", __func__, rnti);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return;
    }

    // create UE context if not exists
    pUeCtx = GetRlcUeContext(rnti);
    if (pUeCtx == 0) {
        LOG_DBG(ULP_LOGGER_NAME, "[%s], create rlc ue context, rnti = %d\n", __func__, rnti);
        pUeCtx = (RlcUeContext*)MemAlloc(sizeof(RlcUeContext));
        memset(pUeCtx, 0, sizeof(RlcUeContext));
        pUeCtx->rnti = rnti;    
        SaveRlcUeContext(pUeCtx);
    }

    // create RX AM entity if not exists
    pRxAmEntity = pUeCtx->rxAMEntityArray[pRlcDataInfo->lcId];
    if (pRxAmEntity == 0) {        
        LOG_DBG(ULP_LOGGER_NAME, "[%s], create rlc AM entity, rnti = %d, lcId = %d\n", __func__, rnti, pRlcDataInfo->lcId);
        pRxAmEntity = (RxAMEntity*)MemAlloc(sizeof(RxAMEntity));
        memset(pRxAmEntity, 0, sizeof(RxAMEntity));
        pUeCtx->rxAMEntityArray[pRlcDataInfo->lcId] = pRxAmEntity;
        pRxAmEntity->rnti = rnti;
        pRxAmEntity->lcId = pRlcDataInfo->lcId;
        pRxAmEntity->amdPduRing.size = 512;
        for (i=0; i<pRxAmEntity->amdPduRing.size; i++) {
             pRxAmEntity->amdPduRing.rNodeArray[i].status = RS_FREE;
             pRxAmEntity->amdPduRing.rNodeArray[i].data = 0;
        }
    }

    // check if receive segment before
    ringSn = amdHeader.sn & 511;
    pAmdPdu = (AmdPdu*)pRxAmEntity->amdPduRing.rNodeArray[ringSn].data;
    
    // new seq num 
    if (pRxAmEntity->amdPduRing.rNodeArray[ringSn].status == RS_FREE) 
    {
        if (pAmdPdu == 0) {
            LOG_DBG(ULP_LOGGER_NAME, "[%s], create AM PDU, rnti = %d\n", __func__, rnti);
            pAmdPdu = (AmdPdu*)MemAlloc(sizeof(AmdPdu));
            pRxAmEntity->amdPduRing.rNodeArray[ringSn].data = (void*)pAmdPdu;
            ListInit(&pAmdPdu->segList, 0);
        }

        if (amdHeader.rf) {
            // handle AMD PDU segment
            LOG_WARN(ULP_LOGGER_NAME, "[%s], TODO handle first AMD PDU segment, rnti = %d\n", __func__, rnti);
            ret = RlcProcessAmdPduSegment(pAmdPdu, &amdHeader, pRlcDataInfo);
            // TODO
        } else {
            ret = RlcDecodeAmdPdu(pAmdPdu, &amdHeader, pRlcDataInfo);
            if (ret == RLC_SUCCESS) {
                pAmdPdu->status = PDU_AM_COMPLETE;
            }          
        }

        if (ret == RLC_SUCCESS) {
            pRxAmEntity->amdPduRing.rNodeArray[ringSn].status = RS_IN_USE;
        }
    } 
    // new PDU segment
    else 
    {
        if (pAmdPdu == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "[%s], pAmdPdu is null, could not happen! rnti = %d\n", __func__, rnti);
            // any resource to free?
            MemFree(pRlcDataInfo->rlcdataBuffer);
            return;
        }

        LOG_TRACE(ULP_LOGGER_NAME, "[%s], receive rlc segment, rnti = %d\n", __func__, rnti);

        if (amdHeader.rf) {
            // handle AMD segment
            LOG_WARN(ULP_LOGGER_NAME, "[%s], TODO handle AMD segment, rnti = %d\n", __func__, rnti);
            ret = RlcProcessAmdPduSegment(pAmdPdu, &amdHeader, pRlcDataInfo);
            // TODO
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "[%s], TODO handle last AMD PDU segment, rnti = %d\n", __func__, rnti);
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
    if (pRlcDataInfo->rlcdataBuffer == 0) {
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

        LOG_TRACE(ULP_LOGGER_NAME, "[%s], p = %d, sn = %d, e = %d, rf = %d, fi = %d\n",
            __func__, pAmdHeader->p, pAmdHeader->sn, pAmdHeader->e, pAmdHeader->rf, pAmdHeader->fi);

        /*Decode AMD PDU Segment*/    
        if ( pAmdHeader->rf ) {
            /* Get Last Segment Flag (LSF) field */
            pAmdHeader->lsf =  (data[i + 2] >> 7 ) & 0x01U;
            /* Get Segment Offset (SO) field */
            pAmdHeader->so  = ( ((UInt16)(data[i + 2] & 0x7FU ) << 8) | (data[i + 3] & 0xFFU )); 
            LOG_TRACE(ULP_LOGGER_NAME, "[%s], lsf = %d, so = %d\n", __func__, pAmdHeader->lsf, pAmdHeader->so);
        }
    } else {
        UInt8 cpt = (data[i] >> 4) & 0x07;
        UInt16 ackSn = 0;
    	if (cpt == 0) {
    		ackSn = ((data[i] & 0x0f) << 6) | ((data[i+1] >> 2) &0x3f);
        	LOG_TRACE(ULP_LOGGER_NAME, "[%s], ackSn = %d\n", __func__, ackSn);
    	}
    }

    return TRUE;
}

// ---------------------------------
BOOL RlcDecodeAmdPdu(AmdPdu* pAmdPdu, AmdHeader* pAmdHeader, RlcUlDataInfo* pRlcDataInfo)
{
    LOG_DBG(ULP_LOGGER_NAME, "[%s], sn = %d, fi = %d, e = %d, length = %d\n", 
        __func__, pAmdHeader->sn, pAmdHeader->fi, pAmdHeader->e, pRlcDataInfo->length);

    BOOL ret = RLC_SUCCESS;
    AmdDFE *pDfe = 0 ;
    unsigned int size = pRlcDataInfo->length - 2; // 2 bytes header
    unsigned int segCount = ListCount(&pAmdPdu->segList);

    if (segCount) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], segCount = %d\n", __func__, segCount);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return RLC_FAILURE;
    }    

    // init pdu segment
    AmdPduSegment* pAmdPduSegment = (AmdPduSegment*)MemAlloc(sizeof(AmdPduSegment));
    if (pAmdPduSegment == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], Fail to create AmdPduSegment\n", __func__);
        MemFree(pRlcDataInfo->rlcdataBuffer);
        return RLC_FAILURE;
    }
    memset(pAmdPduSegment, 0, sizeof(AmdPduSegment));
    ListInit(&pAmdPduSegment->dfeQ, 0);
    pAmdPduSegment->soStart = 0;
    pAmdPduSegment->soEnd = 0xFFFF;

    if (pAmdHeader->e == 0) {
        pDfe = (AmdDFE*)MemAlloc(sizeof(AmdDFE));
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
        // TODO
        LOG_ERROR(ULP_LOGGER_NAME, "[%s], TODO\n", __func__);
    }

    ListPushNode(&pAmdPdu->segList, &pAmdPduSegment->node);

    return ret;
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
    LOG_DBG(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, lcId = %d\n", __func__, sn, pRxAmEntity->rnti, pRxAmEntity->lcId);

    Ring* pRing = &pRxAmEntity->amdPduRing;
    UInt16 ringSn = sn & 511;
    AmdPdu* pAmdPdu = 0;
    AmdPduSegment* pAmdPduSeg = 0;
    
    if (pRing->rNodeArray[ringSn].status == RS_IN_USE) {
        pAmdPdu = pRing->rNodeArray[ringSn].data;
        if (pAmdPdu == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "[%s], pAmdPdu is null, sn = %d, rnti = %d, lcId = %d\n", __func__, sn, pRxAmEntity->rnti, pRxAmEntity->lcId);
            pRing->rNodeArray[ringSn].status = RS_FREE;
        } else {
            if ((pAmdPdu->status == PDU_AM_COMPLETE) || (pAmdPdu->status == PDU_AM_SEGMENT_COMPLETE)) {
                pRing->rNodeArray[ringSn].status = RS_READY;

                pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
                while (pAmdPduSeg != 0) {
                    RlcReassembleAmdDfeQ(sn, pRxAmEntity, pAmdPduSeg);
                    ListDeInit(&pAmdPduSeg->dfeQ);
                    MemFree(pAmdPduSeg);
                    pAmdPduSeg = (AmdPduSegment*)ListPopNode(&pAmdPdu->segList);
                }

                ListDeInit(&pAmdPdu->segList);
                MemFree(pAmdPdu);
                pRing->rNodeArray[ringSn].data = 0;
                pRing->rNodeArray[ringSn].status = RS_FREE;
            } else {
                LOG_TRACE(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, lcId = %d, pAmdPdu->status = %d\n",
                    __func__, sn, pRxAmEntity->rnti, pRxAmEntity->lcId, pAmdPdu->status);
            }
        }
    } else {
        LOG_TRACE(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, lcId = %d, ring node status = %d\n", 
            __func__, sn, pRxAmEntity->rnti, pRxAmEntity->lcId, pRing->rNodeArray[ringSn].status);
    }
}

// --------------------------------
void RlcReassembleAmdDfeQ(UInt16 sn, RxAMEntity* pRxAmEntity, AmdPduSegment* pAmdPduSeg)
{
    LOG_DBG(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, pAmdPduSeg = %p\n", __func__, sn, pRxAmEntity->rnti, pAmdPduSeg);

    RlcAmRawSdu* pRawSdu = &pRxAmEntity->rxRawSdu;

    AmdDFE* pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
    while (pAmdDfe != 0 && pAmdDfe->buffer.pData != 0) {
        if (pRawSdu->rawSdu.pData != 0) {
            RlcReassembleInCmpAMSdu(sn, pRxAmEntity, pRawSdu, pAmdDfe);
        } else {
            RlcReassembleFirstSduSegment(sn, pRxAmEntity, pRawSdu, pAmdDfe);
        }
        MemFree((void*)pAmdDfe);
        pAmdDfe = (AmdDFE*)ListPopNode(&pAmdPduSeg->dfeQ);
    } 
}

// --------------------------------
void RlcReassembleInCmpAMSdu(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe)
{
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, pAmdDfe->status = %d\n", __func__, sn, pRxAmEntity->rnti, pAmdDfe->status);

    RlcAmBuffer *pPrevBuffer = &(pRawSdu->rawSdu);
    RlcAmBuffer *pCurrBuffer = &(pAmdDfe->buffer);
    UInt8* pTmpBuffer = 0;

    if (isSNEqual(sn, pRawSdu->sn + 1) || isSNEqual(sn, pRawSdu->sn)) {
        switch(pAmdDfe->status) {
            case AM_PDU_MAP_SDU_END:
            {
                LOG_DBG(ULP_LOGGER_NAME, "[%s], receive last SDU segment, rnti = %d, sn = %d\n", __func__, pRxAmEntity->rnti, sn);

                // pTmpBuffer = MemAlloc(pPrevBuffer->size + pCurrBuffer->size);
                // memcpy(pTmpBuffer, pPrevBuffer->pData, pPrevBuffer->size);
                // memcpy(pTmpBuffer + pPrevBuffer->size, pCurrBuffer->pData, pCurrBuffer->size);
                // MemFree(pPrevBuffer->pData);
                // MemFree(pCurrBuffer->pData);
                // pPrevBuffer->size += pCurrBuffer->size;
                // pPrevBuffer->pData = pTmpBuffer;

                pTmpBuffer = MemJoin(pPrevBuffer->pData, pCurrBuffer->pData);
                if (pTmpBuffer == 0) {
                    LOG_ERROR(ULP_LOGGER_NAME, "[%s], MemJoin failure, pPrevBuffer->pData = %p, pCurrBuffer->pData = %p\n", 
                        __func__, pPrevBuffer->pData, pCurrBuffer->pData);
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
                LOG_DBG(ULP_LOGGER_NAME, "[%s], receive middle SDU segment, rnti = %d, sn = %d\n", __func__, pRxAmEntity->rnti, sn);

                pTmpBuffer = MemAlloc(pPrevBuffer->size + pCurrBuffer->size);
                memcpy(pTmpBuffer, pPrevBuffer->pData, pPrevBuffer->size);
                memcpy(pTmpBuffer + pPrevBuffer->size, pCurrBuffer->pData, pCurrBuffer->size);
                MemFree(pPrevBuffer->pData);
                MemFree(pCurrBuffer->pData);
                pPrevBuffer->size += pCurrBuffer->size;
                pPrevBuffer->pData = pTmpBuffer;
                pRawSdu->sn = sn;

                break;
            }

            case AM_PDU_MAP_SDU_FULL:
            {
                LOG_WARN(ULP_LOGGER_NAME, "[%s], receive single SDU, discard previous segments, rnti = %d, sn = %d\n", __func__, pRxAmEntity->rnti, sn);
                MemFree(pPrevBuffer->pData);
                pPrevBuffer->pData = 0;
                pPrevBuffer->size = 0;

                RlcDeliverAmSduToPdcp(pRxAmEntity, pPrevBuffer);

                break;
            }

            default:
            {
                LOG_WARN(ULP_LOGGER_NAME, "[%s], receive first SDU segment, discard previous segments, rnti = %d, sn = %d\n", __func__, pRxAmEntity->rnti, sn);
                MemFree(pPrevBuffer->pData);
                pPrevBuffer->pData = pCurrBuffer->pData;
                pPrevBuffer->size = pCurrBuffer->size;
                pRawSdu->sn = sn;

                break;
            }
        }
    } else {
        LOG_WARN(ULP_LOGGER_NAME, "[%s], seq num not consecutive, discard previous received SDU segment, sn = %d, pRawSdu->sn = %d, rnti = %d\n",
            __func__, sn, pRawSdu->sn, pRxAmEntity->rnti);
        MemFree(pRawSdu->rawSdu.pData);
        pRawSdu->rawSdu.pData = 0;
        pRawSdu->rawSdu.size = 0;

        RlcReassembleFirstSduSegment(sn, pRxAmEntity, pRawSdu, pAmdDfe);
    }
}

// --------------------------------
void RlcReassembleFirstSduSegment(UInt16 sn, RxAMEntity* pRxAmEntity, RlcAmRawSdu *pRawSdu, AmdDFE* pAmdDfe)
{
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], sn = %d, rnti = %d, pAmdDfe->status = %d\n", __func__, sn, pRxAmEntity->rnti, pAmdDfe->status);

    switch(pAmdDfe->status) {
        case AM_PDU_MAP_SDU_FULL:
        {
            RlcDeliverAmSduToPdcp(pRxAmEntity, &pAmdDfe->buffer);
            break;
        }

        case AM_PDU_MAP_SDU_START:
        {
            LOG_DBG(ULP_LOGGER_NAME, "[%s], save first SDU segment, sn = %d, rnti = %d, data size = %d\n", __func__, sn, pRxAmEntity->rnti, pAmdDfe->buffer.size);
            pRawSdu->sn = sn;
            pRawSdu->rawSdu.size = pAmdDfe->buffer.size;
            pRawSdu->rawSdu.pData = pAmdDfe->buffer.pData;
            pAmdDfe->buffer.size = 0;
            pAmdDfe->buffer.pData = 0;
            break;
        }

        default:
        {
            LOG_WARN(ULP_LOGGER_NAME, "[%s], should not come here, status = %d, rnti = %d\n", __func__, pAmdDfe->status, pRxAmEntity->rnti);
            MemFree(pAmdDfe->buffer.pData);
            break;
        }
    }
}

// --------------------------------
void RlcDeliverAmSduToPdcp(RxAMEntity* pRxAmEntity, RlcAmBuffer* pAmBuffer)
{
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], rnti = %d, lcId = %d, data size = %d\n", __func__, pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->size);
    IP_RLC_DATA_IND(pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->pData, pAmBuffer->size);
    RlcUeDataInd(pRxAmEntity->rnti, pRxAmEntity->lcId, pAmBuffer->pData, pAmBuffer->size);

    pAmBuffer->pData = 0;
    pAmBuffer->size = 0;
}
