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

namespace dpe {

    class DpEngineConfig;

    class UeLoginInfoReceiver {
    public:
        UeLoginInfoReceiver(DpEngineConfig* pDbeConfig);
        ~UeLoginInfoReceiver();

    private:
        DpEngineConfig* m_pConfig;
        DbConnection m_dbConn;

        void saveUeIdentity(UeIdentityIndMsg* pUeIdentity);
    };

}

#endif 
