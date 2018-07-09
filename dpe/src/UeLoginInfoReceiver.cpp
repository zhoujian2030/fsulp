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

#define TIME_DIFF_IN_SECOND     20

// -------------------------------
UeLoginInfoReceiver::UeLoginInfoReceiver(DpEngineConfig* pDpeConfig) 
: Thread("UE Info Receiver"), m_pConfig(pDpeConfig), m_udpSocketFd(-1)
{
    m_udpSocketFd = SocketUdpInitAndBind(m_pConfig->m_imsiServerPort, (char*)m_pConfig->m_imsiServerIp.c_str());
    DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
    m_currentTargetId = 0;
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
        } else {
            LOG_MSG(LOGGER_MODULE_DPE, ERROR, "invalid data, byteRecvd = %d\n", byteRecvd);
            if (byteRecvd > 0) {
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
            DbInsertLoginImsi(&m_dbConn, (const char*)pUeIdentity->imsi);
        }
    }
}

#define MIN_TIME_DIFF_MILLI_SECOND  (TIME_DIFF_IN_SECOND * 1000 - 1000)
#define MAX_TIME_DIFF_MILLI_SECOND  (TIME_DIFF_IN_SECOND * 1000 + 1000)
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
    for (unsigned int i=0; i<pUeEstabInfoMsg->count; i++) {
        pUeEstabInfo = &pUeEstabInfoMsg->ueEstabInfoArray[i];

        if (!m_targetVect.empty()) {
            vectIt = m_targetVect.end() - 1;
            timeDiff = pUeEstabInfo->timestamp - (*vectIt).timestamp;
            if ((timeDiff >= MIN_TIME_DIFF_MILLI_SECOND) && (timeDiff <= MAX_TIME_DIFF_MILLI_SECOND)) {
                taDiff = pUeEstabInfo->ta - (*vectIt).ta;
                if ((taDiff >= -1) && (taDiff <= 1)) {
                    m_targetVect.push_back(*pUeEstabInfo);
                    LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************target: rnti = %d, prbPower = %d, ta = %d, timeDiff = %ld, size = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, timeDiff, m_targetVect.size());
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "drop this one, timeDiff = %ld, taDiff = %d\n", timeDiff, taDiff);
                    continue;
                }
                break;
            } else if (timeDiff < MIN_TIME_DIFF_MILLI_SECOND) {
                LOG_MSG(LOGGER_MODULE_DPE, TRACE, "drop this one, timeDiff = %ld\n", timeDiff);
                continue;
            } else {
                // clear state for next calc
                LOG_MSG(LOGGER_MODULE_DPE, INFO, "clear m_targetVect and m_potentialTargetMap, timeDiff = %ld\n", timeDiff);
                m_targetVect.clear();
                mapIt = m_potentialTargetMap.begin();
                while (mapIt != m_potentialTargetMap.end()) {
                    (mapIt->second).clear();
                    m_potentialTargetMap.erase(mapIt++);
                }
            }
        }

        for (mapIt=m_potentialTargetMap.begin(); mapIt != m_potentialTargetMap.end(); mapIt++) {
            vectIt = (mapIt->second).end() - 1;
            timeDiff = pUeEstabInfo->timestamp - (*vectIt).timestamp;
            if ((timeDiff >= MIN_TIME_DIFF_MILLI_SECOND) && (timeDiff <= MAX_TIME_DIFF_MILLI_SECOND)) {
                taDiff = pUeEstabInfo->ta - (*vectIt).ta;
                if ((taDiff >= -1) && (taDiff <= 1)) {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "potential target: rnti = %d, prbPower = %d, ta = %d\n", 
                        pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta);
                    // findPotentailTarget = true; 
                    (mapIt->second).push_back(*pUeEstabInfo);

                    // find target
                    if ((mapIt->second).size() == 3) {
                        findTarget = true;
                        m_targetVect.clear();
                        LOG_MSG(LOGGER_MODULE_DPE, INFO, "*****************find target, size = 3\n");
                        for (vectIt = (mapIt->second).begin(); vectIt != (mapIt->second).end(); vectIt++) {
                            LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "prbPower = %d, ta = %d\n", (*vectIt).prbPower, (*vectIt).ta);
                            m_targetVect.push_back(*vectIt);
                        }
                    }
                    break;
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, DEBUG, "timeDiff = %ld, taDiff = %d, compare next one\n", timeDiff, taDiff);
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
            LOG_MSG(LOGGER_MODULE_DPE, TRACE, "rnti = %d, prbPower = %d, ta = %d, m_currentTargetId = %d, timeDiff = %ld\n", 
                pUeEstabInfo->rnti, pUeEstabInfo->prbPower, pUeEstabInfo->ta, m_currentTargetId, timeDiff);

            // clear m_potentialTargetMap
            if (timeDiff > MAX_TIME_DIFF_MILLI_SECOND) {
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
