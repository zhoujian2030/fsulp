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

typedef enum 
{
	TRACE			= 0,		/* Detailed debug message */
	DEBUG			= 1,		/* Brief debug message */
	INFO			= 2,		/* Informational message */
	WARNING			= 3,		/* Non-critical, but important comment reveals possible future problems */
	ERROR			= 4,		/* The program cannot continue and will exit immediatelly */
	E_LOG_LVL_MAX	= 5,
}E_CMLogLevel;

#define MAX_LOG_FILE_PATH_LENGTH 128
typedef struct {
	unsigned int logLevel;
	unsigned char logToConsoleFlag;
	unsigned char logToFileFlag;
	unsigned int maxLogFileSize;
	char logFilePath[MAX_LOG_FILE_PATH_LENGTH];
} LoggerConfig;
typedef struct {
	FILE* fp;
	unsigned int logFileSize;
} LoggerStatus;

void LoggerSetlevel(int loglevel);
void LoggerUpdateConfig(LoggerConfig* pConfig);

void LoggerWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt,...);
void LoggerWriteMem(unsigned int logLevel, unsigned char* pBuffer, unsigned int length);

#define FILENAME /*lint -save -e613 */( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)
#define FUNCNAME __FUNCTION__
#define LINE     __LINE__

#define LOG_MSG(moduleId, eLogLevel, fmt, args...) \
    LoggerWriteMsg(moduleId, eLoglevel, FILENAME, __func__, fmt, ##args);

#define LOG_MEM(eLogLevel, pBuffer, length) \
	LoggerWriteMem(eLogLevel, pBuffer, length);

#ifdef __cplusplus
}
#endif

#endif 