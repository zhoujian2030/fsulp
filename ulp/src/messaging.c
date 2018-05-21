/*
 * messaging.c
 *
 *  Created on: May 16, 2018
 *      Author: J.ZH
 */

#include "messaging.h"
#include "lteLogger.h"
#include <string.h>

#ifdef TI_DSP

// ----------------------------
void* MessageQGetFreeTxFd(MessageQueue* pMsgQueue, unsigned char** pBuffer, unsigned int* pBufferLen)
{
	void* pQmssFd = 0;

	if (pMsgQueue == 0 || pBuffer == 0 || pBufferLen == 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "pMsgQueue = %p, pBuffer = %p, bufferLen = %p\n", pMsgQueue, pBuffer, pBufferLen);
		return pQmssFd;
	}

	if (Qmss_getQueueEntryCount(pMsgQueue->qid) <= 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "Qmss_getQueueEntryCount error\n");
		return pQmssFd;
	}

	if ((pQmssFd = Qmss_queuePop(pMsgQueue->qid)) == 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "Qmss_queuePop error\n");
		return pQmssFd;
	}

	Cppi_getData (Cppi_DescType_HOST, (Cppi_Desc*)pQmssFd, pBuffer, pBufferLen);

	return pQmssFd;
}

// ----------------------------
int MessageQSendByFd(MessageQueue* pMsgQueue, void* pFd, unsigned int length)
{
	if (pMsgQueue == 0 || pFd == 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "pMsgQueue = %p, pFd = %p\n", pMsgQueue, pFd);
		return 0;
	}

	Qmss_QueueHnd txQid;

	if (pMsgQueue->qid == QMSS_TX_FREE_HAND_ULP_TO_OAM_LOG) {
		txQid = QMSS_TX_HAND_ULP_TO_OAM_LOG;
	} else {
		// TODO
	}

	Cppi_setPacketLen(Cppi_DescType_HOST, (Cppi_Desc*)pFd, length);
	Cppi_setDataLen(Cppi_DescType_HOST, (Cppi_Desc*)pFd, length);

	Qmss_queuePushDescSize(txQid, (unsigned int*)pFd, Qmss_Desc_Size);

	return length;
}

// ----------------------------
int MessageQSend(MessageQueue* pMsgQueue, char* pBuffer, unsigned int length)
{
	if (pMsgQueue == 0 || pBuffer == 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "pMsgQueue = %p, pBuffer = %p\n", pMsgQueue, pBuffer);
		return 0;
	}

	void* pQmssFd;
	unsigned char* pQmssDataBuff = 0;
	unsigned int qmssBuffLen = 0;

	if ((pQmssFd = MessageQGetFreeTxFd(pMsgQueue, &pQmssDataBuff, &qmssBuffLen)) == 0) {
		LOG_ERROR(ULP_LOGGER_NAME, "MessageQGetFreeTxFd error\n");
		return 0;
	}

//	if (length > qmssBuffLen) {
//		LOG_WARN(ULP_LOGGER_NAME, "length[%d] exceeds qmssBuffLen[%d]\n", length, qmssBuffLen);
//		length = qmssBuffLen;
//	}

	memcpy(pQmssDataBuff, pBuffer, length);

	return MessageQSendByFd(pMsgQueue, pQmssFd, length);
}

// ----------------------------
int MessageQGetData(MessageQueue* pMsgQueue, unsigned char** pBuffer, void** pFd)
{
	unsigned int length = 0;

	if (pMsgQueue == 0 || pBuffer ==  0 || pFd == 0) {
		return length;
	}

	unsigned int queueCount = Qmss_getQueueEntryCount(pMsgQueue->qid);
	if (queueCount == 0) {
		return length;
	}

	*pFd = Qmss_queuePop(pMsgQueue->qid);
	if (*pFd == 0) {
		return length;
	}

	Cppi_getData (Cppi_DescType_HOST, (Cppi_Desc*)*pFd, pBuffer, &length);

	return length;
}

// ----------------------------
void MessageQFreeRecvFd(void* pFd)
{
	if (pFd == 0) {
		return;
	}

	Qmss_queuePushDesc(Qmss_getQueueHandle(Cppi_getReturnQueue(Cppi_DescType_HOST, (Cppi_Desc*)pFd)), (Uint32 *)pFd);
}

// ----------------------------
int MessageQRecv(MessageQueue* pMsgQueue, char* pBuffer, unsigned int bufferLen)
{
	unsigned int length = 0;

	if (pMsgQueue == 0 || pBuffer ==  0) {
		return length;
	}

	unsigned char* pDataBuffer;
	void* pQmssFd;

	length = MessageQGetData(pMsgQueue, &pDataBuffer, &pQmssFd);

	if (length > 0) {
		if (length > bufferLen) {
			length = bufferLen;
		}
		memcpy((void*)pBuffer, (void*)pDataBuffer, length);
	}

	MessageQFreeRecvFd(pQmssFd);

	return length;
}

#endif

