/*
 * target.h
 *
 *  Created on: July 06, 2018
 *      Author: j.zh
 */

#ifndef TARGET_H
#define TARGET_H 

#include "lteCommon.h"
#include "socket.h"

void TgtInit();

void TgtProcessUeEstablishInfo(UInt16 rnti, UlReportInfoList* pUlRptInfoList);

void TgtSendUeEstablishInd(int socketFd, struct sockaddr_in* pRemoteAddr);

#endif
