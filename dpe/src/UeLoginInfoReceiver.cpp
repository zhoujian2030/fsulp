/*
 * UeLoginInfoReceiver.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "dpeCommon.h"
#include "UeLoginInfoReceiver.h"
#include "DpEngineConfig.h"
#include "logger.h"
#include "EventIndicator.h"

using namespace std;
using namespace dpe;

#ifdef ENDIAN_CONVERT
#define CONVERT_INT16_ENDIAN(value) (((value & 0x00ff) << 8) | ((value & 0xff00) >> 8))
#define CONVERT_INT32_ENDIAN(value) (((value & 0xff000000) >> 24) | ((value & 0x00ff0000) >> 8) | ((value & 0x0000ff00) << 8) | ((value & 0xff) << 24))
#else
#define CONVERT_INT16_ENDIAN(value) (value)
#define CONVERT_INT32_ENDIAN(value) (value)
#endif

// -------------------------------
UeLoginInfoReceiver::UeLoginInfoReceiver(DpEngineConfig* pDpeConfig) 
: Thread("UE Info Receiver"), m_pConfig(pDpeConfig), m_udpSocketFd(-1)
{
    m_udpSocketFd = SocketUdpInitAndBind(m_pConfig->m_ueDataServerPort, (char*)m_pConfig->m_ueDataServerIp.c_str());
    SocketGetSockaddrByIpAndPort(&m_oamAddr, m_pConfig->m_oamServerIp.c_str(), m_pConfig->m_oamServerPort);
    SocketMakeNonBlocking(m_udpSocketFd);
#ifdef TARGET_ANALYSIS
    DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
#endif
    m_maxTargetAccTimeInterval = m_pConfig->m_targetAccTimeInterval + m_pConfig->m_targetAccTimeMargin;
    m_minTargetAccTimeInterval = m_pConfig->m_targetAccTimeInterval - m_pConfig->m_targetAccTimeMargin;
    m_currentTargetId = 0;
    m_missCount = 0;
    m_reportCount = 0;
    m_pEvent = new EventIndicator(false);
}

// -------------------------------
UeLoginInfoReceiver::~UeLoginInfoReceiver() 
{

}

// -------------------------------
void UeLoginInfoReceiver::notify()
{
    m_pEvent->set();
}

// -------------------------------
unsigned long UeLoginInfoReceiver::run()
{
    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "starting...\n");

    struct sockaddr_in remoteAddr;
    int byteRecvd;
    unsigned short msgType = 0;
    char buffer[MAX_UDP_RECV_BUFFER_LENGTH];
    UeDataInd* pUeDataInd = (UeDataInd*)buffer;
    DpeDataInd* pDpeDataInd = (DpeDataInd*)buffer;

    m_pEvent->reset();

    while (true) {
        m_pEvent->wait();

        checkTarget();

        byteRecvd = SocketUdpRecv(m_udpSocketFd, buffer, MAX_UDP_RECV_BUFFER_LENGTH, &remoteAddr);
        // LOG_MEM(ERROR, buffer, byteRecvd);
        if (byteRecvd >= (int)LTE_ULP_DATA_IND_HEAD_LEHGTH) {
            msgType = CONVERT_INT16_ENDIAN(*(unsigned short*)buffer);
            if (msgType == MSG_ULP_UE_IDENTITY_IND) {
                saveUeIdentity(&pUeDataInd->u.ueIdentityInd);
            } else if (msgType == MSG_ULP_UE_ESTABLISH_IND) {
                LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "Recv MSG_ULP_UE_ESTABLISH_IND, byteRecvd = %d\n", byteRecvd);
                processUeEstablishInfo(&pUeDataInd->u.ueEstablishInd, string(inet_ntoa(remoteAddr.sin_addr)));
            } else if (msgType == MSG_ULP_HEARTBEAT_REQ) {
                pDpeDataInd->msgType = MSG_ULP_HEARTBEAT_RESP;
                pDpeDataInd->length = LTE_ULP_DATA_IND_HEAD_LEHGTH;
                SocketUdpSend(m_udpSocketFd, buffer, sizeof(DpeDataInd), &remoteAddr);  
                // LOG_MSG(LOGGER_MODULE_DPE, TRACE, "Send heartbeat response\n");
            } else {
                LOG_MSG(LOGGER_MODULE_DPE, ERROR, "invalid msgType = %d\n", pUeDataInd->msgType);
                LOG_MEM(ERROR, buffer, byteRecvd);
            } 
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
    // LOG_MSG(LOGGER_MODULE_DPE, TRACE, "count = %d\n", pUeIdentityMsg->count);
    for (unsigned int i=0; i<pUeIdentityMsg->count; i++) {
        pUeIdentity = &pUeIdentityMsg->ueIdentityArray[i];
        // LOG_MSG(LOGGER_MODULE_DPE, TRACE, "imsiPresent = %d, mTmsiPresent = %d\n", pUeIdentity->imsiPresent, pUeIdentity->mTmsiPresent);

        if (pUeIdentity->imsiPresent && !pUeIdentity->mTmsiPresent) {
            pUeIdentity->imsi[15] = '\0';
#ifdef TARGET_ANALYSIS
            DbInsertLoginImsi(&m_dbConn, (const char*)pUeIdentity->imsi);
#else             
            LOG_MSG(LOGGER_MODULE_DPE, TRACE, "imsi = %s\n", pUeIdentity->imsi);
#endif
        }
    }
}

// -------------------------------
void UeLoginInfoReceiver::reportTargetUe(int prbPower, string remoteIp)
{
    LOG_MSG(LOGGER_MODULE_DPE, INFO, "************** prbPower = %d **************\n", prbPower);
    LteUlpDataInd targetUeInfo;
    targetUeInfo.msgType = MSG_ULP_TARGET_UE_INFO;
    targetUeInfo.u.targetUeInfoInd.prbPower = prbPower;
#ifdef PPC_LINUX
    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "remoteIp = %s\n", remoteIp.c_str());
    if (remoteIp.length() < 16) {
        memcpy(targetUeInfo.u.targetUeInfoInd.ip, remoteIp.c_str(), remoteIp.length());
    }
#endif
    targetUeInfo.length = sizeof(LteUlpDataInd);

    SocketUdpSend(m_udpSocketFd, (char*)&targetUeInfo, targetUeInfo.length, &m_oamAddr);
}

// -------------------------------
void UeLoginInfoReceiver::checkTarget()
{
    vector<UeEstablishInfo>::iterator it;
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long long timestamp = (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    int timeDiff = 0;
    int randomVal;

    if (!m_targetVect.empty()) {
        m_reportCount++;
        int maxTimeDiff = m_maxTargetAccTimeInterval + m_missCount * (m_pConfig->m_targetAccTimeInterval + m_pConfig->m_targetAccTimeMargin/2);
        it = m_targetVect.end() - 1;
        timeDiff = timestamp - (*it).timestamp;

        if (timeDiff > maxTimeDiff) {
            m_missCount++;

            if ((m_missCount > (m_targetVect.size()/2)) || ((timeDiff > maxTimeDiff) && (m_missCount > 2))) {
                LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************lost target, timeDiff = %d, m_missCount = %d\n", timeDiff, m_missCount);  
                reportTargetUe(0xffffffff, m_remoteIp);
                m_missCount = 0;
                m_targetVect.clear();
                map<unsigned int, vector<UeEstablishInfo> >::iterator mapIt = m_potentialTargetMap.begin();
                while (mapIt != m_potentialTargetMap.end()) {
                    (mapIt->second).clear();
                    m_potentialTargetMap.erase(mapIt++);
                }
            } else {
                LOG_MSG(LOGGER_MODULE_DPE, TRACE, "miss target, timeDiff = %d, m_missCount = %d\n", timeDiff, m_missCount);
                reportTargetUe(((*it).prbPower - 5*m_missCount), m_remoteIp);
            }
        } else {
            if (m_reportCount % 200 == 0) {
                srand(timestamp);
                randomVal = 5 - (rand() % 10);
                reportTargetUe((*it).prbPower + randomVal, m_remoteIp);
            }
        }
    }
}


// -------------------------------
void UeLoginInfoReceiver::processUeEstablishInfo(UeEstablishIndMsg* pUeEstabInfoMsg, string remoteIp)
{
    if (pUeEstabInfoMsg == 0 || pUeEstabInfoMsg->count == 0) {
        return;
    }

    pUeEstabInfoMsg->count = CONVERT_INT32_ENDIAN(pUeEstabInfoMsg->count);

    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "Recv from IP: %s, count = %d\n", remoteIp.c_str(), pUeEstabInfoMsg->count);

    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long long timestamp = (unsigned long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    UeEstablishInfo* pUeEstabInfo;
    map<unsigned int, vector<UeEstablishInfo> >::iterator mapIt;
    vector<UeEstablishInfo>::iterator vectIt;
    int timeDiff = 0;
    // bool findPotentailTarget = false;
    bool findTarget = false;
    short taDiff = 0;
    int prbPowerDiff = 0;

    for (unsigned int i=0; i<pUeEstabInfoMsg->count; i++) {
        pUeEstabInfo = &pUeEstabInfoMsg->ueEstabInfoArray[i];
        pUeEstabInfo->timestamp = timestamp;

        pUeEstabInfo->prbPower = CONVERT_INT32_ENDIAN(pUeEstabInfo->prbPower);
        pUeEstabInfo->rnti = CONVERT_INT16_ENDIAN(pUeEstabInfo->rnti);
        pUeEstabInfo->ta = CONVERT_INT16_ENDIAN(pUeEstabInfo->ta);

        if (!m_targetVect.empty()) {
            int maxTimeDiff = m_maxTargetAccTimeInterval + m_missCount * (m_pConfig->m_targetAccTimeInterval + m_pConfig->m_targetAccTimeMargin/4);
            int minTimeDiff = m_minTargetAccTimeInterval + m_missCount * (m_pConfig->m_targetAccTimeInterval - m_pConfig->m_targetAccTimeMargin/4);
            vectIt = m_targetVect.end() - 1;
            timeDiff = pUeEstabInfo->timestamp - (*vectIt).timestamp;
            if ((timeDiff >= minTimeDiff) && (timeDiff <= maxTimeDiff)) {
                taDiff = pUeEstabInfo->ta - (*vectIt).ta;
                // if ((taDiff >= -1) && (taDiff <= 1)) {
                if (pUeEstabInfo->prbPower != 0) {
                    pUeEstabInfo->timestamp = (*vectIt).timestamp + (m_missCount + 1) * m_pConfig->m_targetAccTimeInterval;
                    m_missCount = 0;
                    m_remoteIp = remoteIp;
                    m_targetVect.push_back(*pUeEstabInfo);
                    LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************target: rnti = %d, prbPower = %d, ta = %d, timeDiff = %d, size = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, timeDiff, m_targetVect.size());
                    reportTargetUe(pUeEstabInfo->prbPower, m_remoteIp);
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "drop this one, rnti = %d, timeDiff = %d, taDiff = %d, prbPower = %d\n", 
                        pUeEstabInfo->rnti, timeDiff, taDiff, pUeEstabInfo->prbPower);
                    continue;
                }
                break;
            } else if (timeDiff < minTimeDiff) {
                LOG_MSG(LOGGER_MODULE_DPE, TRACE, "drop this one, rnti= %d, timeDiff = %d\n", pUeEstabInfo->rnti, timeDiff);
                continue;
            } else {
                m_missCount++;
                if ((m_missCount > (m_targetVect.size()/2)) || (m_missCount > 2)) {
                    // clear state for next calc
                    LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************lost target, timeDiff = %d, m_missCount = %d\n", timeDiff, m_missCount);  
                    m_missCount = 0;
                    m_targetVect.clear();
                    mapIt = m_potentialTargetMap.begin();
                    while (mapIt != m_potentialTargetMap.end()) {
                        (mapIt->second).clear();
                        m_potentialTargetMap.erase(mapIt++);
                    }
                    reportTargetUe(0xffffffff, m_remoteIp);
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "drop this one, rnti = %d, timeDiff = %d, m_missCount = %d\n", pUeEstabInfo->rnti, timeDiff, m_missCount);
                    reportTargetUe(((*vectIt).prbPower - 5*m_missCount), m_remoteIp);
                    continue;
                }
            }
        }

        for (mapIt=m_potentialTargetMap.begin(); mapIt != m_potentialTargetMap.end(); mapIt++) {
            vectIt = (mapIt->second).end() - 1;
            timeDiff = pUeEstabInfo->timestamp - (*vectIt).timestamp;
            if ((timeDiff >= m_minTargetAccTimeInterval) && (timeDiff <= m_maxTargetAccTimeInterval)) {
                taDiff = pUeEstabInfo->ta - (*vectIt).ta;
                prbPowerDiff = pUeEstabInfo->prbPower - (*vectIt).prbPower;
                if ((taDiff >= -1) && (taDiff <= 1) && (prbPowerDiff >= -20) && (prbPowerDiff <= 20)) {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "potential target: rnti = %d, prbPower = %d, ta = %d, timeDiff = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, timeDiff);
                    pUeEstabInfo->timestamp = (*vectIt).timestamp + m_pConfig->m_targetAccTimeInterval;
                    (mapIt->second).push_back(*pUeEstabInfo);

                    // find target
                    if ((mapIt->second).size() == 3) {
                        findTarget = true;
                        m_missCount = 0;              
                        m_remoteIp = remoteIp;          
                        m_targetVect.clear();
                        vectIt = (mapIt->second).begin();
                        LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************find target, size = 3, timeDiff = %d, first timestamp = %llu, last timestamp = %llu\n", 
                            timeDiff, (*vectIt).timestamp, pUeEstabInfo->timestamp);
                        while (vectIt != (mapIt->second).end()) {
                            LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "prbPower = %d, ta = %d\n", (*vectIt).prbPower, (*vectIt).ta);
                            m_targetVect.push_back(*vectIt);
                            vectIt++;
                        }
                        reportTargetUe(pUeEstabInfo->prbPower, m_remoteIp);
                    }
                    break;
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "timeDiff = %d, taDiff = %d, prbPowerDiff = %d, compare next one\n", timeDiff, taDiff, prbPowerDiff);
                }
            }            
        }

        if (findTarget) {            
            LOG_MSG(LOGGER_MODULE_DPE, INFO, "clear m_potentialTargetMap, timeDiff = %d\n", timeDiff);
            mapIt = m_potentialTargetMap.begin();
            while (mapIt != m_potentialTargetMap.end()) {
                (mapIt->second).clear();
                m_potentialTargetMap.erase(mapIt++);
            }
            break;
        } else {
            m_currentTargetId++; 
            LOG_MSG(LOGGER_MODULE_DPE, TRACE, "rnti = %d, prbPower = %d, ta = %d, id = %d, timeDiff = %d\n", 
                pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, m_currentTargetId, timeDiff);

            // clear m_potentialTargetMap
            if (timeDiff > m_maxTargetAccTimeInterval) {
                LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "clear m_potentialTargetMap as all data timeout, timeDiff = %d\n", timeDiff);
                mapIt = m_potentialTargetMap.begin();
                while (mapIt != m_potentialTargetMap.end()) {
                    (mapIt->second).clear();
                    m_potentialTargetMap.erase(mapIt++);
                }
            }

            vector<UeEstablishInfo> pontentialTargetVect;
            pontentialTargetVect.push_back(*pUeEstabInfo);
            m_potentialTargetMap.insert(map<unsigned int, vector<UeEstablishInfo> >::value_type(m_currentTargetId, pontentialTargetVect));
        }
    }
}
