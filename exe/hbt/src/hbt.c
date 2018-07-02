/*
 * heartbeatTester.c
 *
 *  Created on: July 2, 2018
 *      Author: j.zhou
 */

#include "hbt.h"
#include "thread.h"
#include "logger.h"
#include "socket.h"
#include "event.h"
#include "lteUlpOamInterface.h"

#define HB_LOCAL_IP         "127.0.0.1"
#define HB_LOCAL_UDP_PORT   6002

#define ULP_LOCAL_IP         "127.0.0.1"
#define ULP_LOCAL_UDP_PORT   6000

Event gHBEvent;
int gHBSocket = -1;
struct sockaddr_in gUlpAddress;

unsigned int gMaxHeartbeatReqSend = 0;
unsigned int gNumHeartbeatResp = 0;
unsigned int gNumHeartbeatReq = 0;
unsigned int gUlpDisconnecTime = 0; // in seconds

// ---------------------------------
void* HeartbeatTesterEntryFunc(void* p)
{
    LOG_MSG(HB_LOGGER_NAME, DEBUG, "Entry\n");

    char buffer[MAX_UDP_OAM_DATA_BUFFER];
    LteOamDataReq* pOamDataReq = (LteOamDataReq*)buffer;
    LteUlpDataInd* pUlpDataInd = (LteUlpDataInd*)buffer;
    int byteRecvd;
    struct sockaddr_in remoteAddr;

    while (1) {
        EventWait(&gHBEvent);

        byteRecvd = SocketUdpRecv(gHBSocket, buffer, MAX_UDP_OAM_DATA_BUFFER, &remoteAddr);
        if (byteRecvd >= LTE_ULP_DATA_IND_HEAD_LEHGTH) {
            if (pUlpDataInd->msgType == MSG_ULP_HEARTBEAT_RESP) {
                gNumHeartbeatResp++;
                LOG_MSG(HB_LOGGER_NAME, INFO, "Recv Heartbeat resp from ULP, heartbeatResp = %d, gNumHeartbeatReq = %d, gUlpDisconnecTime = %ds\n", 
                    gNumHeartbeatResp, gNumHeartbeatReq, gUlpDisconnecTime);
                gUlpDisconnecTime = 0;
                if (gNumHeartbeatReq >= gMaxHeartbeatReqSend) {
                    break;
                }
            } else {
                LOG_MSG(HB_LOGGER_NAME, ERROR, "invalid msgType = %d from ULP\n", pUlpDataInd->msgType);
                LOG_MEM(ERROR, buffer, byteRecvd);
            }
        }

        if (gUlpDisconnecTime > 6) {
            LOG_MSG(HB_LOGGER_NAME, ERROR, "miss heartbeat resp from ULP, gUlpDisconnecTime = %ds\n", gUlpDisconnecTime);
        }

        // send heartbeat req
        pOamDataReq->msgType = MSG_ULP_HEARTBEAT_REQ;
        pOamDataReq->length = LTE_OAM_DATA_REQ_HEAD_LEHGTH;
        SocketUdpSend(gHBSocket, buffer, pOamDataReq->length, &gUlpAddress);  
        gNumHeartbeatReq++;

        LOG_MSG(HB_LOGGER_NAME, DEBUG, "gNumHeartbeatReq = %d, gUlpDisconnecTime = %ds\n", gNumHeartbeatReq, gUlpDisconnecTime);
        
        gUlpDisconnecTime += 3;
    }

    return 0;
}

// --------------------------------
void NotifyHeartbeatTester()
{
    EventSend(&gHBEvent);
}

// --------------------------------
void StartHeartbeatTester()
{
    EventInit(&gHBEvent);
    gHBSocket = SocketUdpInitAndBind(HB_LOCAL_UDP_PORT, HB_LOCAL_IP);
    SocketMakeNonBlocking(gHBSocket);
    SocketGetSockaddrByIpAndPort(&gUlpAddress, ULP_LOCAL_IP, ULP_LOCAL_UDP_PORT);

    ThreadHandle threadHandle;
    ThreadParams threadParams;
    threadParams.priority = 89;
    threadParams.policy = RT_SCHED_RR;
    threadParams.stackSize = 0;
    ThreadCreate((void*)HeartbeatTesterEntryFunc, &threadHandle, &threadParams);

    LOG_MSG(HB_LOGGER_NAME, DEBUG, "Create Heartbeat Tester task\n");

    // void *status;
    // int result = pthread_join(threadHandle, &status);
    // if (0 != result) {
    //     LOG_MSG(HB_LOGGER_NAME, ERROR, "Error. return code from pthread_join is %d\n", result);
    // }
}
