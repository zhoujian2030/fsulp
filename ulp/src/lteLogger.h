/*
 * lteLogger.h
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#ifndef LTELOGGER_H_
#define LTELOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lteCommon.h"

#if defined OS_LINUX
#include "logger.h"
#elif defined INTEGRATE_PHY
#include "system.h"
#else
//#include "logInfoDef.h"
#include "list.h"
#endif

#define LOG_LEVEL_LENGTH		3

typedef enum
{
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DBG   = 1,
	LOG_LEVEL_INFO  = 2,
	LOG_LEVEL_WARN  = 3,
	LOG_LEVEL_ERROR = 4
}E_LogLevel;

extern unsigned int gLogLevel;

void InitLteLogger();
void LteLoggerSetLogLevel(unsigned int level);

// -------------------------------------------
#ifdef INTEGRATE_PHY

#define ULP_LOGGER_NAME gLogPrintfTypePtr

#define LOG_TRACE(logType, fmt, args...){\
	if(LOG_LEVEL_TRACE >= gLogLevel)\
		MyLogPrintf(logType, DEBUG, fmt, ##args);}

#define LOG_DBG(logType, fmt, args...){\
	if(LOG_LEVEL_DBG >= gLogLevel)\
		MyLogPrintf(logType, DEBUG, fmt, ##args);}

#define LOG_INFO(logType, fmt, args...){\
	if(LOG_LEVEL_INFO >= gLogLevel)\
		MyLogPrintf(logType, INFO, fmt, ##args);}

#define LOG_WARN(logType, fmt, args...){\
	if(LOG_LEVEL_WARN >= gLogLevel)\
		MyLogPrintf(logType, WANNING, fmt, ##args);}

#define LOG_ERROR(logType, fmt, args...){\
	if(LOG_LEVEL_ERROR >= gLogLevel)\
		MyLogPrintf(logType, ERROR, fmt, ##args);}

#define LOG_BUFFER(pData,length){\
        if(LOG_LEVEL_DBG >= gLogLevel && pData != 0){\
                unsigned int i=0;\
                unsigned int nLine = length/5;\
                unsigned int rest = 0;\
                while(nLine--) {\
                        MyLogPrintf(gLogPrintfTypePtr, DEBUG, "%02x %02x %02x %02x %02x\n", \
                                        pData[i], pData[i+1], pData[i+2], pData[i+3], pData[i+4]);\
                        i += 5;\
                }\
                rest = length - i;\
                if (rest == 1) {\
                        MyLogPrintf(gLogPrintfTypePtr, DEBUG, "%02x\n", pData[i]);\
                } else if (rest == 2) {\
                        MyLogPrintf(gLogPrintfTypePtr, DEBUG, "%02x %02x\n", pData[i], pData[i+1]);\
                } else if (rest == 3) {\
                        MyLogPrintf(gLogPrintfTypePtr, DEBUG, "%02x %02x %02x\n", pData[i], pData[i+1], pData[i+2]);\
                } else if (rest == 4) {\
                        MyLogPrintf(gLogPrintfTypePtr, DEBUG, "%02x %02x %02x %02x\n", pData[i], pData[i+1], pData[i+2], pData[i+3]);\
                }\
        }\
	}

// -----------------------------------------------
#elif defined OS_LINUX

#define ULP_LOGGER_NAME "ULP"

#define LOG_TRACE(moduleId, fmt, args...) \
        LoggerWriteMsg(moduleId, TRACE, FILENAME, __func__, fmt, ##args);

#define LOG_DBG(moduleId, fmt, args...) \
        LoggerWriteMsg(moduleId, DEBUG, FILENAME, __func__, fmt, ##args);

#define LOG_INFO(moduleId,fmt,args...) \
        LoggerWriteMsg(moduleId, INFO, FILENAME, __func__, fmt, ##args);

#define LOG_WARN(moduleId, fmt,args...) \
        LoggerWriteMsg(moduleId, WARNING, FILENAME, __func__, fmt, ##args);

#define LOG_ERROR(moduleId, fmt, args...) \
        LoggerWriteMsg(moduleId, ERROR, FILENAME, __func__, fmt, ##args);

#define LOG_BUFFER(pBuffer, length) \
        LoggerWriteMem(TRACE, (unsigned char*)pBuffer, length);

// ---------------------------------------------------
#elif defined TI_DSP

#define ULP_LOGGER_NAME 		0x12
#define FUNCNAME 				__FUNCTION__

#define TIMESTAMP_LENGTH		27
#define LOG_ITEM_HEAD_LENGTH	(TIMESTAMP_LENGTH + LOG_LEVEL_LENGTH + 4)
#define MAX_LOG_ITEM_LENGTH		256

#define MAX_LOG_DATA_SIZE		(4096 - 4)
//#define MAX_LOG_DATA_SIZE	15

typedef struct
{
	unsigned short logType;
	unsigned short length;
	char buffer[MAX_LOG_DATA_SIZE];
} LogData;

#define MAX_LOG_ITEM_NUM		1024
#define MAX_LOG_PARAM_NUM		6
typedef struct {
	ListNode node;
	char* fmt;
	char* funcName;
	unsigned int value[MAX_LOG_PARAM_NUM];
	unsigned char logLevel;
} LogFormatData;

int WriteLog(unsigned char moduleId, E_LogLevel eLogLevel, const char* funcName, const char *fmt,...);
int WriteBuffer(const char* pData, unsigned int dataLen);
void NotifyLogHandler();

#define LOG_TRACE(moduleId,fmt,args...){\
        if((unsigned int)LOG_LEVEL_TRACE >= gLogLevel)\
                WriteLog(moduleId,LOG_LEVEL_TRACE, FUNCNAME, fmt, ##args);}

#define LOG_DBG(moduleId,fmt,args...){\
        if((unsigned int)LOG_LEVEL_DBG >= gLogLevel)\
                WriteLog(moduleId,LOG_LEVEL_DBG, FUNCNAME, fmt, ##args);}

#define LOG_INFO(moduleId,fmt,args...){\
        if((unsigned int)LOG_LEVEL_INFO >= gLogLevel)\
                WriteLog(moduleId,LOG_LEVEL_INFO, FUNCNAME, fmt, ##args);}

#define LOG_WARN(moduleId,fmt,args...){\
        if((unsigned int)LOG_LEVEL_WARN >= gLogLevel)\
                WriteLog(moduleId,LOG_LEVEL_WARN, FUNCNAME, fmt, ##args);}

#define LOG_ERROR(moduleId,fmt,args...){\
        if((unsigned int)LOG_LEVEL_ERROR >= gLogLevel)\
                WriteLog(moduleId,LOG_LEVEL_ERROR, FUNCNAME, fmt, ##args);}

#define LOG_BUFFER(pData,dataLen){\
        if((unsigned int)LOG_LEVEL_TRACE >= gLogLevel)\
                WriteBuffer((const char*)pData,dataLen);}
#endif


#ifdef __cplusplus
}
#endif

#endif /* LTELOGGER_H_ */
