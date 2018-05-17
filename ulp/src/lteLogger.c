/*
 * lteLogger.c
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#include "lteLogger.h"
//#include "list.h"
//#include "mempool.h"
#include "sync.h"
#include <stdio.h>
#include <stdarg.h>
#ifdef RUN_ON_STANDALONE_CORE
//#include "logger.h"
#include "event.h"
#include "thread.h"
#include "messaging.h"
#include <string.h>
#endif

void ProcessLogData(char* pData, unsigned int length);

 #define MAX_LOG_DATA_SIZE	(4096 - 4)
//#define MAX_LOG_DATA_SIZE	15
typedef struct
{
	unsigned short logType;
	unsigned short length;
	char buffer[MAX_LOG_DATA_SIZE];
} LogData;


#ifndef OS_LINUX
#pragma DATA_SECTION(gLogLevel, ".ulpdata");
#pragma DATA_SECTION(gLogSem, ".ulpdata");
#pragma DATA_SECTION(gLogData, ".ulpdata");
#endif
unsigned int gLogLevel = 1;
SEM_LOCK gLogSem;
LogData gLogData;
LogData gLogData2;
unsigned int gActiveLogDataIndex = 0;
LogData gLogDataArray[2];

#ifdef RUN_ON_STANDALONE_CORE

void* LogHandlerEntryFunc(void* p);

#define TASK_LOG_HANDLER_PRIORITY		2
#define TASK_LOG_HANDLER_STACK_SIZE	(64*1024)
#pragma DATA_SECTION(gTaskLogHandlerStack, ".ulpdata");
UInt8 gTaskLogHandlerStack[TASK_LOG_HANDLER_STACK_SIZE];


#pragma DATA_SECTION(gLogEvent, ".ulpdata");
Event gLogEvent;

#pragma DATA_SECTION(gSendLogMsgQ, ".ulpdata");
MessageQueue gSendLogMsgQ;

#endif

// -------------------------------
void InitLogger()
{
	SemInit(&gLogSem, 1);
	gLogData.length = 0;
	gLogData.logType = 1;

	gActiveLogDataIndex = 0;
	gLogDataArray[0].length = 0;
	gLogDataArray[0].logType = 1;
	gLogDataArray[1].length = 0;
	gLogDataArray[1].logType = 1;

#ifdef RUN_ON_STANDALONE_CORE
	EventInit(&gLogEvent);

	gSendLogMsgQ.qid = QMSS_TX_FREE_HAND_ULP_TO_OAM_LOG;

	ThreadHandle threadHandle;
	ThreadParams threadParams;
	threadParams.stackSize = TASK_LOG_HANDLER_STACK_SIZE;
	threadParams.stack = gTaskLogHandlerStack;
	threadParams.priority = TASK_LOG_HANDLER_PRIORITY;
	ThreadCreate((void*)LogHandlerEntryFunc, &threadHandle, &threadParams);
#endif
}

#ifdef RUN_ON_STANDALONE_CORE
// -------------------------------
void NotifyLogHandler()
{
	EventSend(&gLogEvent);
}

// ---------------------------------
void* LogHandlerEntryFunc(void* p)
{
    while (1) {
        EventWait(&gLogEvent);

        SendLogData();
    }
}

// -------------------------------
void SendLogData()
{
	if (gLogDataArray[gActiveLogDataIndex].length == 0) {
		return;
	}

	LogData* pLogData = &gLogDataArray[gActiveLogDataIndex];

	SemWait(&gLogSem);

	(gActiveLogDataIndex == 0) ? (gActiveLogDataIndex = 1) : (gActiveLogDataIndex = 0);
	gLogDataArray[gActiveLogDataIndex].length = 0;

	SemPost(&gLogSem);

	ProcessLogData((char*)pLogData, pLogData->length + 4);
}

#endif

// -------------------------------
void ProcessLogData(char* pData, unsigned int length)
{
#ifdef RUN_ON_STANDALONE_CORE
	MessageQSend(&gSendLogMsgQ, (char*)pData, length);
#else
	printf("%s\n", pData);
#endif
}

// -------------------------------
int WriteLog(unsigned char moduleId, E_LogLevel eLogLevel, const char *fmt,...)
{
	va_list args;
	va_start(args, fmt);

	SemWait(&gLogSem);

	LogData* pLogData = &gLogDataArray[gActiveLogDataIndex];

	char* pLogBuffer = pLogData->buffer + pLogData->length;
	int remainLen = MAX_LOG_DATA_SIZE - pLogData->length;
	if (remainLen > 0) {
		int varDataLen = vsnprintf(pLogBuffer, remainLen, fmt, args);
		if ((pLogData->length + varDataLen) <= MAX_LOG_DATA_SIZE) {
			pLogData->length += varDataLen;
		}
	}

	SemPost(&gLogSem);

	va_end(args);

	return 0;
}

// -------------------------------
// return num of bytes sent
int WriteBuffer(const char* pData, unsigned int dataLen)
{
#ifdef RUN_ON_STANDALONE_CORE

	unsigned char* pMsgQBuff = 0;
	unsigned int msgQBuffLen = 0;
	void* pFd = MessageQGetFreeTxFd(&gSendLogMsgQ, &pMsgQBuff, &msgQBuffLen);
	if (pFd == 0 || pMsgQBuff == 0 || msgQBuffLen == 0) {
		return 0;
	}

	if (dataLen > MAX_LOG_DATA_SIZE) {
		dataLen = MAX_LOG_DATA_SIZE;
	}

	LogData* pLogData = (LogData*)pMsgQBuff;
	pLogData->logType = 2;
	pLogData->length = dataLen;
	memcpy(pLogData->buffer, pData, dataLen);

	return MessageQSendByFd(&gSendLogMsgQ, pFd, dataLen + 4);

#else
	unsigned int i;
	for (i=0; i<dataLen; i++) {
		printf("%02x ", pBuffer[i]);
		if (i%10 == 9) {
			printf("\n");
		}
	}
	printf("\n");

	return dataLen;
#endif

}


