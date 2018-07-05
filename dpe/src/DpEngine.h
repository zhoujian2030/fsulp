/*
 * DpEngine.h
 *
 *  Created on: June 30, 2018
 *      Author: j.zhou
 */

#ifndef DP_ENGINE_H
#define DP_ENGINE_H

#include "Thread.h"
#include "dbInterface.h"
#include <vector>
#include <string>
#include <map>

namespace dpe {

    typedef enum {
        DP_REQ_GET_UE_LOGIN_INFO_BY_IMSI_AND_DATE   = 1,
        DP_REQ_GET_UE_LOGIN_INFO_BY_DATE            = 2,
        DP_REQ_DELETE_UE_LOGIN_INFO_BY_IMSI         = 3
    } EnumDpRequestType;

    typedef struct {
        char imsi[16];
        unsigned int mTmsi;
        char beginDate[MAX_DB_NAME_LENGTH];
        char endDate[MAX_DB_NAME_LENGTH];
    } DpGetUeLoginInfoReq;

    typedef struct {
        char imsi[16];
        unsigned int mTmsi;
        char timestamp[MAX_DB_NAME_LENGTH];
    } DpDeleteUeLoginInfoReq;

    typedef struct {
        unsigned int msgType;
        
        union {
            DpGetUeLoginInfoReq     getLoginInfoReq;
            DpDeleteUeLoginInfoReq  delLoginInfoReq;
        } u;
    } DpRequest;

    class DpEngineConfig;

    class DpEngine : public Thread {
    public:
        DpEngine(DpEngineConfig* pConfig);
        virtual ~DpEngine();

        void handleUserRequest(DpRequest* pRequest);

    protected:
        virtual unsigned long run();
        
    private:
        void analysisLoginBehavior(std::string imsi, std::vector<std::string> loginTime);

        void preHandleQueryLoginInfoByImsi(unsigned int id, const char* imsi, unsigned int mTmsi, const char* timestamp);

        static void queryLoginInfoByImsiCallback(
            void* param, 
            unsigned int id, 
            const char* imsi, 
            unsigned int mTmsi, 
            const char* timestamp);

        DpEngineConfig* m_pConfig;
        DbConnection m_loginDbConn;
        DbConnection m_userDbConn;

        std::map<std::string, std::vector<std::string> > m_imsiLoginTimeMap;
    };

}

#endif 
