/*
 * DpEngine.cpp
 *
 *  Created on: June 30, 2018
 *      Author: j.zhou
 */

#include "DpEngine.h"
#include "dpeCommon.h"
#include "DpEngineConfig.h"
#include "logger.h"

using namespace std;
using namespace dpe;

// -------------------------------
DpEngine::DpEngine(DpEngineConfig* pDpeConfig) 
: Thread("DP Engine"), m_pConfig(pDpeConfig)
{
    DbGetConnection(&m_dbConn, m_pConfig->m_mobileIdDbName.c_str());
}

// -------------------------------
DpEngine::~DpEngine() 
{

}

// -------------------------------
unsigned long DpEngine::run()
{
    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "starting...\n");

    while (true) {
        //
        Thread::sleep(1000);
    }

    return 0;
}

// -------------------------------
void DpEngine::handleUserRequest(DpRequest* pRequest)
{
    if (pRequest == 0) {
        LOG_MSG(LOGGER_MODULE_DPE, ERROR, "Null pointer pRequest\n");
        return;
    }

    switch (pRequest->msgType) {
        case DP_REQ_GET_UE_LOGIN_INFO_BY_IMSI_AND_DATE:
        {
            int numRecord = 0;
            DpGetUeLoginInfoReq* pGetLoginInfoReq = (DpGetUeLoginInfoReq*)&pRequest->u.getLoginInfoReq;
            string imsi(pGetLoginInfoReq->imsi);

            map<string, vector<string> >::iterator it = m_imsiLoginTimeMap.find(imsi);
            if (it != m_imsiLoginTimeMap.end()) {
                LOG_MSG(LOGGER_MODULE_DPE, WARNING, "imsi = %s exists in m_imsiLoginTimeMap\n", pGetLoginInfoReq->imsi);
                (it->second).clear();
                m_imsiLoginTimeMap.erase(it);
            }

            DbQueryLoginInfoByImsiAndDate(&m_dbConn, this, pGetLoginInfoReq->imsi, 
                pGetLoginInfoReq->startDate, pGetLoginInfoReq->endDate,
                DpEngine::queryLoginInfoByImsiCallback, &numRecord);
            if (numRecord > 0) {
                it = m_imsiLoginTimeMap.find(imsi);
                if (it != m_imsiLoginTimeMap.end() && (it->second).size() == numRecord) {
                    // process query result
                    analysisLoginBehavior(imsi, it->second);
                } else {
                    LOG_MSG(LOGGER_MODULE_DPE, ERROR, "unexpected error, numRecord = %d\n", numRecord);
                    if (it != m_imsiLoginTimeMap.end()) {
                        LOG_MSG(LOGGER_MODULE_DPE, ERROR, "vector size = %d\n", (it->second).size());
                        (it->second).clear();
                        m_imsiLoginTimeMap.erase(it);
                    }
                }
            }

            break;
        }

        default:
        {
            LOG_MSG(LOGGER_MODULE_DPE, ERROR, "unsupported msgType = %d\n", pRequest->msgType);
            break;
        }
    }
}

// -------------------------------
void DpEngine::analysisLoginBehavior(string imsi, vector<string> loginTime)
{
    vector<string>::iterator firstIt = loginTime.begin();
    vector<string>::iterator lastIt = loginTime.end() - 1;
    LOG_MSG(LOGGER_MODULE_DPE, INFO, "[%s]: %s ~ %s\n", imsi.c_str(), (*firstIt).c_str(), (*lastIt).c_str());
}

// -------------------------------
void DpEngine::preHandleQueryLoginInfoByImsi(unsigned int id, const char* imsi, unsigned int mTmsi, const char* timestamp)
{
    if (imsi == 0 || timestamp == 0) {
        LOG_MSG(LOGGER_MODULE_DPE, ERROR, "imsi = %p, timestamp = %p\n", imsi, timestamp);
        return;
    }

    LOG_MSG(LOGGER_MODULE_DPE, TRACE, "id = %d, imsi = %s, mTmsi = 0x%x, timestamp = %s\n", id, imsi, mTmsi, timestamp);

    string mapKeyImsi = string(imsi);
    map<string, vector<string> >::iterator it = m_imsiLoginTimeMap.find(imsi);
    if (it != m_imsiLoginTimeMap.end()) {
        (it->second).push_back(string(timestamp));
    } else {
        vector<string> loginTimeVect;
        loginTimeVect.push_back(string(timestamp));
        m_imsiLoginTimeMap.insert(map<string, vector<string> >::value_type(mapKeyImsi, loginTimeVect));
    }
}

// -------------------------------
void DpEngine::queryLoginInfoByImsiCallback(
    void* param, 
    unsigned int id, 
    const char* imsi, 
    unsigned int mTmsi, 
    const char* timestamp) 
{
    if (param != 0) {
        // DpEngine* pDpEngine = (DpEngine*)param;
        ((DpEngine*)param)->preHandleQueryLoginInfoByImsi(id, imsi, mTmsi, timestamp);
    }
}
