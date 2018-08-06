/*
 * loggerConfig.h
 *
 *  Created on: June 29, 2018
 *      Author: j.zh
 */

#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cJSON.h"

#define MAX_LOG_BLOCK_SIZE	(1024*16)

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
#define MAX_LOG_FILE_PATH_LENGTH	128
#define MAX_LOG_SERVER_IP_LENGTH	64
typedef struct {
	unsigned int  logLevel;	
	unsigned int  logType;

	unsigned char logToConsoleFlag;

	// logger file config
	unsigned char logToFileFlag;
	unsigned int  maxLogFileSize;
	char logFilePath[MAX_LOG_FILE_PATH_LENGTH];

	// logger socket config
	unsigned char logToSocketFlag;
	char logServerIp[MAX_LOG_SERVER_IP_LENGTH];
	unsigned short logServerPort;

	// log format config
	unsigned char logModuleNameFlag;
	unsigned char logFileNameFlag;
	unsigned char logFuncNameFlag;
	unsigned char logThreadIdFlag;

	// for async logging
	unsigned int asyncWaitTime;
	unsigned int logBufferingSize;	// for async log type 1, 0 if no buffering
} LoggerConfig;


void LoggerParseConfig(cJSON* jsonConfig, LoggerConfig* pLoggerConfig);


#ifdef __cplusplus
}
#endif

#endif 

