/*
 * TestSocket.cpp
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#include "TestSocket.h"
#include "cJSON.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "file.h"
#include <string>
#include "socket.h"

using namespace std;

// -------------------------------
TEST_F(TestSocket, UDP_Socket_Blocking){
    int serverSocket = SocketUdpInitAndBind(6666, "0.0.0.0");
    int clientSocket = SocketUdpInitAndBind(6667, "0.0.0.0");
    ASSERT_TRUE(serverSocket != -1);
    ASSERT_TRUE(clientSocket != -1);

    sockaddr_in serverAddr;
    SocketGetSockaddrByIpAndPort(&serverAddr, "0.0.0.0", 6666);

    // send from client to server
    int bytesSent = SocketUdpSend(clientSocket, "hello", 5, &serverAddr);
    ASSERT_TRUE(bytesSent == 5);

    // server recv
    char recvBuffer[1024];
    sockaddr_in remoteAddr;
    int bytesRecvd = SocketUdpRecv(serverSocket, recvBuffer, 1024, &remoteAddr);
    ASSERT_TRUE(bytesRecvd == 5);
    ASSERT_TRUE(memcmp(recvBuffer, "hello", 5) == 0);
    ASSERT_EQ(remoteAddr.sin_port, htons(6667));

    // server send response to client
    bytesSent = SocketUdpSend(serverSocket, "hi", 2, &remoteAddr);
    ASSERT_TRUE(bytesSent == 2);

    // client recv response
    bytesRecvd = SocketUdpRecv(clientSocket, recvBuffer, 1024, &remoteAddr);
    ASSERT_TRUE(bytesRecvd == 2);
    ASSERT_TRUE(memcmp(recvBuffer, "hi", 2) == 0);
    ASSERT_EQ(remoteAddr.sin_port, htons(6666));
}

// -------------------------------
TEST_F(TestSocket, UDP_Socket_NonBlocking){
    int serverSocket = SocketUdpInitAndBind(6666, "0.0.0.0");
    SocketMakeNonBlocking(serverSocket);
    int clientSocket = SocketUdpInitAndBind(6667, "0.0.0.0");
    ASSERT_TRUE(serverSocket != -1);
    ASSERT_TRUE(clientSocket != -1);

    sockaddr_in serverAddr;
    SocketGetSockaddrByIpAndPort(&serverAddr, "0.0.0.0", 6666);

    // send from client to server
    int bytesSent = SocketUdpSend(clientSocket, "hello", 5, &serverAddr);
    ASSERT_TRUE(bytesSent == 5);
    // usleep(10);

    // server recv
    char recvBuffer[1024];
    sockaddr_in remoteAddr;
    int bytesRecvd = SocketUdpRecv(serverSocket, recvBuffer, 1024, &remoteAddr);
    ASSERT_TRUE(bytesRecvd == 5);
    ASSERT_TRUE(memcmp(recvBuffer, "hello", 5) == 0);
    ASSERT_EQ(remoteAddr.sin_port, htons(6667));

    bytesRecvd = SocketUdpRecv(serverSocket, recvBuffer, 1024, &remoteAddr);
    ASSERT_TRUE(bytesRecvd == 0);
}