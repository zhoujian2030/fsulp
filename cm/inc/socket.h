/*
 * socket.h
 *
 *  Created on: June 2, 2018
 *      Author: j.zh
 */

#ifndef SOCKET_H
#define SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


int SocketClose(int fd);
void SocketGetSockaddrByIpAndPort(struct sockaddr_in* sockaddr, const char* ip, unsigned short port);
int SocketBind(unsigned short port, char* localIp);

// Init IPV4 UDP socket
int SocketUdpInitAndBind(unsigned short port, char* localIp);
int SocketUdpInit();

void SocketMakeNonBlocking(int fd);
void SocketMakeBlocking(int fd);

// Recv UDP data
// return :
//      -1 if error
//      num bytes received if success
//      0 if no data received
int SocketUdpRecv(int fd, char* pBuffer, int bufferSize, struct sockaddr_in* pRemoteAddr);

int SocketUdpSend(int fd, char* pBuffer, int numBytesToSend, struct sockaddr_in* pRemoteAddr);



#ifdef __cplusplus
}
#endif

#endif
