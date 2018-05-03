/*
 * lteMac.c
 *
 *  Created on: Apr 16, 2018
 *      Author: j.zh
 */

#include "lteMac.h"
#include "baseType.h"
#include "lteMacPhyInterface.h"
#include "lteRlcMacInterface.h"
#include "mempool.h"
#include "lteLogger.h"
#include "lteIntegrationPoint.h"

void MacProcessUlSchPdu(UlSchPdu* pPdu);
void MacProcessMacLcId(UInt8 startPaddingFlag,
    UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt8 **dataPtr_p,
    DemuxDataBase *demuxData_p,
    UInt32 *pos,
    UInt32 lcId,
    UInt32 *localLength);
void MacProcessUlCcchLch(UInt8 startPaddingFlag,
    UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt32 *localLength,
    UInt8  **dataPtr_p,
    DemuxDataBase *demuxData_p,
    UInt32 *pos);
void MacProcessPowerHeadLch(SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    DemuxDataBase *demuxData_p,
    UInt32 *pos,
    UInt32 *dataPtrPos);
void MacProcessCrntiLch(SInt32 *remainingPayloadLength,
    UInt8  *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos);
void MacProcessShortOrTruncatedBsr(SInt32 *remainingPayloadLength,
    UInt8  *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos,
    UInt32 lcId);
void MacProcessLongBsrLch(SInt32 *remainingPayloadLength,
    UInt8  *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos);
void MacProcessPaddingLch(UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength, 
    UInt32 *localLength, 
    UInt8  *sucessDecodeFlag,
    UInt32 pos,
    DemuxDataBase *demuxData_p,
    UInt8 *startPaddingFlag);
UInt8* MacCalculateSubHeaderLength(UInt8 *headerPtr_p, UInt32 *len_p, UInt32 *dataPtrPos_p);
void MacDeMultiplexAndSend(DemuxDataBase *demuxData_p,
    UInt8   *origDataPtr_p,
    UInt8   *dataPtr_p,
    UInt32  length,
    UInt16  recvdRNTI,
    UInt32  pos,
    UInt32  dataPtrPos);
void MacDemuxOneToTenLchMsg(UInt32 lchId,
    UInt32 *lcIdx,
    UInt8 *rlcFlag,
    UInt32 *dataPtrPos,
    UInt32 length,
    UInt8 **dataPtr_p,
    RlcUlData **dataToRlc_p,
    UInt8 *ulDataReceivedFlag);

// ---------------------------
void MacUlSchDataInd(unsigned char* pBuffer, unsigned short length) 
{
    if (pBuffer == 0) {
        return;
    }
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], length = %d\n", __func__, length);
    // LOG_BUFFER(pBuffer, length);

    unsigned short tempLen = 0;
    unsigned char  numPdu = 0;
    
#ifdef MAC_PHY_INTF_NEW
    // unsigned char* pTmpBuffer = pBuffer;
    UlSchPdu *pUlSchPdu;
    RxUlSchInd *pRxUlSchInd = (RxUlSchInd*)(pBuffer + tempLen);
    tempLen += UL_SCH_IND_MSG_HEADER_LEN;

    // pTmpBuffer += UL_SCH_IND_MSG_HEADER_LEN;
    
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], sfn = %d, sf = %d, numOfPDUs = %d\n", __func__, pRxUlSchInd->sfn, pRxUlSchInd->sf, pRxUlSchInd->numOfPDUs);

    while ((tempLen < length) && (numPdu < pRxUlSchInd->numOfPDUs)) {
        pUlSchPdu = (UlSchPdu*)(pBuffer + tempLen);
        tempLen += UL_SCH_PDU_HEADER_LENGTH;
        tempLen += pUlSchPdu->length;
        // check if PDU length valid
        if (tempLen > length) {
            LOG_ERROR(ULP_LOGGER_NAME, "[%s], length = %d, tempLen = %d\n", __func__, length, tempLen);
            break;
        }

        MacProcessUlSchPdu(pUlSchPdu);
        numPdu++;
    }
#else
    S_PhyHlMsgHead* pMsgHead = (S_PhyHlMsgHead*)pBuffer;
	tempLen += sizeof(S_PhyHlMsgHead);
    S_UlIndHead *pUlIndHead;
    S_RxUlschIndHeadPdu *pUlSchPduHead;
    UlSchPdu ulSchPdu;

    if (pMsgHead->opc == RX_ULSCH_INDICATION) {
    	pUlIndHead = (S_UlIndHead*)(pBuffer + tempLen);
    	tempLen += sizeof(S_UlIndHead);
        LOG_TRACE(ULP_LOGGER_NAME, "[%s], sfn = %d, sf = %d, numOfPDUs = %d\n", __func__, pUlIndHead->sfn, pUlIndHead->sf, pUlIndHead->numOfPDUs);

		while ((tempLen < length) && (numPdu < pUlIndHead->numOfPDUs)) {
			pUlSchPduHead = (S_RxUlschIndHeadPdu*)(pBuffer + tempLen);
			tempLen += sizeof(S_RxUlschIndHeadPdu);

			// check if PDU length valid
			if (tempLen > length) {
				LOG_ERROR(ULP_LOGGER_NAME, "[%s], length = %d, tempLen = %d\n", __func__, length, tempLen);
				break;
			}

			// only handle crc correct data
			if (pUlSchPduHead->CRCFlag == 1) {
				ulSchPdu.rnti = pUlSchPduHead->RNTI;
				ulSchPdu.length = (pUlSchPduHead->bitLen + 7) >> 3;
				ulSchPdu.buffer = pBuffer + tempLen;

				MacProcessUlSchPdu(&ulSchPdu);
			} else {
		    	LOG_ERROR(ULP_LOGGER_NAME, "[%s], crc error, ignore it, rnti = %d\n", __func__, pUlSchPduHead->RNTI);
			}

			tempLen += (pUlSchPduHead->wordLen << 2);
			numPdu++;
		}

    } else {
    	LOG_ERROR(ULP_LOGGER_NAME, "[%s], unsupported opc = %d\n", __func__, pMsgHead->opc);
    }
#endif
}

// ---------------------------
void MacProcessUlSchPdu(UlSchPdu* pPdu)
{
    LOG_DBG(ULP_LOGGER_NAME, "[%s], rnti = %d, length = %d\n", __func__, pPdu->rnti, pPdu->length);
    if ((pPdu->buffer == 0) || (pPdu->length == 0)) {
        return;
    }

    LOG_BUFFER(pPdu->buffer, pPdu->length);

    UInt8 extnFlag = FALSE;
    UInt8 successDecodeFlag = TRUE;
    UInt8 startPaddingFlag = FALSE;
    UInt8* pData = pPdu->buffer;
    UInt32 lcId = 0;
    UInt32 pos = 0;
    UInt32 dataPtrPos  = 0;
    UInt32 localLength = 0;
    SInt32 remainingPayloadLength = pPdu->length;

    DemuxDataBase demuxData[MAX_NUM_CHANNELS] = {{0}};
    DemuxDataBase *pDemuxData = &demuxData[0];

    do {
        lcId = (*pData & 0x1f);
        if (*pData & MAC_UL_PDU_EXTN_FLAG) {
            extnFlag = TRUE;
        }
        else {
            extnFlag = FALSE;
        }

        localLength = 0;

        LOG_TRACE(ULP_LOGGER_NAME, "[%s], lcId = %d, extnFlag = %d\n", __func__, lcId, extnFlag);

        switch (lcId) {
            case MAC_UL_CCCH_LCH:
            {
                MacProcessUlCcchLch(startPaddingFlag,
                    extnFlag,
                    &dataPtrPos,
                    &remainingPayloadLength,
                    &successDecodeFlag,
                    &localLength,
                    &pData,
                    pDemuxData,
                    &pos);
                break;
            }

            case MAC_LCID_1:
            case MAC_LCID_2:
            case MAC_LCID_3:
            case MAC_LCID_4:
            case MAC_LCID_5:
            case MAC_LCID_6:
            case MAC_LCID_7:
            case MAC_LCID_8:
            case MAC_LCID_9:  
            case MAC_LCID_10:
            {
                MacProcessMacLcId(startPaddingFlag, 
                    extnFlag, 
                    &dataPtrPos, 
                    &remainingPayloadLength, 
                    &successDecodeFlag, 
                    &pData, 
                    pDemuxData, 
                    &pos, 
                    lcId, 
                    &localLength);
                break;
            }

            case MAC_POWER_HEAD_LCH:
            {
                MacProcessPowerHeadLch(&remainingPayloadLength,
                    &successDecodeFlag,
                    pDemuxData,
                    &pos,
                    &dataPtrPos);
                break;
            }

            case MAC_CRNTI_LCH:
            {
                MacProcessCrntiLch(&remainingPayloadLength,
                    &successDecodeFlag,
                    &pos,
                    pDemuxData,
                    &dataPtrPos);
                break;
            }

            case MAC_TRUNCATED_BSR:
            case MAC_S_BSR_LCH:
            {
                MacProcessShortOrTruncatedBsr(&remainingPayloadLength,
                    &successDecodeFlag,
                    &pos,
                    pDemuxData,
                    &dataPtrPos,
                    lcId);
                break;
            }

            case MAC_L_BSR_LCH:
            {
                MacProcessLongBsrLch(&remainingPayloadLength,
                    &successDecodeFlag,
                    &pos,
                    pDemuxData,
                    &dataPtrPos);
                break;
            } 

            case MAC_PADDING_LCH:
            {
                MacProcessPaddingLch(extnFlag,
                    &dataPtrPos,
                    &remainingPayloadLength, 
                    &localLength, 
                    &successDecodeFlag,
                    pos,
                    pDemuxData,
                    &startPaddingFlag);
                break;
            }

            default:
            {
                LOG_ERROR(ULP_LOGGER_NAME,"[%s] Invalid lcId = %d\n", __func__,lcId);      
            }
        }

        if(remainingPayloadLength < 0)
        {
            successDecodeFlag = FALSE;
            LOG_ERROR(ULP_LOGGER_NAME,"[%s], Invalid remainingPayloadLength = %d\n", __func__, remainingPayloadLength);
        }
        pData++;

    } while((TRUE == extnFlag) && (TRUE == successDecodeFlag));

    if (successDecodeFlag) {
        MacDeMultiplexAndSend(pDemuxData, pPdu->buffer, pData, pPdu->length, pPdu->rnti, pos, dataPtrPos);
    } else {
        LOG_ERROR(ULP_LOGGER_NAME,"[%s], fail to decode the mac pdu\n", __func__);
    }
}

// --------------------------------
void MacProcessMacLcId( UInt8 startPaddingFlag,
    UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt8 **dataPtr_p,
    DemuxDataBase *demuxData_p,
    UInt32 *pos,
    UInt32 lcId,
    UInt32 *localLength )
{
    if (((TRUE == startPaddingFlag) && (FALSE == extnFlag)) || (FALSE == extnFlag)) {
        *dataPtrPos += 1;
        if (*remainingPayloadLength <= *dataPtrPos )
        {
            *sucessDecodeFlag = FALSE;
            return;
        }
        *localLength = *remainingPayloadLength - *dataPtrPos;
    } else if (TRUE == extnFlag) {
        /*Increment the pointer to point to length field*/
        (*dataPtr_p)++;
        *dataPtr_p = MacCalculateSubHeaderLength(*dataPtr_p, localLength, dataPtrPos);
    }

    if ((0 == *localLength) || (*remainingPayloadLength < *localLength))
    {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*localLength != 0) {
        if(*pos < MAX_NUM_CHANNELS) {
            LOG_DBG(ULP_LOGGER_NAME, "[%s], lcId = %d, length = %d\n", __func__, lcId, *localLength);
            (demuxData_p + *pos)->lchId = lcId;
            (demuxData_p + *pos)->length =  *localLength;
            *pos += 1;
        }

        *remainingPayloadLength = *remainingPayloadLength - *localLength;
    }
}

// --------------------------------
void MacProcessUlCcchLch(UInt8 startPaddingFlag,
    UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt32 *localLength,
    UInt8  **dataPtr_p,
    DemuxDataBase *demuxData_p,
    UInt32 *pos) 
{
    if (((TRUE == startPaddingFlag) && (FALSE == extnFlag)) || (FALSE == extnFlag)) {
        /*This is the case with 1/2 byte padding at the start of 
         *PDU or no padding. So the last subheader is of 1byte
         *i.e no length field.
         */
        *dataPtrPos += 1;

        if (*remainingPayloadLength <= *dataPtrPos )
        {
            *sucessDecodeFlag = FALSE;
            return;
        }
        *localLength = *remainingPayloadLength - *dataPtrPos;
    } else if (TRUE == extnFlag) {
        /*Increment the pointer to point to length field*/
        (*dataPtr_p)++;
        *dataPtr_p = MacCalculateSubHeaderLength(*dataPtr_p, localLength, dataPtrPos);
    }

    if ( (0 == *localLength) || (*remainingPayloadLength < *localLength)|| (*localLength>100) ) {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*localLength != 0) {
        if(*pos < MAX_NUM_CHANNELS) {
            LOG_TRACE(ULP_LOGGER_NAME, "[%s], lcId = %d, length = %d\n", __func__, MAC_UL_CCCH_LCH, *localLength);
            (demuxData_p + *pos)->lchId = MAC_UL_CCCH_LCH;
            (demuxData_p + *pos)->length =  *localLength;
            *pos += 1;
        }
        *remainingPayloadLength = *remainingPayloadLength -*localLength;
    }
}

// --------------------------------
void MacProcessPowerHeadLch(SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    DemuxDataBase *demuxData_p,
    UInt32 *pos,
    UInt32 *dataPtrPos)
{
    if(*remainingPayloadLength < POWER_HEADROOM_MAC_CE_LENGTH)
    {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*pos < MAX_NUM_CHANNELS)
    {
        (demuxData_p + *pos)->lchId   = MAC_POWER_HEAD_LCH;
        (demuxData_p + *pos)->length  = POWER_HEADROOM_MAC_CE_LENGTH;
        *pos += 1;
    }

    *remainingPayloadLength = 
        *remainingPayloadLength - POWER_HEADROOM_MAC_CE_LENGTH;

    *dataPtrPos += 1;    
}

// --------------------------------
void MacProcessCrntiLch(SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos)
{
    if(*remainingPayloadLength < CRNTI_MAC_CE_LENGTH)
    {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*pos < MAX_NUM_CHANNELS)
    {
        (demuxData_p + *pos)->lchId = MAC_CRNTI_LCH;
        (demuxData_p + *pos)->length = CRNTI_MAC_CE_LENGTH;
        *pos += 1;
    }

    *remainingPayloadLength = *remainingPayloadLength - CRNTI_MAC_CE_LENGTH;
    *dataPtrPos += 1;
}

// --------------------------------
void MacProcessShortOrTruncatedBsr(SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos,
    UInt32 lcId)
{
    LOG_TRACE(ULP_LOGGER_NAME, "[%s], remainingPayloadLength = %d\n", __func__, *remainingPayloadLength);
    if(*remainingPayloadLength < SHORT_BSR_MAC_CE_LENGTH)
    {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*pos < MAX_NUM_CHANNELS)
    {
        (demuxData_p + *pos)->lchId  = lcId;
        (demuxData_p + *pos)->length = SHORT_BSR_MAC_CE_LENGTH;
        *pos += 1;                    
    }

    *remainingPayloadLength = *remainingPayloadLength - SHORT_BSR_MAC_CE_LENGTH;
    *dataPtrPos += 1;
}

// --------------------------------
void MacProcessLongBsrLch(SInt32 *remainingPayloadLength,
    UInt8 *sucessDecodeFlag,
    UInt32 *pos,
    DemuxDataBase *demuxData_p,
    UInt32 *dataPtrPos)
{
    if(*remainingPayloadLength < LONG_BSR_MAC_CE_LENGTH)
    {
        *sucessDecodeFlag = FALSE;
        return;
    }

    if(*pos < MAX_NUM_CHANNELS)
    {
        (demuxData_p + *pos)->lchId = MAC_L_BSR_LCH;
        /*Large BSR is of 3 bytes*/
        (demuxData_p + *pos)->length = LONG_BSR_MAC_CE_LENGTH;
        *pos += 1;                    
    }

    *remainingPayloadLength = *remainingPayloadLength - LONG_BSR_MAC_CE_LENGTH;
    *dataPtrPos += 1;
}

// --------------------------------
void MacProcessPaddingLch(UInt8 extnFlag,
    UInt32 *dataPtrPos,
    SInt32 *remainingPayloadLength, 
    UInt32 *localLength, 
    UInt8 *sucessDecodeFlag,
    UInt32 pos,
    DemuxDataBase *demuxData_p,
    UInt8 *startPaddingFlag)
{
    if (FALSE == extnFlag)
    {
        *dataPtrPos += 1;
        /*case where padding is of 1 byte and present 
         *after the last MAC PDU subheader
         */
        if(*remainingPayloadLength > *dataPtrPos)
        {
            *localLength = *remainingPayloadLength - *dataPtrPos ;
        }
        else
        {
            *localLength = 
                *dataPtrPos - *remainingPayloadLength;
        }
        if(*remainingPayloadLength >= *localLength)
        {
            *remainingPayloadLength = 
                *remainingPayloadLength - *localLength;

        }
        else
        {
            *sucessDecodeFlag = FALSE;
            return;
        }

        if(pos < MAX_NUM_CHANNELS)
        {
            (demuxData_p + pos)->length = 0;
        }
    }
    else
    {
        *startPaddingFlag = TRUE;
        *dataPtrPos += 1;
    }
}

// --------------------------------
UInt8* MacCalculateSubHeaderLength(UInt8 *headerPtr_p, UInt32 *len_p, UInt32 *dataPtrPos_p)
{
    *len_p = 0;
    
    /*Reffer section 6.1.2 in 36321-850.doc*/
    if (*headerPtr_p & 0x80)
    {
        /*length is greater than 128 bytes*/
        *len_p = *headerPtr_p++ & 0x7F;
        *len_p <<= 8;

        *len_p |= *headerPtr_p;
        *dataPtrPos_p = *dataPtrPos_p+3;
    }
    else
    {
        /*length is less than 128 bytes*/
        *len_p = *headerPtr_p & 0x7F;
        *dataPtrPos_p = *dataPtrPos_p + 2;
    }

    return headerPtr_p;    
}

// -----------------------------
void MacDeMultiplexAndSend(DemuxDataBase *demuxData_p,
    UInt8   *origDataPtr_p,
    UInt8   *dataPtr_p,
    UInt32  length,
    UInt16  recvdRNTI,
    UInt32  pos,
    UInt32  dataPtrPos)
{
    UInt32 idx = 0;
    UInt32 lcIdx  = 0;
    UInt32 lchId  = 0;
    UInt32 lcIdSduLen = 0;
    UInt8 rlcFlag = FALSE;
    // RlcUlData* pRlcUlData = 0;
    UInt8 ulDataReceivedFlag = FALSE;
    MacUeDataInd_t* pMacUeDataInd = (MacUeDataInd_t*)MemAlloc(sizeof(MacUeDataInd_t));
    pMacUeDataInd->rnti = recvdRNTI;
    pMacUeDataInd->rlcData = 0;

    LOG_TRACE(ULP_LOGGER_NAME, "[%s], origDataPtr_p = %p, dataPtr_p = %p, length = %d, recvdRNTI = %d, dataPtrPos = %d\n", 
        __func__, origDataPtr_p, dataPtr_p, length, recvdRNTI, dataPtrPos);

    for (idx=0; idx < pos && (pos <= MAX_NUM_CHANNELS); idx++) {
        lchId  = (demuxData_p + idx)->lchId;
        lcIdSduLen = (demuxData_p + idx)->length;

        LOG_TRACE(ULP_LOGGER_NAME, "[%s], lchId = %d, lcIdSduLen = %d\n", __func__, lchId, lcIdSduLen);

        switch(lchId) {
            case MAC_UL_CCCH_LCH:
            {
                LOG_DBG(ULP_LOGGER_NAME, "[%s], TODO, MAC_UL_CCCH_LCH\n", __func__);
                return;
            }

            case MAC_LCID_1:
            case MAC_LCID_2:
            case MAC_LCID_3:
            case MAC_LCID_4:
            case MAC_LCID_5:
            case MAC_LCID_6:
            case MAC_LCID_7:
            case MAC_LCID_8:
            case MAC_LCID_9:
            case MAC_LCID_10:
            {
                 MacDemuxOneToTenLchMsg(lchId,
                    &lcIdx,
                    &rlcFlag,
                    &dataPtrPos,
                    lcIdSduLen,
                    &dataPtr_p,
                    &pMacUeDataInd->rlcData,
                    &ulDataReceivedFlag);
                break;
            }

            case MAC_TRUNCATED_BSR:
            case MAC_S_BSR_LCH:
            {
                UInt16 bsrVal = (*dataPtr_p & 0x3F);
                UInt32 shortBSRLcgId = ((*dataPtr_p & 0xC0)>>6);

                LOG_TRACE(ULP_LOGGER_NAME, "[%s], bsrVal = %d, shortBSRLcgId = %d\n", __func__, bsrVal, shortBSRLcgId);

                dataPtrPos = dataPtrPos + lcIdSduLen;
                /*increment data pointer to point to next SDU*/
                dataPtr_p++;
                break;
            }

            case MAC_L_BSR_LCH:
            {
                UInt8 bsrVal0 = ((*dataPtr_p & 0xFC) >> 2);

                UInt8 bsrVal1 = ((*dataPtr_p & 0x03) << 4);
                dataPtr_p++;
                bsrVal1 |= ((*dataPtr_p & 0xF0) >>4 );

                UInt8 bsrVal2 = ((*dataPtr_p & 0x0F) <<2 );
                dataPtr_p++;
                bsrVal2 |=((*dataPtr_p & 0xC0) >> 6);

                UInt8 bsrVal3 = (*dataPtr_p & 0x3F);
                dataPtr_p++;

                LOG_TRACE(ULP_LOGGER_NAME, "[%s], bsrVal0 = %d, bsrVal1 = %d, bsrVal2 = %d, bsrVal3 = %d\n", __func__, bsrVal0, bsrVal1, bsrVal2, bsrVal3);

                dataPtrPos = dataPtrPos + lcIdSduLen;      
                break;
            }  

            case MAC_POWER_HEAD_LCH:
            {
                UInt8 ph = *dataPtr_p & 0x3F;
                LOG_TRACE(ULP_LOGGER_NAME, "[%s], ph = %d\n", __func__, ph);

                dataPtrPos = dataPtrPos + lcIdSduLen;
                dataPtr_p++;
                break;
            }

            default:
            {
                dataPtr_p += lcIdSduLen;
                dataPtrPos = dataPtrPos + lcIdSduLen;
                break;
            }
        }
    }

    if(!IP_MAC_DATA_IND(pMacUeDataInd)) {
        return;
    }
    MacUeDataInd(pMacUeDataInd);
}

// ------------------------------------------------
void MacDemuxOneToTenLchMsg(UInt32 lchId,
    UInt32 *lcIdx,
    UInt8 *rlcFlag,
    UInt32 *dataPtrPos,
    UInt32 length,
    UInt8 **dataPtr_p,
    RlcUlData **dataToRlc_p,
    UInt8 *ulDataReceivedFlag)
{
    RlcUlDataInfo *rlcLCIdData_p = 0;

    LOG_DBG(ULP_LOGGER_NAME, "[%s], *dataPtrPos = %d, length = %d, lchId = %d\n", __func__, *dataPtrPos, length, lchId);

    if (*lcIdx >= MAX_NUM_UL_PKTS) {
        //TODO
        LOG_WARN(ULP_LOGGER_NAME, "[%s], invalid *lcIdx = %d\n", __func__, *lcIdx);
    } else {
        *ulDataReceivedFlag = TRUE;

        /*data arrrived for RLC*/
        if (0 == *dataToRlc_p) {
            *dataToRlc_p = (RlcUlData*)MemAlloc(sizeof(RlcUlData));
        }    

        rlcLCIdData_p = &(*dataToRlc_p)->rlcDataArray[*lcIdx];
        *lcIdx += 1;
        (*dataToRlc_p)->numLCInfo = *lcIdx;

        /* fill the struct for sending data to RLC*/
        rlcLCIdData_p->rlcdataBuffer = MemAlloc(length);
        memcpy(rlcLCIdData_p->rlcdataBuffer, *dataPtr_p, length);
        rlcLCIdData_p->length = length;
        rlcLCIdData_p->lcId = lchId;

        // LOG_BUFFER(rlcLCIdData_p->rlcdataBuffer, rlcLCIdData_p->length);
        
        *rlcFlag = TRUE;

        /*increment dataPtr_p by len so that dataPtr_p reaches 
        *to start of next RLC SDU.
        */
        (*dataPtr_p) += length;
        *dataPtrPos += length;
    }
}
