/*
 * lteUlpWorker.h
 *
 *  Created on: June 08, 2018
 *      Author: j.zh
 */

#include "lteUlpWorker.h"
#include "lteLogger.h"
#include "lteMacPhyInterface.h"
#include "lteMac.h"
#include "lteRrc.h"
#include "lteUlpOamInterface.h"
#include "thread.h"
#include "socket.h"
#include "messaging.h"
#include "mempool.h"
#include "event.h"
#include "lteConfig.h"
#include "lteKpi.h"
#ifndef OS_LINUX
#include <ti/csl/csl_tsc.h>
#else
#include <sys/time.h>
#endif
#include "target.h"

extern List gRrcUeContextList;
extern List gReadyRrcUeContextList;

// standlone configurations
#ifndef OS_LINUX
#pragma DATA_SECTION(gRecvdPhyDataList, ".ulpdata");
#pragma DATA_SECTION(gUlpHandlerEvent, ".ulpdata");
#pragma DATA_SECTION(gMaxRrcCtxIdleCount, ".ulpdata");
#endif
unsigned char gUlpWorkerInitilized = FALSE;
List gRecvdPhyDataList;
Event gUlpHandlerEvent;
unsigned int gMaxRrcCtxIdleCount = 200; // default 200 * 5 = 1000 ms

#ifdef TI_DSP
#pragma DATA_SECTION(gUlpRecvMessageQ, ".ulpdata");
MessageQueue gUlpRecvMessageQ;
#elif defined ARM_LINUX
#define MAX_MESSAGEQ_BUFFER_LENGTH  4096
MessageQueue gUlpRecvMessageQ;
#endif

#ifdef OS_LINUX
int gOamUdpFd = -1;
struct sockaddr_in gOamAddress;
#ifdef DPE
struct sockaddr_in gDpeAddress;
#endif
#ifdef HEARTBEAT_DEBUG
int gHeartbeatUdpFd = -1;
struct sockaddr_in gTestOamAddress;
#endif
#endif

#ifndef OS_LINUX
#define TASK_ULP_HANDLER_PRIORITY		5
#define TASK_ULP_HANDLER_STACK_SIZE		(256*1024)
#pragma DATA_SECTION(gTaskUlpHandlerStack, ".ulpdata");
UInt8 gTaskUlpHandlerStack[TASK_ULP_HANDLER_STACK_SIZE];
#else
#define TASK_ULP_HANDLER_PRIORITY       98
#endif

void* UlpWorkerEntryFunc(void* p);
void UlpRecvAndHandlePhyData();
void UlpRecvAndHandleOamData();

//---------------------------------
void InitUlpWorker(unsigned char startUlpWorkerFlag)
{    
    if (startUlpWorkerFlag) {        
        gUlpWorkerInitilized = TRUE;
#ifdef INTEGRATE_PHY        
        ListInit(&gRecvdPhyDataList, 1);
#else
        ListInit(&gRecvdPhyDataList, 0);
#endif
        EventInit(&gUlpHandlerEvent);

#ifdef TI_DSP
        gUlpRecvMessageQ.qid = QMSS_RX_HAND_ULP_FROM_L1_DATA;
#elif defined ARM_LINUX
        gUlpRecvMessageQ.qid = QMSS_RX_HAND_ULP_FROM_L1_DATA;
        if (gLteConfig.explicitInitQmssFlag) {
            Init_Netcp();
        }
#endif

#ifdef OS_LINUX 
        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.priority = TASK_ULP_HANDLER_PRIORITY;
        threadParams.policy = RT_SCHED_RR;
        threadParams.stackSize = 0;
        ThreadCreate((void*)UlpWorkerEntryFunc, &threadHandle, &threadParams);
        LOG_DBG(ULP_LOGGER_NAME, "Create ulp handler task\n");
#else 
        ThreadHandle threadHandle;
        ThreadParams threadParams;
        threadParams.stackSize = TASK_ULP_HANDLER_STACK_SIZE;
        threadParams.stack = gTaskUlpHandlerStack;
        threadParams.priority = TASK_ULP_HANDLER_PRIORITY;
        ThreadCreate((void*)UlpWorkerEntryFunc, &threadHandle, &threadParams);
        LOG_DBG(ULP_LOGGER_NAME, "Create ulp handler task\n");
#endif 
    }

    gMaxRrcCtxIdleCount = gLteConfig.ueIdentityWaitTime / gLteConfig.pollingInterval;
    if (gMaxRrcCtxIdleCount == 0) {
        gMaxRrcCtxIdleCount = 200;
    }

#ifdef OS_LINUX
    gOamUdpFd = SocketUdpInitAndBind(gLteConfig.oamUdpPort - 2, "0.0.0.0");
    SocketMakeNonBlocking(gOamUdpFd);
    SocketGetSockaddrByIpAndPort(&gOamAddress, gLteConfig.oamIp, gLteConfig.oamUdpPort);
#ifdef DPE 
    SocketGetSockaddrByIpAndPort(&gDpeAddress, gLteConfig.dpeIp, gLteConfig.dpeUdpPort);
#endif

#ifdef HEARTBEAT_DEBUG
    gHeartbeatUdpFd = SocketUdpInitAndBind(6000, "0.0.0.0");
    SocketGetSockaddrByIpAndPort(&gTestOamAddress, "127.0.0.1", 6002);
    SocketMakeNonBlocking(gHeartbeatUdpFd);
#endif
#endif
}

//---------------------------------
void NotifyUlpWorker() {
    if (gUlpWorkerInitilized) {
        EventSend(&gUlpHandlerEvent);
    }
}

// ---------------------------
void PhyUlDataInd(unsigned char* pBuffer, unsigned short length) 
{
    if (pBuffer == 0) {
        return;
    }

    // LOG_TRACE(ULP_LOGGER_NAME, "length = %d\n", length);
    // LOG_BUFFER(pBuffer, length);

    if (gUlpWorkerInitilized) {
        PhyDataIndNode* pPhyDataInd = (PhyDataIndNode*)MemAlloc(sizeof(PhyDataIndNode));
        if (pPhyDataInd == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for PhyDataIndNode\n");
            return;
        }
        pPhyDataInd->length = length;
        pPhyDataInd->pBuffer = MemAlloc(length);
        if (pPhyDataInd == 0) {
            LOG_ERROR(ULP_LOGGER_NAME, "fail to allocate memory for phy data, length = %d\n", length);
            return;
        }
        memcpy(pPhyDataInd->pBuffer, pBuffer, length);
        ListPushNode(&gRecvdPhyDataList, &pPhyDataInd->node);
    } else {
        MacProcessPhyDataInd(pBuffer, length);
    }
}

//---------------------------------
void UlpRecvAndHandlePhyData()
{
    UInt32 count = 0;
    PhyDataIndNode* pPhyDataNode;

#ifdef TI_DSP
    UInt8* pMsgQBuffer;
    void* pFd = 0;
    UInt32 byteRecvd;
#elif defined ARM_LINUX
    UInt8 recvMsgQBuffer[MAX_MESSAGEQ_BUFFER_LENGTH];
    UInt32 byteRecvd;
#endif

    // process received messages in queue
    count = ListCount(&gRecvdPhyDataList);
    if (count > 0) {
        pPhyDataNode = (PhyDataIndNode*)ListPopNode(&gRecvdPhyDataList);
        while (pPhyDataNode != 0) {
            MacProcessPhyDataInd(pPhyDataNode->pBuffer, pPhyDataNode->length);
            MemFree(pPhyDataNode->pBuffer);
            MemFree(pPhyDataNode);
            pPhyDataNode = (PhyDataIndNode*)ListPopNode(&gRecvdPhyDataList);
        }
    }

#ifdef TI_DSP
    // receive msg from message queue
    count = 0;
    while (count < 4) {
        byteRecvd = MessageQGetData(&gUlpRecvMessageQ, &pMsgQBuffer, &pFd);

        if (byteRecvd == 0) {
            MessageQFreeRecvFd(pFd);
            break;
        }

        PhyUlDataInd(pMsgQBuffer, byteRecvd);

        MessageQFreeRecvFd(pFd);

        count++;
    }
#elif defined ARM_LINUX
    count = MessageQCount(&gUlpRecvMessageQ);
    // LOG_TRACE(ULP_LOGGER_NAME, "count = %d\n", count);
    if (count > 10) {
        count = 10;
    }
    while (count--) {
        byteRecvd = MessageQRecv(&gUlpRecvMessageQ, (char*)recvMsgQBuffer, MAX_MESSAGEQ_BUFFER_LENGTH);

        if (byteRecvd == 0) {
            break;
        }

        PhyUlDataInd(recvMsgQBuffer, byteRecvd);
    }
#elif defined OS_LINUX
    LOG_TRACE(ULP_LOGGER_NAME, "Task running\n");
#endif
}

//---------------------------------
void UlpRecvAndHandleOamData()
{
    char buffer[MAX_UDP_OAM_DATA_BUFFER];
    LteOamDataReq* pOamDataReq = (LteOamDataReq*)buffer;
    LteUlpDataInd* pUlpDataInd = (LteUlpDataInd*)buffer;
    
    // handle heartbeat
#ifdef OS_LINUX
    struct sockaddr_in remoteAddr;
    int byteRecvd = 0;

#ifdef HEARTBEAT_DEBUG
    byteRecvd = SocketUdpRecv(gHeartbeatUdpFd, buffer, MAX_UDP_OAM_DATA_BUFFER, &remoteAddr);
    if (byteRecvd >= LTE_OAM_DATA_REQ_HEAD_LEHGTH) {
        if (pOamDataReq->msgType == MSG_ULP_HEARTBEAT_REQ) {
            gLteKpi.heartbeatResp++;
            pUlpDataInd->msgType = MSG_ULP_HEARTBEAT_RESP;
            pUlpDataInd->length = LTE_ULP_DATA_IND_HEAD_LEHGTH;
            SocketUdpSend(gHeartbeatUdpFd, buffer, sizeof(LteUlpDataInd), &remoteAddr);  
            LOG_INFO(ULP_LOGGER_NAME, "Send Heartbeat response to Test OAM, heartbeatResp = %d\n", gLteKpi.heartbeatResp);
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "invalid msgType = %d from Test OAM\n", pOamDataReq->msgType);
            LOG_BUFFER(buffer, byteRecvd);
        }
    } 
#endif

    byteRecvd = SocketUdpRecv(gOamUdpFd, buffer, MAX_UDP_OAM_DATA_BUFFER, &remoteAddr);
    if (byteRecvd >= LTE_OAM_DATA_REQ_HEAD_LEHGTH) {
        if (pOamDataReq->msgType == MSG_ULP_HEARTBEAT_REQ) {
            pUlpDataInd->msgType = MSG_ULP_HEARTBEAT_RESP;
            pUlpDataInd->length = LTE_ULP_DATA_IND_HEAD_LEHGTH;
            SocketUdpSend(gOamUdpFd, buffer, sizeof(LteUlpDataInd), &remoteAddr);  
            LOG_INFO(ULP_LOGGER_NAME, "Send Heartbeat response to OAM\n");
        } else {
            LOG_ERROR(ULP_LOGGER_NAME, "invalid msgType = %d\n", pOamDataReq->msgType);
            LOG_BUFFER(buffer, byteRecvd);
        }
    }
#else
    // TODO in DSP
#endif

    // handle UE estabilsh
    TgtSendUeEstablishInd(gOamUdpFd, &gDpeAddress);

    // handle UE identity
    int ueIndex = 0;
    RrcUeContext *pRrcUeCtx, *pNextRrcUeCtx;
    pUlpDataInd->msgType = MSG_ULP_UE_IDENTITY_IND;
    pUlpDataInd->length = LTE_ULP_DATA_IND_HEAD_LEHGTH + LTE_UE_ID_IND_MSG_HEAD_LEHGTH;

    UInt32 rrcCtxCount = ListCount(&gReadyRrcUeContextList);
    while (rrcCtxCount > 0) {
        pRrcUeCtx = (RrcUeContext*)ListPopNode(&gReadyRrcUeContextList);
        if (pRrcUeCtx == 0) {
            break;
        }
        if (pRrcUeCtx->ueIdentity.imsiPresent || pRrcUeCtx->ueIdentity.mTmsiPresent) {            
            LOG_INFO(ULP_LOGGER_NAME, "add ue data from gReadyRrcUeContextList, rnti = %d\n", pRrcUeCtx->rnti);
            memcpy((void*)&pUlpDataInd->u.ueIdentityInd.ueIdentityArray[ueIndex], (void*)&pRrcUeCtx->ueIdentity, sizeof(UeIdentity));
            pUlpDataInd->u.ueIdentityInd.ueIdentityArray[ueIndex].rnti = pRrcUeCtx->rnti;
            pUlpDataInd->length += sizeof(UeIdentity);
            MemFree(pRrcUeCtx);
            ueIndex++;
            if (ueIndex == MAX_NUM_UE_INFO_REPORT) {
                LOG_INFO(ULP_LOGGER_NAME, "1 reach max report UE count: %d\n", ueIndex);
                break;
            }
        }
        rrcCtxCount--;
    }

    rrcCtxCount = ListCount(&gRrcUeContextList);    
    if (rrcCtxCount > 0) {
        pRrcUeCtx = (RrcUeContext*)ListGetFirstNode(&gRrcUeContextList);
        while (pRrcUeCtx != 0) {
            pNextRrcUeCtx = (RrcUeContext*)ListGetNextNode(&pRrcUeCtx->node);
                
            if (ueIndex < MAX_NUM_UE_INFO_REPORT) {
                if ((pRrcUeCtx->ueIdentity.imsiPresent && pRrcUeCtx->ueIdentity.mTmsiPresent) 
                    || (pRrcUeCtx->idleCount >= gMaxRrcCtxIdleCount)) 
                {
                    pRrcUeCtx->deleteFlag = 1;
                    memcpy((void*)&pUlpDataInd->u.ueIdentityInd.ueIdentityArray[ueIndex], (void*)&pRrcUeCtx->ueIdentity, sizeof(UeIdentity));
                    pUlpDataInd->u.ueIdentityInd.ueIdentityArray[ueIndex].rnti = pRrcUeCtx->rnti;
                    pUlpDataInd->length += sizeof(UeIdentity);
                    RrcDeleteUeContext(pRrcUeCtx);
                    ueIndex++;
                }
            } else {
                LOG_INFO(ULP_LOGGER_NAME, "2 reach max report UE count: %d\n", ueIndex);
            }

            RrcUpdateUeContextTime(pRrcUeCtx, 1);

            pRrcUeCtx = pNextRrcUeCtx;
        }
    }

    if (ueIndex > 0) {
        pUlpDataInd->u.ueIdentityInd.count = ueIndex;
        if (ueIndex > 0) {
#ifdef OS_LINUX
            SocketUdpSend(gOamUdpFd, buffer, sizeof(LteUlpDataInd), &gOamAddress);  
#ifdef DPE 
            SocketUdpSend(gOamUdpFd, buffer, pUlpDataInd->length, &gDpeAddress);  
#endif
#endif
        }
    }
}

//---------------------------------
void* UlpWorkerEntryFunc(void* p)
{
    LOG_TRACE(ULP_LOGGER_NAME, "Entry\n");
    
#ifndef OS_LINUX
    UInt32 prevTime, curTime;
#else 
    struct timeval prevTv, curTv;
    UInt32 delta;
#endif

    while (1) {
        EventWait(&gUlpHandlerEvent);

#ifndef OS_LINUX
        prevTime = CSL_tscRead();
#else
        gettimeofday(&prevTv, 0);
#endif
        
        UlpRecvAndHandlePhyData();
        UlpRecvAndHandleOamData();

#ifndef OS_LINUX
        curTime = CSL_tscRead();
        if ((curTime - prevTime) > 10000) {
        	// only print when consuming time is more than 0.1ms
        	LOG_WARN(ULP_LOGGER_NAME, "end scheduling, time = %d\n", (curTime - prevTime));
        }
#else
        gettimeofday(&curTv, 0);

        if (curTv.tv_sec >= prevTv.tv_sec) {
            delta = (curTv.tv_sec - prevTv.tv_sec) * 1000000 + curTv.tv_usec - prevTv.tv_usec;
        } else {
            delta = (curTv.tv_sec + 60 - prevTv.tv_sec) * 1000000 + curTv.tv_usec - prevTv.tv_usec;
        }
#ifdef ARM_LINUX
        if (delta > 500) {
#else 
        if (delta > 1000) {
#endif
        	// only print when consuming time is more than 0.1ms
        	LOG_WARN(ULP_LOGGER_NAME, "end scheduling, delta = %d\n", delta);
        }
#endif
    }

    return 0;
}

