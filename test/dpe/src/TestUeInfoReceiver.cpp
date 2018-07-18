/*
 * TestUeInfoReceiver.cpp
 *
 *  Created on: July 09, 2018
 *      Author: j.zh
 */

#include "TestUeInfoReceiver.h"
#include <iostream>
#include <string>
#include "UeLoginInfoReceiver.h"
#include "DpEngineConfig.h"
#include "logger.h"
#include "socket.h"
#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace dpe;

// -------------------------------
TEST_F(TestUeInfoReceiver, processUeEstablishInfo)
{
    LoggerSetlevel(TRACE);

    DpEngineConfig* pDpeConfig = new DpEngineConfig();
    pDpeConfig->m_ueDataServerIp = "127.0.0.1";
    pDpeConfig->m_ueDataServerPort = 6666;
    UeLoginInfoReceiver* pUeInfoReceiver = new UeLoginInfoReceiver(pDpeConfig);
    pUeInfoReceiver->start();
    int socket = SocketUdpInit();
    sockaddr_in serverAddr;
    SocketGetSockaddrByIpAndPort(&serverAddr, pDpeConfig->m_ueDataServerIp.c_str(), pDpeConfig->m_ueDataServerPort);

    usleep(100);

    UeDataInd ueDataInd;
    unsigned long timestamp = 0;
    struct timeval tv;
    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 100;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 65;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);
    
    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 101;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 66;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(7);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(3);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(2);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(7);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(1);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(4);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);

    gettimeofday(&tv, 0);
    timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;    
    ueDataInd.msgType = MSG_ULP_UE_ESTABLISH_IND;
    ueDataInd.u.ueEstablishInd.count = 1;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].rnti = 102;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].prbPower = 67;
    ueDataInd.u.ueEstablishInd.ueEstabInfoArray[0].timestamp = timestamp;
    ueDataInd.length = MIN_UE_ULP_IND_MSG_LENGTH + sizeof(UeEstablishInfo);
    
    SocketUdpSend(socket, (char*)&ueDataInd, ueDataInd.length, &serverAddr);

    sleep(5);
}