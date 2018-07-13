/*
 * UeLoginInfoReceiver.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "dpeCommon.h"
#include "UeLoginInfoReceiver.h"
#include "DpEngineConfig.h"
#include "logger.h"
#include "EventIndicator.h"

using namespace std;
using namespace dpe;

// -------------------------------
UeLoginInfoReceiver::UeLoginInfoReceiver(DpEngineConfig* pDpeConfig) 
: Thread("UE Info Receiver"), m_pConfig(pDpeConfig), m_udpSocketFd(-1)
{
    m_udpSocketFd = SocketUdpInitAndBind(m_pConfig->m_imsiServerPort, (char*)m_pConfig->m_imsiServerIp.c_str());
    SocketGetSockaddrByIpAndPort(&m_oamAddr, m_pConfig->m_oamServerIp.c_str(), m_pConfig->m_oamServerPort);
    SocketMakeNonBlocking(m_udpSocketFd);
    // DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
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

    UeLoginInfo* pUeLoginInfo;
    struct sockaddr_in remoteAddr;
    int byteRecvd;
    char recvBuffer[MAX_UDP_RECV_BUFFER_LENGTH];
    pUeLoginInfo = (UeLoginInfo*)recvBuffer;

    m_pEvent->reset();

    while (true) {
        m_pEvent->wait();

        checkTarget();

        byteRecvd = SocketUdpRecv(m_udpSocketFd, recvBuffer, MAX_UDP_RECV_BUFFER_LENGTH, &remoteAddr);
        if (byteRecvd > (int)MIN_UE_ULP_IND_MSG_LENGTH) {
            if (pUeLoginInfo->msgType == MSG_ULP_UE_IDENTITY_IND) {
                // LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "Recv MSG_ULP_UE_IDENTITY_IND, byteRecvd = %d\n", byteRecvd);
                saveUeIdentity(&pUeLoginInfo->u.ueIdentityInd);
            } else if (pUeLoginInfo->msgType == MSG_ULP_UE_ESTABLISH_IND) {
                LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "Recv MSG_ULP_UE_ESTABLISH_IND, byteRecvd = %d\n", byteRecvd);
                processUeEstablishInfo(&pUeLoginInfo->u.ueEstablishInd);
            } else {
                LOG_MSG(LOGGER_MODULE_DPE, ERROR, "invalid msgType = %d\n", pUeLoginInfo->msgType);
                LOG_MEM(ERROR, recvBuffer, byteRecvd);
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
            LOG_MSG(LOGGER_MODULE_DPE, TRACE, "imsi = %s\n", pUeIdentity->imsi);
            // DbInsertLoginImsi(&m_dbConn, (const char*)pUeIdentity->imsi);
        }
    }
}

// -------------------------------
void UeLoginInfoReceiver::reportTargetUe(int prbPower)
{
    LOG_MSG(LOGGER_MODULE_DPE, INFO, "************** prbPower = %d **************\n", prbPower);
    LteUlpDataInd targetUeInfo;
    targetUeInfo.msgType = MSG_ULP_TARGET_UE_INFO;
    targetUeInfo.u.targetUeInfoInd.prbPower = prbPower;
    targetUeInfo.length = sizeof(LteUlpDataInd);

    SocketUdpSend(m_udpSocketFd, (char*)&targetUeInfo, targetUeInfo.length, &m_oamAddr);
}

// -------------------------------
void UeLoginInfoReceiver::checkTarget()
{
    vector<UeEstablishInfo>::iterator it;
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    long timeDiff = 0;
    int randomVal;

    if (!m_targetVect.empty()) {
        m_reportCount++;
        int maxTimeDiff = m_maxTargetAccTimeInterval + m_missCount * (m_pConfig->m_targetAccTimeInterval + m_pConfig->m_targetAccTimeMargin/2);
        it = m_targetVect.end() - 1;
        timeDiff = timestamp - (*it).timestamp;

        if (timeDiff > maxTimeDiff) {
            m_missCount++;

            if ((m_missCount > (m_targetVect.size()/2)) || ((timeDiff > maxTimeDiff) && (m_missCount > 2))) {
                LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************lost target, timeDiff = %ld, m_missCount = %d\n", timeDiff, m_missCount);  
                reportTargetUe(0xffffffff);
                m_missCount = 0;
                m_targetVect.clear();
                map<unsigned int, vector<UeEstablishInfo> >::iterator mapIt = m_potentialTargetMap.begin();
                while (mapIt != m_potentialTargetMap.end()) {
                    (mapIt->second).clear();
                    m_potentialTargetMap.erase(mapIt++);
                }
            } else {
                LOG_MSG(LOGGER_MODULE_DPE, TRACE, "miss target, timeDiff = %ld, m_missCount = %d\n", timeDiff, m_missCount);
                reportTargetUe(((*it).prbPower - 5*m_missCount));
            }
        } else {
            if (m_reportCount % 20 == 0) {
                srand(timestamp);
                randomVal = 5 - (rand() % 10);
                reportTargetUe((*it).prbPower + randomVal);
            }
        }
    }
}


// -------------------------------
void UeLoginInfoReceiver::processUeEstablishInfo(UeEstablishIndMsg* pUeEstabInfoMsg)
{
    if (pUeEstabInfoMsg == 0 || pUeEstabInfoMsg->count == 0) {
        return;
    }

    UeEstablishInfo* pUeEstabInfo;
    map<unsigned int, vector<UeEstablishInfo> >::iterator mapIt;
    vector<UeEstablishInfo>::iterator vectIt;
    long timeDiff = 0;
    // bool findPotentailTarget = false;
    bool findTarget = false;
    short taDiff = 0;
    int prbPowerDiff = 0;

    for (unsigned int i=0; i<pUeEstabInfoMsg->count; i++) {
        pUeEstabInfo = &pUeEstabInfoMsg->ueEstabInfoArray[i];

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
                    m_targetVect.push_back(*pUeEstabInfo);
                    LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************target: rnti = %d, prbPower = %d, ta = %d, timeDiff = %ld, size = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, timeDiff, m_targetVect.size());
                    reportTargetUe(pUeEstabInfo->prbPower);
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "drop this one, rnti = %d, timeDiff = %ld, taDiff = %d, prbPower = %d\n", 
                        pUeEstabInfo->rnti, timeDiff, taDiff, pUeEstabInfo->prbPower);
                    continue;
                }
                break;
            } else if (timeDiff < minTimeDiff) {
                LOG_MSG(LOGGER_MODULE_DPE, TRACE, "drop this one, rnti= %d, timeDiff = %ld\n", pUeEstabInfo->rnti, timeDiff);
                continue;
            } else {
                m_missCount++;
                if ((m_missCount > (m_targetVect.size()/2)) || (m_missCount > 2)) {
                    // clear state for next calc
                    LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************lost target, timeDiff = %ld, m_missCount = %d\n", timeDiff, m_missCount);  
                    m_missCount = 0;
                    m_targetVect.clear();
                    mapIt = m_potentialTargetMap.begin();
                    while (mapIt != m_potentialTargetMap.end()) {
                        (mapIt->second).clear();
                        m_potentialTargetMap.erase(mapIt++);
                    }
                    reportTargetUe(0xffffffff);
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "drop this one, rnti = %d, timeDiff = %ld, m_missCount = %d\n", pUeEstabInfo->rnti, timeDiff, m_missCount);
                    reportTargetUe(((*vectIt).prbPower - 5*m_missCount));
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
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "potential target: rnti = %d, prbPower = %d, ta = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta);
                    pUeEstabInfo->timestamp = (*vectIt).timestamp + m_pConfig->m_targetAccTimeInterval;
                    (mapIt->second).push_back(*pUeEstabInfo);

                    // find target
                    if ((mapIt->second).size() == 3) {
                        findTarget = true;
                        m_missCount = 0;                        
                        m_targetVect.clear();
                        vectIt = (mapIt->second).begin();
                        LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************find target, size = 3, timeDiff = %lu, first timestamp = %lu, last timestamp = %lu\n", 
                            timeDiff, (*vectIt).timestamp, pUeEstabInfo->timestamp);
                        while (vectIt != (mapIt->second).end()) {
                            LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "prbPower = %d, ta = %d\n", (*vectIt).prbPower, (*vectIt).ta);
                            m_targetVect.push_back(*vectIt);
                            vectIt++;
                        }
                        reportTargetUe(pUeEstabInfo->prbPower);
                    }
                    break;
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "timeDiff = %ld, taDiff = %d, prbPowerDiff = %d, compare next one\n", timeDiff, taDiff, prbPowerDiff);
                }
            }            
        }

        if (findTarget) {            
            LOG_MSG(LOGGER_MODULE_DPE, INFO, "clear m_potentialTargetMap, timeDiff = %ld\n", timeDiff);
            mapIt = m_potentialTargetMap.begin();
            while (mapIt != m_potentialTargetMap.end()) {
                (mapIt->second).clear();
                m_potentialTargetMap.erase(mapIt++);
            }
            break;
        } else {
            m_currentTargetId++; 
            LOG_MSG(LOGGER_MODULE_DPE, TRACE, "rnti = %d, prbPower = %d, ta = %d, id = %d, timeDiff = %ld\n", 
                pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, m_currentTargetId, timeDiff);

            // clear m_potentialTargetMap
            if (timeDiff > m_maxTargetAccTimeInterval) {
                LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "clear m_potentialTargetMap as all data timeout, timeDiff = %ld\n", timeDiff);
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
