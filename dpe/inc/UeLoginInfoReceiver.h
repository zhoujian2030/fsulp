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
#include <map>
#include <vector>

namespace dpe {

    class DpEngineConfig;
    class EventIndicator;

    typedef LteUlpDataInd UeDataInd;
    typedef LteUlpDataInd DpeDataInd;
    #define MAX_UDP_RECV_BUFFER_LENGTH      MAX_UDP_OAM_DATA_BUFFER 
    #define MIN_UE_ULP_IND_MSG_LENGTH       (LTE_ULP_DATA_IND_HEAD_LEHGTH + LTE_UE_ID_IND_MSG_HEAD_LEHGTH)

    class UeLoginInfoReceiver : public Thread {
    public:
        UeLoginInfoReceiver(DpEngineConfig* pDpeConfig);
        virtual ~UeLoginInfoReceiver();

        void notify();

    protected:
        virtual unsigned long run();

    private:
        EventIndicator* m_pEvent;

        DpEngineConfig* m_pConfig;
        int m_udpSocketFd;
        DbConnection m_dbConn;

        struct sockaddr_in m_oamAddr;
        unsigned int m_currentTargetId;
        unsigned int m_missCount;
        unsigned int m_reportCount;
        std::string m_remoteIp;
        int m_maxTargetAccTimeInterval;
        int m_minTargetAccTimeInterval;
        std::map<unsigned int, std::vector<UeEstablishInfo> > m_potentialTargetMap;
        std::vector<UeEstablishInfo> m_targetVect;

        void saveUeIdentity(UeIdentityIndMsg* pUeIdentityMsg);
        void processUeEstablishInfo(UeEstablishIndMsg* pUeEstabInfoMsg, std::string remoteIp);
        void checkTarget();

        void reportTargetUe(int prbPower, std::string remoteIp);
    };

}

#endif 
