/*
 * CLogger.h
 *
 *  Created on: Nov 01, 2016
 *      Author: j.zhou
 */
#ifndef C_LOGGER_H
#define C_LOGGER_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define ULP_LOGGER_NAME "ULP"
#define UE_LOGGER_NAME "UE"
#define KPI_LOGGER_NAME "KPI"
#define FILENAME /*lint -save -e613 */( NULL == strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '/')+1): strrchr(__FILE__, '\\')+1)
#define FUNCNAME __FUNCTION__
#define LINE     __LINE__

extern unsigned char gLogLevel;

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


#endif
