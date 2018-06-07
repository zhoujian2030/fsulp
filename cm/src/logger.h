/*
 * logger.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include "queue.h"

typedef enum 
{
	TRACE			= 0,		/* Detailed debug message */
	DEBUG			= 1,		/* Brief debug message */
	INFO			= 2,		/* Informational message */
	WARNING			= 3,		/* Non-critical, but important comment reveals possible future problems */
	ERROR			= 4,		/* The program cannot continue and will exit immediatelly */
	E_LOG_LVL_MAX	= 5
}E_CMLogLevel;

typedef enum {
	SYNC_LOG		= 0,
	AYNC_LOG_TYPE_1 = 1,
	AYNC_LOG_TYPE_2 = 2,
	INVALID_LOG_TYPE = 3
} E_LogType;
#define MAX_LOG_FILE_PATH_LENGTH 128
typedef struct {
	unsigned int  logLevel;	
	unsigned int  logType;

	unsigned char logToConsoleFlag;

	// logger file config
	unsigned char logToFileFlag;
	unsigned int  maxLogFileSize;
	char logFilePath[MAX_LOG_FILE_PATH_LENGTH];

	// log format config
	unsigned char logModuleNameFlag;
	unsigned char logFileNameFlag;
	unsigned char logFuncNameFlag;
	unsigned char logThreadIdFlag;

	// for async logging
	unsigned int asyncWaitTime;
	unsigned int logBufferingSize;	// for async log type 1, 0 if no buffering
} LoggerConfig;

typedef struct {
	FILE* fp;
	unsigned int logFileSize;
} LoggerStatus;

#define MAX_LOG_BLOCK_SIZE	(1024*16)
typedef struct {
	void* next;	
	unsigned char fullFlag;
	unsigned int length;
	char logData[MAX_LOG_BLOCK_SIZE];
} LogBufferCache;

#define MAX_TIMESTAMP_LENGTH	32
#define MAX_LOG_CONTENT_LENGTH	256
#define MAX_LOG_MEMORY_LENGTH	2048
#define MAX_LOG_ARGS_NUM		6

typedef struct {
	//char timestamp[MAX_TIMESTAMP_LENGTH];
	char moduleId[6];
	int  logLevel;
	char* fileName;
	char* funcName;
	unsigned long threadId;

	unsigned char logContentFlag;

	char* fmt;
	unsigned int args[MAX_LOG_ARGS_NUM];

	char logContent[MAX_LOG_CONTENT_LENGTH];
} LogMsgInfo;

typedef struct {
	char logMem[MAX_LOG_MEMORY_LENGTH];
} LogMemInfo;

typedef enum {
	LOG_MSG_TYPE = 1,
	LOG_MEM_TYPE = 2
} E_LogContentType;

typedef struct {
	QNode node;
	// char timestamp[MAX_TIMESTAMP_LENGTH];
	// char moduleId[6];
	// int  logLevel;
	// char* fileName;
	// char* funcName;
	// unsigned long threadId;

	// unsigned char logContentFlag;

	// char* fmt;
	// unsigned int args[MAX_LOG_ARGS_NUM];

	// char logContent[MAX_LOG_CONTENT_LENGTH];
	E_LogContentType contentType;
	union {
		LogMsgInfo logMsgInfo;
		LogMemInfo logMemInfo;
	} u;
} LogInfo;

void LoggerSetlevel(int loglevel);
void LoggerInit();

void LoggerWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt,...);
void LoggerWriteMem(unsigned int logLevel, unsigned char* pBuffer, unsigned int length);

#define FILENAME /*lint -save -e613 */( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)
#define FUNCNAME __FUNCTION__
#define LINE     __LINE__
#define LOGGER_MODULE_CM	"CM"

#define LOG_MSG(moduleId, eLogLevel, fmt, args...) \
    LoggerWriteMsg(moduleId, eLogLevel, FILENAME, __func__, fmt, ##args);

#define LOG_MEM(eLogLevel, pBuffer, length) \
	LoggerWriteMem(eLogLevel, pBuffer, length);

#ifdef __cplusplus
}
#endif

#endif 
