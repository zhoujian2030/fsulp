/*
 * UeLoginInfoReceiver.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include "dpeCommon.h"
#include "UeLoginInfoReceiver.h"
#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
UeLoginInfoReceiver::UeLoginInfoReceiver(DpEngineConfig* pDbeConfig) 
: Thread("DP Engine"), m_pConfig(pDbeConfig), m_udpSocketFd(-1)
{
    m_udpSocketFd = SocketUdpInitAndBind(m_pConfig->m_localUdpServerPort, (char*)m_pConfig->m_localIp.c_str());
    DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
}

// -------------------------------
UeLoginInfoReceiver::~UeLoginInfoReceiver() 
{

}

// -------------------------------
unsigned long UeLoginInfoReceiver::run()
{
    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "starting...\n");

    UeLoginInfo* pUeLoginInfo;
    struct sockaddr_in remoteAddr;
    int byteRecvd;
    char recvBuffer[MAX_UDP_RECV_BUFFER_LENGTH];
    pUeLoginInfo = (UeLoginInfo*)recvBuffer;

    while (true) {
        byteRecvd = SocketUdpRecv(m_udpSocketFd, recvBuffer, MAX_UDP_RECV_BUFFER_LENGTH, &remoteAddr);
        if (byteRecvd >= MIN_UE_LOGIN_INFO_MSG_LENGTH) {
            if (pUeLoginInfo->msgType == MSG_ULP_UE_IDENTITY_IND) {
                // LOG_MSG(LOGGER_MODULE_DPE, INFO, "Recv UE login info, byteRecvd = %d\n", byteRecvd);
                saveUeIdentity(&pUeLoginInfo->u.ueIdentityInd);
            } else {
                LOG_MSG(LOGGER_MODULE_DPE, ERROR, "invalid msgType = %d\n", pUeLoginInfo->msgType);
                LOG_MEM(ERROR, recvBuffer, byteRecvd);
            }
        } else {
            LOG_MSG(LOGGER_MODULE_DPE, ERROR, "invalid data, byteRecvd = %d\n", byteRecvd);
            LOG_MEM(ERROR, recvBuffer, byteRecvd);
        }
    }

    return 0;
}

// -------------------------------
void UeLoginInfoReceiver::saveUeIdentity(UeIdentityIndMsg* pUeIdentityMsg)
{
    if (pUeIdentityMsg == 0 || pUeIdentityMsg->count == 0) {
        return;
    }

    UeIdentity* pUeIdentity;
    for (unsigned int i=0; i<pUeIdentityMsg->count; i++) {
        pUeIdentity = &pUeIdentityMsg->ueIdentityArray[i];

        if (pUeIdentity->imsiPresent && !pUeIdentity->mTmsiPresent) {
            pUeIdentity->imsi[15] = '\0';
            DbInsertLoginImsi(&m_dbConn, (const char*)pUeIdentity->imsi);
        }
    }
}
