/*
 * target.c
 *
 *  Created on: July 09, 2018
 *      Author: j.zh
 */

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "target.h"
#include "lteLogger.h"
#include "lteUlpOamInterface.h"
#include "mempool.h"
#include "sync.h"

SEM_LOCK gTgtSem;
static LteUlpDataInd gUeEstablishDataInd;

// ------------------------------------
void TgtInit()
{
    SemInit(&gTgtSem, 1);
    memset((void*)&gUeEstablishDataInd, 0, sizeof(LteUlpDataInd));
    gUeEstablishDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    gUeEstablishDataInd.length = LTE_ULP_DATA_IND_HEAD_LEHGTH + LTE_UE_ESTAB_IND_MSG_HEAD_LEHGTH;
}


// ------------------------------------
void TgtProcessUeEstablishInfo(UInt16 rnti, UlReportInfoList* pUlRptInfoList)
{
    if (pUlRptInfoList == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pUlRptInfoList is NULL\n");
        return;
    }

    if (pUlRptInfoList->count == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "pUlRptInfoList->count = 0\n");
        return;
    }

    SemWait(&gTgtSem);

    UeEstablishIndMsg* pUeEstabIndMsg = (UeEstablishIndMsg*)&gUeEstablishDataInd.u.ueEstablishInd;
    if (pUeEstabIndMsg->count >= MAX_NUM_UE_INFO_REPORT) {
        LOG_ERROR(ULP_LOGGER_NAME, "pUeEstabIndMsg->count >= MAX_NUM_UE_INFO_REPORT\n");
        SemPost(&gTgtSem);
        return;
    }

    UeEstablishInfo* pUeEstabInfo = &pUeEstabIndMsg->ueEstabInfoArray[pUeEstabIndMsg->count++];
    struct timeval tv;
    gettimeofday(&tv, 0);
    pUeEstabInfo->timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    pUeEstabInfo->rnti = rnti;
    pUeEstabInfo->prbPower = pUlRptInfoList->ulRptInfo[pUlRptInfoList->count - 1].prbPower; // take the prbPower of last RLC segment
    pUeEstabInfo->ta = pUlRptInfoList->ulRptInfo[pUlRptInfoList->count - 1].ta;
    gUeEstablishDataInd.length += sizeof(UeEstablishInfo);

    LOG_DBG(ULP_LOGGER_NAME, "rnti = %d, prbPower = %d, length = %d\n", pUeEstabInfo->rnti, pUeEstabInfo->prbPower, gUeEstablishDataInd.length);

    SemPost(&gTgtSem);
}

// ------------------------------------
void TgtSendUeEstablishInd(int socketFd, struct sockaddr_in* pRemoteAddr)
{
    if (socketFd == -1 || pRemoteAddr == 0) {
        LOG_ERROR(ULP_LOGGER_NAME, "socketFd = %d, pRemoteAddr = %p\n", socketFd, pRemoteAddr);
        return;
    }

    SemWait(&gTgtSem);

    if (gUeEstablishDataInd.u.ueEstablishInd.count > 0) {
        SocketUdpSend(socketFd, (char*)&gUeEstablishDataInd, gUeEstablishDataInd.length, pRemoteAddr);  
        LOG_DBG(ULP_LOGGER_NAME, "count = %d\n", gUeEstablishDataInd.u.ueEstablishInd.count);
        gUeEstablishDataInd.length = LTE_ULP_DATA_IND_HEAD_LEHGTH + LTE_UE_ESTAB_IND_MSG_HEAD_LEHGTH;
        gUeEstablishDataInd.u.ueEstablishInd.count = 0;
    }

    SemPost(&gTgtSem);
}

