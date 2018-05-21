/*
 * lteLogger.c
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#include "lteLogger.h"
#include <stdio.h>
#include <stdarg.h>
#ifdef RUN_ON_STANDALONE_CORE
#include "event.h"
#include "thread.h"
#include "messaging.h"
#include <string.h>
#endif

void ProcessLogData();
void SendLogData(char* pData, unsigned int length);
int BuildLogItemData(LogData* pLogData, LogFormatData* pLogItem);

#ifndef OS_LINUX
#pragma DATA_SECTION(gLogLevel, ".ulpdata");
#pragma DATA_SECTION(gLogLevelName, ".ulpdata");
#endif
unsigned int gLogLevel = 1;
unsigned char gLogLevelName[LOG_LEVEL_ERROR + 1][LOG_LEVEL_LENGTH + 1] = {
        {"TRC"},
        {"DBG"},
        {"INF"},
        {"WAR"},
        {"ERR"}
};

// --------------------
#ifdef RUN_ON_STANDALONE_CORE

void* LogHandlerEntryFunc(void* p);

#define TASK_LOG_HANDLER_PRIORITY		2
#define TASK_LOG_HANDLER_STACK_SIZE		(64*1024)

#pragma DATA_SECTION(gTaskLogHandlerStack, ".ulpdata");
UInt8 gTaskLogHandlerStack[TASK_LOG_HANDLER_STACK_SIZE];


#pragma DATA_SECTION(gLogEvent, ".ulpdata");
Event gLogEvent;

#pragma DATA_SECTION(gSendLogMsgQ, ".ulpdata");
MessageQueue gSendLogMsgQ;

#pragma DATA_SECTION(gLogItems, ".ulpdata");
LogFormatData gLogItems[MAX_LOG_ITEM_NUM];
List gLogItemPool;
List gLogItemList;

#endif

// -------------------------------
void InitLogger()
{
#ifdef RUN_ON_STANDALONE_CORE
	EventInit(&gLogEvent);

	ListInit(&gLogItemPool, 1);
	ListInit(&gLogItemList, 1);

	unsigned int i;
	ListNode* pNode;
	for (i=0; i<MAX_LOG_ITEM_NUM; i++) {
		pNode = &gLogItems[i].node;
		ListPushNode(&gLogItemPool, pNode);
	}

	gSendLogMsgQ.qid = QMSS_TX_FREE_HAND_ULP_TO_OAM_LOG;

	ThreadHandle threadHandle;
	ThreadParams threadParams;
	threadParams.stackSize = TASK_LOG_HANDLER_STACK_SIZE;
	threadParams.stack = gTaskLogHandlerStack;
	threadParams.priority = TASK_LOG_HANDLER_PRIORITY;
	ThreadCreate((void*)LogHandlerEntryFunc, &threadHandle, &threadParams);
#endif
}

// -------------------------------
void LoggerSetLogLevel(unsigned int level)
{
	if (level > LOG_LEVEL_ERROR) {
		gLogLevel = LOG_LEVEL_ERROR;
	} else {
		gLogLevel = level;
	}
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

        ProcessLogData();
    }
}

// -------------------------------
void ProcessLogData()
{
	LogData logData;
	logData.logType = 1;
	logData.length = 0;
	LogFormatData* pLogItem;
	unsigned int count = ListCount(&gLogItemList);

	while (count > 0) {
		pLogItem = (LogFormatData*)ListPopNode(&gLogItemList);
		if (pLogItem == 0) {
			break;
		}

		if (0 == BuildLogItemData(&logData, pLogItem)) {
			// the log item is not written to buffer, save it back
			ListPushNodeHead(&gLogItemList, &pLogItem->node);
			break;
		}

		ListPushNode(&gLogItemPool, &pLogItem->node);

		count--;
	}

	if (logData.length > 0) {
		SendLogData((char*)&logData, logData.length + 4);
	}
}

#endif

// -------------------------------
void SendLogData(char* pData, unsigned int length)
{
#ifdef RUN_ON_STANDALONE_CORE
	MessageQSend(&gSendLogMsgQ, (char*)pData, length);
#else
	printf("%s\n", pData);
#endif
}

// -------------------------------
int BuildLogItemData(LogData* pLogData, LogFormatData* pLogItem)
{
	if ((pLogData->length + LOG_ITEM_HEAD_LENGTH) > MAX_LOG_DATA_SIZE) {
		// no enough space for this log item
		return 0;
	}

	static unsigned char logSeq = 0;

	char* ptr = pLogData->buffer + pLogData->length;
	int remainLen = MAX_LOG_DATA_SIZE - pLogData->length;
	int varDataLen;
	int logItemLen = 0;

	varDataLen = snprintf(ptr, remainLen, "%02x[%04d-%02d-%02d %02d:%02d:%02d:%03d]", logSeq, gSystemTime.year,
			gSystemTime.month, gSystemTime.day, gSystemTime.hour, gSystemTime.minute, gSystemTime.second, gSystemTime.millisecond);
	if(varDataLen > (remainLen-1)) {
		return 0;
	}
	remainLen -= varDataLen;
	logItemLen += varDataLen;
	ptr += varDataLen;

	*ptr++ = '[';
	remainLen--;
	logItemLen++;

	memcpy(ptr, gLogLevelName[pLogItem->logLevel], LOG_LEVEL_LENGTH);
	remainLen -= LOG_LEVEL_LENGTH;
	logItemLen += LOG_LEVEL_LENGTH;
	ptr += LOG_LEVEL_LENGTH;

	*ptr++ = ']';
	*ptr++ = ':';
	*ptr++ = ' ';
	remainLen -= 3;
	logItemLen += 3;

	varDataLen = snprintf(ptr, remainLen, "[%s], ", pLogItem->funcName);
	if(varDataLen > (remainLen-1)) {
		return 0;
	}
	remainLen -= varDataLen;
	logItemLen += varDataLen;
	ptr += varDataLen;

	varDataLen = snprintf(ptr, remainLen, (const char*)pLogItem->fmt,
			pLogItem->value[0], pLogItem->value[1], pLogItem->value[2],
			pLogItem->value[3], pLogItem->value[4], pLogItem->value[5]);
	if(varDataLen > (remainLen-1)) {
		return 0;
	}
	remainLen -= varDataLen;
	logItemLen += varDataLen;

	pLogData->length += logItemLen;

	return logItemLen;
}

// -------------------------------
int WriteLog(unsigned char moduleId, E_LogLevel eLogLevel, const char* funcName, const char *fmt,...)
{
	LogFormatData* pLogData = (LogFormatData*)ListPopNode(&gLogItemPool);
	if (pLogData == 0) {
		return -1;
	}

	unsigned int i;
	va_list args;

	pLogData->fmt = (char*)fmt;
	pLogData->funcName = (char*)funcName;
	pLogData->logLevel = eLogLevel;

	va_start(args, fmt);
	for (i=0; i<MAX_LOG_PARAM_NUM; i++) {
		// NOT support log string type when the string value is a local tmp var within function !!!!
		pLogData->value[i] = va_arg(args, unsigned int);
	}
	va_end(args);

	ListPushNode(&gLogItemList, &pLogData->node);

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


