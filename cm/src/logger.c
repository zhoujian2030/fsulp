/*
 * logger.c
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "logger.h"
#include <unistd.h>

static LoggerStatus gLoggerStatus_s = {
    0,
    0
};
static LoggerConfig gLoggerConfig_s = {
    TRACE,
    0,  // log type
    1,  // log to console flag

    0,  // log to file flag
    1024*1024*5,    // 5M bytes
    "",

    0,  // default not log module name
    0,  // log file name
    1,  // log function name
    1,  // log thread id

    5,      // async wait time
    1024*4, // default 4k bytes buffering
};

static char gLoggerLevelName_s[E_LOG_LVL_MAX][6] = {
        {"TRACE"},
        {"DEBUG"},
        {"INFO "},
        {"WARN "},
        {"ERROR"}
};
#define MAX_LOG_ITEM_LENGTH 4096

// --------------------------------
// for async logging
#ifdef RT_LOGGER
#define LOGGER_THREAD_PRIORITY      80
#endif

#define MAX_NUM_LOG_BUFFER_BLOCK    6
static LogBufferCache gLogBufferQueue[MAX_NUM_LOG_BUFFER_BLOCK];
static LogBufferCache* gpWriteBuffer = 0;
static LogBufferCache* gpReadBuffer = 0;
pthread_cond_t gLoggerCondition;
pthread_mutex_t gLoggerMutex;

#define MAX_NUM_LOG_INFO_NODE       4096
#define MAX_NUM_LOG_ITEM_TO_WRITE   32  // when count of gBusyLogInfoQueue reaches, notify logger
LogInfo gLogInfoNodeArray[MAX_NUM_LOG_INFO_NODE];
Queue gIdleLogInfoQueue;
Queue gBusyLogInfoQueue;
static void ProcessLogQueue();

static void InitAsyncLogger();
void* LoggerEntryFunc(void* p);
static void AsyncWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt, va_list args);
// --------------------------------

static void LoggerOutputLog(char* logBuff, unsigned length);
static void LoggerGetTimestamp(char* logBuf);

// -------------------------------------
void LoggerSetlevel(int loglevel)
{
    if (loglevel < TRACE) {
        gLoggerConfig_s.logLevel = TRACE;
    } else if (loglevel >= E_LOG_LVL_MAX) {
        gLoggerConfig_s.logLevel = ERROR;
    } else {
        gLoggerConfig_s.logLevel = loglevel;
    }
}

// -------------------------------------
void LoggerInit(LoggerConfig* pConfig)
{
    if (pConfig != 0) {
        memcpy((void*)&gLoggerConfig_s, (void*)pConfig, sizeof(LoggerConfig));
        if (gLoggerConfig_s.logToFileFlag) {
            if (gLoggerStatus_s.fp != 0) {
                fclose(gLoggerStatus_s.fp);
            }
            gLoggerStatus_s.logFileSize = 0;
            gLoggerStatus_s.fp = fopen(gLoggerConfig_s.logFilePath, "w+");
            if (gLoggerStatus_s.fp == 0) {
                gLoggerConfig_s.logToFileFlag = 0;
                printf("Fail to create log file : %s\n", gLoggerConfig_s.logFilePath);
                return;
            }
        }
    }

    if (gLoggerConfig_s.logType == SYNC_LOG) {
        if (pthread_mutex_init(&gLoggerMutex, 0) != 0) {
            printf("Fail to init pthread_mutex_t\n"); 
        }
    } else {
        InitAsyncLogger();
    }
}

// -------------------------------------
static void InitAsyncLogger()
{
    if (gLoggerConfig_s.logType != SYNC_LOG) {
        unsigned int i;
        int ret;

        if (gLoggerConfig_s.logType == AYNC_LOG_TYPE_1) {
            LogBufferCache* ptr = &gLogBufferQueue[0];
            for (i=0; i<MAX_NUM_LOG_BUFFER_BLOCK; i++) {
                gLogBufferQueue[i].fullFlag = 0;
                gLogBufferQueue[i].length = 0;

                if (i == 0) {
                    gpWriteBuffer = ptr;
                    gpReadBuffer = ptr;
                } else {
                    ptr->next = (void*)&gLogBufferQueue[i];
                    ptr = &gLogBufferQueue[i];
                }
            }
            ptr->next = (void*)gpWriteBuffer;
        } else {
            QueueInit(&gIdleLogInfoQueue);
            QueueInit(&gBusyLogInfoQueue);
            QNode* pNode;
            for (i=0; i<MAX_NUM_LOG_INFO_NODE; i++) {
                pNode = &gLogInfoNodeArray[i].node;
                QueuePushNode(&gIdleLogInfoQueue, pNode);
            }
        }

        ret = pthread_mutex_init(&gLoggerMutex, 0);
        if (ret == 0) {
            ret = pthread_cond_init(&gLoggerCondition, 0);
            if (ret != 0) {
                pthread_mutex_destroy(&gLoggerMutex);
                gLoggerConfig_s.logType = SYNC_LOG;
                printf("Fail to init pthread_cond_t\n"); 
                return;
            }
        } else {
            gLoggerConfig_s.logType = SYNC_LOG;
            printf("Fail to init pthread_mutex_t\n"); 
            return;
        }

        // create async thread
        pthread_attr_t attr;
        pthread_t pid;
        ret = pthread_attr_init(&attr);
        if (ret != 0) {
            printf("pthread_attr_init failure\n");
            return;
        }    

        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

#ifdef RT_LOGGER
        struct sched_param schedParam;
        schedParam.sched_priority = LOGGER_THREAD_PRIORITY;
        pthread_attr_setschedpolicy(&attr, SCHED_RR);
        pthread_attr_setschedparam(&attr, &schedParam);
#endif
        ret = pthread_create(&pid, &attr, LoggerEntryFunc, 0);
        if (ret != 0) {
            printf("pthread_create failure\n");
            return;
        }

        if (gLoggerConfig_s.logBufferingSize > MAX_LOG_BLOCK_SIZE) {
            gLoggerConfig_s.logBufferingSize = MAX_LOG_BLOCK_SIZE;
        }
    }
}

// -------------------------------------
void* LoggerEntryFunc(void* p)
{
    int ret;

    struct timespec outtime;
    struct timeval now;

    while (1) {
        ret = pthread_mutex_lock(&gLoggerMutex);
        if (ret != 0) {
            printf("Fail to lock on mutex\n");
            break;
        }
        
        // wait if current log buffer is not full
        while (((gLoggerConfig_s.logType == AYNC_LOG_TYPE_2) && (!QueueCount(&gBusyLogInfoQueue))) ||
            ((gLoggerConfig_s.logType == AYNC_LOG_TYPE_1) && (gpReadBuffer->length == 0))) {

            if (gLoggerConfig_s.asyncWaitTime > 0) {
                gettimeofday(&now, 0);
                now.tv_usec += gLoggerConfig_s.asyncWaitTime * 1000;
                if(now.tv_usec >= 1000000)
                {
                    now.tv_sec += now.tv_usec / 1000000;
                    now.tv_usec %= 1000000;
                }
                outtime.tv_nsec = now.tv_usec * 1000;
                outtime.tv_sec = now.tv_sec;

                pthread_cond_timedwait(&gLoggerCondition, &gLoggerMutex, &outtime);  
            } else {
                pthread_cond_wait(&gLoggerCondition, &gLoggerMutex);
            }
        }

        if (gLoggerConfig_s.logType == AYNC_LOG_TYPE_1) {
            // write to file
            // printf("gpReadBuffer = %p\n", gpReadBuffer);
            LoggerOutputLog(gpReadBuffer->logData, gpReadBuffer->length);
            gpReadBuffer->length = 0;
            // if full, the write ptr is moved to next buffer, so also move read ptr
            if (gpReadBuffer->fullFlag) {
                gpReadBuffer->fullFlag = 0;
                gpReadBuffer = (LogBufferCache*)gpReadBuffer->next;
            }
            pthread_mutex_unlock(&gLoggerMutex);
        } else {
            pthread_mutex_unlock(&gLoggerMutex);
            ProcessLogQueue();
        }
    }

    return 0;
}

// --------------------------------------
#define TIMESTAMP_SAMPLE                "1970-01-01 08:19:38.554"
#define TIMESTAMP_MILLI_SECOND_OFFSET   (strlen(TIMESTAMP_SAMPLE) - 3)
#define TIMESTAMP_SECOND_OFFSET         (strlen(TIMESTAMP_SAMPLE) - 6)
static char gCachedTimeStamp[MAX_TIMESTAMP_LENGTH];
static __time_t gPrevSysSecond = 0;
static unsigned int gPrevSysMilliSecond = 0;
static void LoggerGetTimestamp(char* logBuff)
{
    if (logBuff == 0) {
        return;
    }

    int offset = 0;
    char* logPtr = logBuff;
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned int currMilliSecond = tv.tv_usec/1000;

    if ((gPrevSysSecond == tv.tv_sec) && (gPrevSysMilliSecond == currMilliSecond)) {
        // timestamp is not changed
        // snprintf(logBuff, MAX_TIMESTAMP_LENGTH + 3, "[%s] ", gCachedTimeStamp);
        *logPtr++ = '[';
        offset = strlen(gCachedTimeStamp);
        memcpy(logPtr, gCachedTimeStamp, offset);
        logPtr += offset;
        *logPtr++ = ']';
        *logPtr++ = ' ';
        *logPtr = '\0';
    } else {    
        // when async logging type 1, the memory is protected outside
        if (gLoggerConfig_s.logType != AYNC_LOG_TYPE_1) {
            pthread_mutex_lock(&gLoggerMutex);
        }

        if (gPrevSysSecond == 0) {
            // timestamp is not initialized yet
            strftime(gCachedTimeStamp, MAX_TIMESTAMP_LENGTH, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
            offset = strlen(gCachedTimeStamp);
            snprintf(gCachedTimeStamp + offset, MAX_TIMESTAMP_LENGTH - offset, ".%03d", currMilliSecond);
        } else {
            if (gPrevSysSecond == tv.tv_sec) {            
                // update millisecond
                snprintf(gCachedTimeStamp + TIMESTAMP_MILLI_SECOND_OFFSET, MAX_TIMESTAMP_LENGTH - TIMESTAMP_MILLI_SECOND_OFFSET, "%03d", currMilliSecond);
            } else {
                // second is changed
                __time_t prevSysMin = gPrevSysSecond/60;
                __time_t currSysMin = tv.tv_sec/60;
                if (prevSysMin == currSysMin) {
                    // update second and millisecond
                    snprintf(gCachedTimeStamp + TIMESTAMP_SECOND_OFFSET, MAX_TIMESTAMP_LENGTH - TIMESTAMP_SECOND_OFFSET, "%02d.%03d", (int)tv.tv_sec%60, currMilliSecond);
                } else {
                    // update the whole timestamp
                    strftime(gCachedTimeStamp, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
                    offset = strlen(gCachedTimeStamp);
                    snprintf(gCachedTimeStamp + offset, MAX_TIMESTAMP_LENGTH - offset, ".%03d", currMilliSecond);
                }
            }
        }

        gPrevSysSecond = tv.tv_sec;
        gPrevSysMilliSecond = currMilliSecond;

        // snprintf(logBuff, MAX_TIMESTAMP_LENGTH + 3, "[%s] ", gCachedTimeStamp);
        *logPtr++ = '[';
        offset = strlen(gCachedTimeStamp);
        memcpy(logPtr, gCachedTimeStamp, offset);
        logPtr += offset;
        *logPtr++ = ']';
        *logPtr++ = ' ';
        *logPtr = '\0';

        if (gLoggerConfig_s.logType != AYNC_LOG_TYPE_1) {
            pthread_mutex_unlock(&gLoggerMutex);
        }      
    }
}

// -------------------------------------
void LoggerWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt,...)
{
    if ((logLevel >= gLoggerConfig_s.logLevel) && (logLevel < E_LOG_LVL_MAX)) {
        char logBuff[MAX_LOG_ITEM_LENGTH] = { };
        int offset = 0, length = 0;
        char* logPtr;
        va_list args;

        if (gLoggerConfig_s.logType != SYNC_LOG) {
            va_start(args, fmt);
            if (gLoggerConfig_s.logType == AYNC_LOG_TYPE_1) {
                AsyncWriteMsg(moduleId, logLevel, fileName, funcName, fmt, args);
            } else {
                LogInfo* pLogInfoNode = (LogInfo*)QueuePopNode(&gIdleLogInfoQueue);
                if (pLogInfoNode == 0) {               
                    va_end(args);
                    pthread_mutex_lock(&gLoggerMutex);
                    pthread_cond_signal(&gLoggerCondition);
                    pthread_mutex_unlock(&gLoggerMutex); 
                    // printf("no idle log item, drop this log\n");    
                    return;
                }

                pLogInfoNode->contentType = LOG_MSG_TYPE;
                // LoggerGetTimestamp(pLogInfoNode->timestamp);
                pLogInfoNode->u.logMsgInfo.logLevel = logLevel;
                if (gLoggerConfig_s.logModuleNameFlag) {
                    memcpy(pLogInfoNode->u.logMsgInfo.moduleId, moduleId, 6);
                }
                pLogInfoNode->u.logMsgInfo.fileName = (char*)fileName;
                pLogInfoNode->u.logMsgInfo.funcName = (char*)funcName;
                pLogInfoNode->u.logMsgInfo.threadId = pthread_self();
                int length = strlen(fmt);
                int i = 0, n = 0, s = 0;
                while (i < length) {
                    if (fmt[i++] == '%') {
                        n++;
                        if (n > MAX_LOG_ARGS_NUM) {
                            break;
                        }

                        if (fmt[i++] == 's') {
                            s++;
                            break;
                        }
                    }
                }

                if ((n > MAX_LOG_ARGS_NUM) || (s > 0)) {
                    vsnprintf(pLogInfoNode->u.logMsgInfo.logContent, MAX_LOG_CONTENT_LENGTH, fmt, args);
                    pLogInfoNode->u.logMsgInfo.logContentFlag = 1;
                    // printf("logContentFlag = 1\n");
                } else {
                    for (i=0; i<n; i++) {
                        pLogInfoNode->u.logMsgInfo.args[i] = va_arg(args, unsigned int);
                    }
                    pLogInfoNode->u.logMsgInfo.logContentFlag = 0;
                    pLogInfoNode->u.logMsgInfo.fmt = (char*)fmt;
                }
                va_end(args);

                QueuePushNode(&gBusyLogInfoQueue, &pLogInfoNode->node);
            }
        } else {
            logPtr = logBuff;

            LoggerGetTimestamp(logPtr);
            length = strlen(logPtr);
            logPtr += length;
            offset += length;

            *logPtr++ = '[';
            length = strlen(gLoggerLevelName_s[logLevel]);
            memcpy(logPtr, gLoggerLevelName_s[logLevel], length);
            logPtr += length;
            *logPtr++ = ']';
            *logPtr++ = ' ';            
            offset = offset + length + 3;

            if (gLoggerConfig_s.logModuleNameFlag) {
                *logPtr++ = '[';
                length = strlen(moduleId);
                memcpy(logPtr, moduleId, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ' ';            
                offset = offset + length + 3;
            }

            if (gLoggerConfig_s.logThreadIdFlag) {
                snprintf(logPtr, MAX_LOG_ITEM_LENGTH - offset, "[%lu] ", pthread_self());
                length = strlen(logPtr);
                logPtr += length;          
                offset += length;
            }
            
            if (gLoggerConfig_s.logFileNameFlag) {
                *logPtr++ = '[';
                length = strlen(fileName);
                memcpy(logPtr, fileName, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ' ';            
                offset = offset + length + 3;
            }

            *logPtr++ = '-';
            *logPtr++ = ' ';
            offset += 2;

            if (gLoggerConfig_s.logFuncNameFlag) {
                *logPtr++ = '[';
                length = strlen(funcName);
                memcpy(logPtr, funcName, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ',';
                *logPtr++ = ' ';            
                offset = offset + length + 4;
            }     

            va_start(args, fmt);
            vsnprintf(logPtr, MAX_LOG_ITEM_LENGTH - offset, fmt, args);
            va_end(args);
            length = strlen(logPtr);
            logPtr += length;
            offset += length;
            *logPtr = '\0';

            // offset = strlen(logPtr);
            // if ((offset >= 1) && (logPtr[offset - 1] != '\n')) {
            //     if (offset < MAX_LOG_ITEM_LENGTH - 1)
            //     {
            //         logPtr[offset] = '\n';
            //         logPtr[offset + 1] = '\0';
            //     }
            //     else
            //     {
            //         logPtr[offset - 1] = '\n';
            //         logPtr[offset] = '\0';
            //     }
            // }

            LoggerOutputLog(logBuff, strlen(logBuff));
        }
    }
}

// --------------------------------------
static void AsyncWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt, va_list args)
{
    int offset = 0, length = 0;
    unsigned char fullFlag = 0;
    // unsigned char dropFlag = 0;
    char* logPtr;

    pthread_mutex_lock(&gLoggerMutex);

    if (gpWriteBuffer->fullFlag) {
        // gpWriteBuffer points to gpReadBuffer
        // fullFlag = 1;
        // dropFlag = 1;
        printf("1 No available log buffer, drop this log, gpWriteBuffer = %p\n", gpWriteBuffer);
        pthread_cond_signal(&gLoggerCondition);
        pthread_mutex_unlock(&gLoggerMutex);
        return;
    } else if ((gLoggerConfig_s.logBufferingSize != 0) && (gpWriteBuffer->length != 0) &&
        ( (MAX_TIMESTAMP_LENGTH + gpWriteBuffer->length + 128) >= gLoggerConfig_s.logBufferingSize )) {
        fullFlag = 1;
        gpWriteBuffer->fullFlag = 1;
        gpWriteBuffer = (LogBufferCache*)gpWriteBuffer->next;
        if (gpWriteBuffer->fullFlag) {
            // dropFlag = 1;
            printf("2 No available log buffer, drop this log, gpWriteBuffer = %p\n", gpWriteBuffer);
            pthread_cond_signal(&gLoggerCondition);
            pthread_mutex_unlock(&gLoggerMutex);
            return;
        }
    }

    logPtr = gpWriteBuffer->logData + gpWriteBuffer->length;
    int remainBufferSize = gLoggerConfig_s.logBufferingSize - gpWriteBuffer->length;

    LoggerGetTimestamp(logPtr);
    length = strlen(logPtr);
    logPtr += length;
    offset += length;

    *logPtr++ = '[';
    length = strlen(gLoggerLevelName_s[logLevel]);
    memcpy(logPtr, gLoggerLevelName_s[logLevel], length);
    logPtr += length;
    *logPtr++ = ']';
    *logPtr++ = ' ';            
    offset = offset + length + 3;

    if (gLoggerConfig_s.logModuleNameFlag) {
        *logPtr++ = '[';
        length = strlen(moduleId);
        memcpy(logPtr, moduleId, length);
        logPtr += length;
        *logPtr++ = ']';
        *logPtr++ = ' ';            
        offset = offset + length + 3;
    }

    if (gLoggerConfig_s.logThreadIdFlag) {
        snprintf(logPtr, remainBufferSize - offset, "[%lu] ", pthread_self());
        length = strlen(logPtr);
        logPtr += length;          
        offset += length;
    }
    
    if (gLoggerConfig_s.logFileNameFlag) {
        *logPtr++ = '[';
        length = strlen(fileName);
        memcpy(logPtr, fileName, length);
        logPtr += length;
        *logPtr++ = ']';
        *logPtr++ = ' ';            
        offset = offset + length + 3;
    }

    *logPtr++ = '-';
    *logPtr++ = ' ';
    offset += 2;

    if (gLoggerConfig_s.logFuncNameFlag) {
        *logPtr++ = '[';
        length = strlen(funcName);
        memcpy(logPtr, funcName, length);
        logPtr += length;
        *logPtr++ = ']';
        *logPtr++ = ',';
        *logPtr++ = ' ';            
        offset = offset + length + 4;
    }     

    vsnprintf(logPtr, remainBufferSize - offset, fmt, args);
    length = strlen(logPtr);
    logPtr += length;
    offset += length;

    // offset = strlen(logPtr);
    // if ((offset >= 1) && (logPtr[offset - 1] != '\n')) {
    //     if (offset < MAX_LOG_ITEM_LENGTH - 1)
    //     {
    //         logPtr[offset] = '\n';
    //         logPtr[offset + 1] = '\0';
    //     }
    //     else
    //     {
    //         logPtr[offset - 1] = '\n';
    //         logPtr[offset] = '\0';
    //     }
    // }

    gpWriteBuffer->length += offset;
    if (gpWriteBuffer->length >= gLoggerConfig_s.logBufferingSize) {
        fullFlag = 1;
        gpWriteBuffer->fullFlag = 1;
        gpWriteBuffer = (LogBufferCache*)gpWriteBuffer->next;
    }

    if (fullFlag) {
        pthread_cond_signal(&gLoggerCondition);
    }

    pthread_mutex_unlock(&gLoggerMutex);
}

// --------------------------------------
static void ProcessLogQueue()
{
    LogInfo* pLogInfoNode;
    char logBuff[4096] = {0};
    char* logPtr = logBuff;
    char timestamp[MAX_TIMESTAMP_LENGTH];
    int remainBufferSize = 4095;
    int offset = 0;
    int length = 0;
    unsigned int count = QueueCount(&gBusyLogInfoQueue);
    LoggerGetTimestamp(timestamp);

    // printf("count = %d\n", count);
    while (count > 0) {
        pLogInfoNode = (LogInfo*)QueuePopNode(&gBusyLogInfoQueue);
        if (pLogInfoNode == 0) {
            break;
        }

        if (pLogInfoNode->contentType == LOG_MSG_TYPE) {
            length = strlen(timestamp);
            memcpy(logPtr, timestamp, length);
            logPtr += length;
            offset += length; 
            
            *logPtr++ = '[';
            length = strlen(gLoggerLevelName_s[pLogInfoNode->u.logMsgInfo.logLevel]);
            memcpy(logPtr, gLoggerLevelName_s[pLogInfoNode->u.logMsgInfo.logLevel], length);
            logPtr += length;
            *logPtr++ = ']';
            *logPtr++ = ' ';            
            offset = offset + length + 3;

            if (gLoggerConfig_s.logModuleNameFlag) {
                *logPtr++ = '[';
                length = strlen(pLogInfoNode->u.logMsgInfo.moduleId);
                memcpy(logPtr, pLogInfoNode->u.logMsgInfo.moduleId, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ' ';            
                offset = offset + length + 3;
            }

            if (gLoggerConfig_s.logThreadIdFlag) {
                snprintf(logPtr, remainBufferSize - offset, "[%lu] ", pLogInfoNode->u.logMsgInfo.threadId);
                length = strlen(logPtr);
                logPtr += length;          
                offset += length;
            }
            
            if (gLoggerConfig_s.logFileNameFlag) {
                *logPtr++ = '[';
                length = strlen(pLogInfoNode->u.logMsgInfo.fileName);
                memcpy(logPtr, pLogInfoNode->u.logMsgInfo.fileName, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ' ';            
                offset = offset + length + 3;
            }

            *logPtr++ = '-';
            *logPtr++ = ' ';
            offset += 2;

            if (gLoggerConfig_s.logFuncNameFlag) {
                *logPtr++ = '[';
                length = strlen(pLogInfoNode->u.logMsgInfo.funcName);
                memcpy(logPtr, pLogInfoNode->u.logMsgInfo.funcName, length);
                logPtr += length;
                *logPtr++ = ']';
                *logPtr++ = ',';
                *logPtr++ = ' ';            
                offset = offset + length + 4;
            }     

            if (!pLogInfoNode->u.logMsgInfo.logContentFlag) {
                snprintf(logPtr, remainBufferSize - offset, (const char*)pLogInfoNode->u.logMsgInfo.fmt,  
                    pLogInfoNode->u.logMsgInfo.args[0], pLogInfoNode->u.logMsgInfo.args[1], pLogInfoNode->u.logMsgInfo.args[2],
                    pLogInfoNode->u.logMsgInfo.args[3], pLogInfoNode->u.logMsgInfo.args[4], pLogInfoNode->u.logMsgInfo.args[5]);
                length = strlen(logPtr);
                logPtr += length;
                offset += length;
            } else {
                length = strlen(pLogInfoNode->u.logMsgInfo.logContent);
                memcpy(logPtr, pLogInfoNode->u.logMsgInfo.logContent, length);
                logPtr += length;
                offset += length;
            }

        } else {
            length = strlen(pLogInfoNode->u.logMemInfo.logMem);
            if ((offset + length) >= 4096) {
                QueuePushNodeHead(&gBusyLogInfoQueue, &pLogInfoNode->node);
                break;
            } else {
                memcpy(logPtr, pLogInfoNode->u.logMemInfo.logMem, length);
                offset += length;
                logPtr += length;
            }
        }

        QueuePushNode(&gIdleLogInfoQueue, &pLogInfoNode->node);
        
        count--;

        if ((offset + MAX_TIMESTAMP_LENGTH + 128) >= remainBufferSize) {
            break;
        }
    }

    *logPtr = '\0';
    if (strlen(logBuff) > 0) {
        LoggerOutputLog(logBuff, strlen(logBuff));
    }
}

// --------------------------------------
void LoggerWriteMem(unsigned int logLevel, unsigned char* pBuffer, unsigned int length)
{
    if ((pBuffer != 0) && (length > 0) && (logLevel >= gLoggerConfig_s.logLevel) && (logLevel < E_LOG_LVL_MAX)) {
        int offset = 0;
        unsigned int i;
        char* logPtr;
        int remainBufferSize;

        if (gLoggerConfig_s.logType != SYNC_LOG) {
            if (gLoggerConfig_s.logType == AYNC_LOG_TYPE_1) {
                int fullFlag = 0;

                pthread_mutex_lock(&gLoggerMutex);

                if (gpWriteBuffer->fullFlag) {
                    // gpWriteBuffer points to gpReadBuffer
                    printf("1 No available log buffer, drop this log, gpWriteBuffer = %p\n", gpWriteBuffer);
                    pthread_cond_signal(&gLoggerCondition);
                    pthread_mutex_unlock(&gLoggerMutex);
                    return;
                } else if ((gLoggerConfig_s.logBufferingSize != 0) && (gpWriteBuffer->length != 0) &&
                    ( (3*length + length/16 + gpWriteBuffer->length + 5) >= gLoggerConfig_s.logBufferingSize )) {
                    fullFlag = 1;
                    gpWriteBuffer->fullFlag = 1;
                    gpWriteBuffer = (LogBufferCache*)gpWriteBuffer->next;
                    if (gpWriteBuffer->fullFlag) {
                        printf("2 No available log buffer, drop this log, gpWriteBuffer = %p\n", gpWriteBuffer);
                        pthread_cond_signal(&gLoggerCondition);
                        pthread_mutex_unlock(&gLoggerMutex);
                        return;
                    }
                }

                logPtr = gpWriteBuffer->logData + gpWriteBuffer->length;
                remainBufferSize = gLoggerConfig_s.logBufferingSize - gpWriteBuffer->length;
                for (i=0; i<length; i++) {
                    snprintf(logPtr + offset, remainBufferSize - offset, "%02x ", pBuffer[i]);
                    offset = strlen(logPtr);

                    if (0 == ((i + 1) % 16)) {
                        snprintf(logPtr + offset, remainBufferSize - offset, "\n");
                        offset = strlen(logPtr);
                    }

                    if (offset >= remainBufferSize) {
                        break;
                    }
                }
                snprintf(logPtr + offset, remainBufferSize - offset, "\n");                
                offset = strlen(logPtr);

                gpWriteBuffer->length += offset;
                if (gpWriteBuffer->length >= gLoggerConfig_s.logBufferingSize) {
                    fullFlag = 1;
                    gpWriteBuffer->fullFlag = 1;
                    // printf("%lu, gpWriteBuffer = %p\n", pthread_self(), gpWriteBuffer);
                    gpWriteBuffer = (LogBufferCache*)gpWriteBuffer->next;
                }

                if (fullFlag) {
                    pthread_cond_signal(&gLoggerCondition);
                }

                pthread_mutex_unlock(&gLoggerMutex);
            } else {
                LogInfo* pLogInfoNode = (LogInfo*)QueuePopNode(&gIdleLogInfoQueue);
                if (pLogInfoNode == 0) {   
                    pthread_mutex_lock(&gLoggerMutex);
                    pthread_cond_signal(&gLoggerCondition);
                    pthread_mutex_unlock(&gLoggerMutex); 
                    // printf("no idle log item, drop this log mem\n");    
                    return;
                }

                pLogInfoNode->contentType = LOG_MEM_TYPE;
                remainBufferSize = MAX_LOG_MEMORY_LENGTH;
                logPtr = pLogInfoNode->u.logMemInfo.logMem;
                for (i=0; i<length; i++) {
                    snprintf(logPtr + offset, remainBufferSize - offset, "%02x ", pBuffer[i]);
                    offset = strlen(logPtr);

                    if (0 == ((i + 1) % 16)) {
                        snprintf(logPtr + offset, remainBufferSize - offset, "\n");
                        offset = strlen(logPtr);
                    }

                    if (offset >= remainBufferSize) {
                        break;
                    }
                }
                snprintf(logPtr + offset, remainBufferSize - offset, "\n");    

                QueuePushNode(&gBusyLogInfoQueue, &pLogInfoNode->node);
            }
        } else {
            char logBuff[MAX_LOG_ITEM_LENGTH] = {};
            for (i=0; i<length; i++) {
                snprintf(logBuff + offset, MAX_LOG_ITEM_LENGTH - offset, "%02x ", pBuffer[i]);

                offset = strlen(logBuff);
                if (offset >= MAX_LOG_ITEM_LENGTH) {
                    return;
                }

                if (0 == ((i + 1) % 16)) {
                    snprintf(logBuff + offset, MAX_LOG_ITEM_LENGTH - offset, "\n");
                    offset = strlen(logBuff);
                    if (offset >= MAX_LOG_ITEM_LENGTH) {
                        if ((i + 1) == length) {
                            break;
                        } else {
                            return;
                        }
                    }
                }
            }
            snprintf(logBuff + offset, MAX_LOG_ITEM_LENGTH - offset, "\n");

            LoggerOutputLog(logBuff, strlen(logBuff));
        }
    }
}

// --------------------------------------
static void LoggerOutputLog(char* logBuff, unsigned length)
{
    if (gLoggerConfig_s.logToConsoleFlag) {
        printf("%s", logBuff);
    }

    if (gLoggerConfig_s.logToFileFlag) {
        int numBytesWritten = fwrite(logBuff, 1, length, gLoggerStatus_s.fp);
        if (numBytesWritten > 0) {
            gLoggerStatus_s.logFileSize += numBytesWritten;
            if (gLoggerStatus_s.logFileSize > gLoggerConfig_s.maxLogFileSize) {
                rewind(gLoggerStatus_s.fp);
                gLoggerStatus_s.logFileSize = 0;
            }
            fflush(gLoggerStatus_s.fp);
        }
    }
}



