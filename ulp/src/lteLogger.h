/*
 * lteLogger.h
 *
 *  Created on: Apr 27, 2018
 *      Author: J.ZH
 */

#ifndef LTELOGGER_H_
#define LTELOGGER_H_

#ifdef OS_LINUX
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#else
#include "system.h"
#endif

extern unsigned int gLogLevel;

typedef enum
{
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DBG   = 1,
	LOG_LEVEL_INFO  = 2,
	LOG_LEVEL_WARN  = 3,
	LOG_LEVEL_ERROR = 4
}E_LogLevel;

#ifdef OS_LINUX

#define ULP_LOGGER_NAME "ULP"
#define FILENAME /*lint -save -e613 */( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)
#define FUNCNAME __FUNCTION__
#define LINE     __LINE__

#define LOG_TRACE(moduleId, fmt, args...){\
        if (gLogLevel == 0) {\
                (void)moduleId;\
                struct timeval tv;\
                gettimeofday(&tv, 0);\
                int ms = tv.tv_usec/1000;\
                struct tm * tmVal =  localtime(&tv.tv_sec);\
                char date[32];\
                strftime(date, sizeof(date),"%Y-%m-%d %H:%M:%S",tmVal);\
                printf("[%s.%d] [TRACE] [%3.3s] [%lu] [%s:%d] - ", date, ms, moduleId, pthread_self(), FILENAME,LINE);\
                printf(fmt,##args);\
        } }

#define LOG_DBG(moduleId, fmt, args...){\
        if (gLogLevel <= 1) {\
                (void)moduleId;\
                struct timeval tv;\
                gettimeofday(&tv, 0);\
                int ms = tv.tv_usec/1000;\
                struct tm * tmVal =  localtime(&tv.tv_sec);\
                char date[32];\
                strftime(date, sizeof(date),"%Y-%m-%d %H:%M:%S",tmVal);\
                printf("[%s.%d] [DEBUG] [%3.3s] [%lu] [%s:%d] - ", date, ms, moduleId, pthread_self(), FILENAME,LINE);\
                printf(fmt,##args);\
        } }

#define LOG_INFO(moduleId,fmt,args...){\
        if (gLogLevel <= 2) {\
                (void)moduleId;\
                struct timeval tv;\
                gettimeofday(&tv, 0);\
                int ms = tv.tv_usec/1000;\
                struct tm * tmVal =  localtime(&tv.tv_sec);\
                char date[32];\
                strftime(date, sizeof(date),"%Y-%m-%d %H:%M:%S",tmVal);\
                printf("[%s.%d] [INFO ] [%3.3s] [%lu] [%s:%d] - ", date, ms, moduleId, pthread_self(), FILENAME,LINE);\
                printf(fmt,##args);\
        } }
#define LOG_WARN(moduleId, fmt,args...){\
        if (gLogLevel <= 3) {\
                (void)moduleId;\
                struct timeval tv;\
                gettimeofday(&tv, 0);\
                int ms = tv.tv_usec/1000;\
                struct tm * tmVal =  localtime(&tv.tv_sec);\
                char date[32];\
                strftime(date, sizeof(date),"%Y-%m-%d %H:%M:%S",tmVal);\
                printf("[%s.%d] [WARN ] [%3.3s] [%lu] [%s:%d] - ", date, ms, moduleId, pthread_self(), FILENAME,LINE);\
                printf(fmt,##args);\
        } }
#define LOG_ERROR(moduleId, fmt, args...){\
        if (gLogLevel <= 4) {\
                (void)moduleId;\
                struct timeval tv;\
                gettimeofday(&tv, 0);\
                int ms = tv.tv_usec/1000;\
                struct tm * tmVal =  localtime(&tv.tv_sec);\
                char date[32];\
                strftime(date, sizeof(date),"%Y-%m-%d %H:%M:%S",tmVal);\
                printf("[%s.%d] [ERROR] [%3.3s] [%lu] [%s:%d] - ", date, ms, moduleId, pthread_self(), FILENAME,LINE);\
                printf(fmt,##args);\
        } }

#define LOG_BUFFER(pBuffer, length) {\
        if (gLogLevel <= 1) {\
            unsigned int i;\
            for (i=0; i<length; i++) {\
                printf("%02x ", pBuffer[i]);\
                if (i%10 == 9) {\
                    printf("\n");\
                }\
            }\
            printf("\n");\
        } }
#else

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

#endif





#endif /* LTELOGGER_H_ */
