/*
 * UeLoginInfoReceiver.h
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#ifndef UE_LOGIN_INFO_RECEIVER_H
#define UE_LOGIN_INFO_RECEIVER_H

#include "lteUlpOamInterface.h"
#include "socket.h"
#include "dbInterface.h"
#include "Thread.h"

namespace dpe {

    class DpEngineConfig;

    typedef LteUlpDataInd UeLoginInfo;
    #define MAX_UDP_RECV_BUFFER_LENGTH      MAX_UDP_OAM_DATA_BUFFER 
    #define MIN_UE_LOGIN_INFO_MSG_LENGTH    (LTE_ULP_DATA_IND_HEAD_LEHGTH + LTE_UE_ID_IND_MSG_HEAD_LEHGTH + sizeof(UeIdentity))

    class UeLoginInfoReceiver : public Thread {
    public:
        UeLoginInfoReceiver(DpEngineConfig* pDbeConfig);
        virtual ~UeLoginInfoReceiver();

    protected:
        virtual unsigned long run();

    private:
        DpEngineConfig* m_pConfig;
        int m_udpSocketFd;
        DbConnection m_dbConn;

        void saveUeIdentity(UeIdentityIndMsg* pUeIdentityMsg);
    };

}

#endif 
