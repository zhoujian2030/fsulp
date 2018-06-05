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
#include <stdarg.h>
#include "logger.h"
#include <unistd.h>

static LoggerStatus gLoggerStatus_s = {
    0,
    0
};
static LoggerConfig gLoggerConfig_s = {
    TRACE,
    1, 

    0,
    1024*1024*5,    // 5M bytes
    "",

    0,  // default not log module name
    0,  // log file name
    1,  // log function name
    1,  // log thread id

    0,
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
                // gLoggerConfig_s.asyncLoggingFlag = 0;
                printf("Fail to create log file : %s\n", gLoggerConfig_s.logFilePath);
                return;
            }
        }
    }

    InitAsyncLogger();

    if (!gLoggerConfig_s.asyncLoggingFlag) {
        if (pthread_mutex_init(&gLoggerMutex, 0) != 0) {
            printf("Fail to init pthread_mutex_t\n"); 
        }
    }
}

// -------------------------------------
static void InitAsyncLogger()
{
    if (gLoggerConfig_s.asyncLoggingFlag) {
        unsigned int i;
        int ret;
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

        ret = pthread_mutex_init(&gLoggerMutex, 0);
        if (ret == 0) {
            ret = pthread_cond_init(&gLoggerCondition, 0);
            if (ret != 0) {
                pthread_mutex_destroy(&gLoggerMutex);
                gLoggerConfig_s.asyncLoggingFlag = 0;
                printf("Fail to init pthread_cond_t\n"); 
                return;
            }
        } else {
            gLoggerConfig_s.asyncLoggingFlag = 0;
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

    while (1) {
        ret = pthread_mutex_lock(&gLoggerMutex);
        if (ret != 0) {
            printf("Fail to lock on mutex\n");
            break;
        }

        // wait if current log buffer is not full
        while (!gpReadBuffer->fullFlag) {
            ret = pthread_cond_wait(&gLoggerCondition, &gLoggerMutex);
            if (ret != 0) {
                printf("Fail to wait on condition\n");
                pthread_mutex_unlock(&gLoggerMutex);
                break;
            }
        }

        // write to file
        // printf("gpReadBuffer = %p\n", gpReadBuffer);
        LoggerOutputLog(gpReadBuffer->logData, gpReadBuffer->length);
        gpReadBuffer->fullFlag = 0;
        gpReadBuffer->length = 0;
        gpReadBuffer = (LogBufferCache*)gpReadBuffer->next;

        pthread_mutex_unlock(&gLoggerMutex);

        // // sleep 1ms in case too much log
        // if (gpReadBuffer->fullFlag) {
        //     usleep(1000);
        // }
    }

    return 0;
}

// --------------------------------------
#define MAX_TIMESTAMP_LENGTH            32
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
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned int currMilliSecond = tv.tv_usec/1000;

    if ((gPrevSysSecond == tv.tv_sec) && (gPrevSysMilliSecond == currMilliSecond)) {
        // timestamp is not changed
        snprintf(logBuff, MAX_TIMESTAMP_LENGTH + 3, "[%s] ", gCachedTimeStamp);
    } else {    
        // when async logging, the memory is protected outside
        if (!gLoggerConfig_s.asyncLoggingFlag) {
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
        snprintf(logBuff, MAX_TIMESTAMP_LENGTH + 3, "[%s] ", gCachedTimeStamp);

        if (!gLoggerConfig_s.asyncLoggingFlag) {
            pthread_mutex_unlock(&gLoggerMutex);
        }      
    }
}

// -------------------------------------
void LoggerWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt,...)
{
    if ((logLevel >= gLoggerConfig_s.logLevel) && (logLevel < E_LOG_LVL_MAX)) {
        char logBuff[MAX_LOG_ITEM_LENGTH] = { };
        int offset = 0;
        char* logPtr;
        va_list args;

        if (gLoggerConfig_s.asyncLoggingFlag) {
            va_start(args, fmt);
            AsyncWriteMsg(moduleId, logLevel, fileName, funcName, fmt, args);
            va_end(args);
        } else {
            logPtr = logBuff;

            LoggerGetTimestamp(logPtr + offset);
            offset = strlen(logPtr);

            snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "[%s] ", gLoggerLevelName_s[logLevel]);
            offset = strlen(logPtr);

            if (gLoggerConfig_s.logModuleNameFlag) {
                snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "[%3.3s] ", moduleId);
                offset = strlen(logPtr);
            }

            if (gLoggerConfig_s.logThreadIdFlag) {
                snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "[%lu] ", pthread_self());
                offset = strlen(logPtr);
            }
            
            if (gLoggerConfig_s.logFileNameFlag) {
                snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "[%s] ", fileName);
                offset = strlen(logPtr);
            }

            snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "- ");
            offset = strlen(logPtr);

            if (gLoggerConfig_s.logFuncNameFlag) {
                snprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, "[%s], ", funcName);
                offset = strlen(logPtr);
            }     

            va_start(args, fmt);
            vsnprintf(logPtr + offset, MAX_LOG_ITEM_LENGTH - offset, fmt, args);
            va_end(args);

            offset = strlen(logPtr);
            if ((offset >= 1) && (logPtr[offset - 1] != '\n')) {
                if (offset < MAX_LOG_ITEM_LENGTH - 1)
                {
                    logPtr[offset] = '\n';
                    logPtr[offset + 1] = '\0';
                }
                else
                {
                    logPtr[offset - 1] = '\n';
                    logPtr[offset] = '\0';
                }
            }

            LoggerOutputLog(logBuff, strlen(logBuff));
        }
    }
}

// --------------------------------------
static void AsyncWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt, va_list args)
{
    int offset = 0;
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

    LoggerGetTimestamp(logPtr + offset);
    offset = strlen(logPtr);

    snprintf(logPtr + offset, remainBufferSize - offset, "[%s] ", gLoggerLevelName_s[logLevel]);
    offset = strlen(logPtr);

    if (gLoggerConfig_s.logModuleNameFlag) {
        snprintf(logPtr + offset, remainBufferSize - offset, "[%3.3s] ", moduleId);
        offset = strlen(logPtr);
    }

    if (gLoggerConfig_s.logThreadIdFlag) {
        snprintf(logPtr + offset, remainBufferSize - offset, "[%lu] ", pthread_self());
        offset = strlen(logPtr);
    }
    
    if (gLoggerConfig_s.logFileNameFlag) {
        snprintf(logPtr + offset, remainBufferSize - offset, "[%s] ", fileName);
        offset = strlen(logPtr);
    }

    snprintf(logPtr + offset, remainBufferSize - offset, "- ");
    offset = strlen(logPtr);

    if (gLoggerConfig_s.logFuncNameFlag) {
        snprintf(logPtr + offset, remainBufferSize - offset, "[%s], ", funcName);
        offset = strlen(logPtr);
    }     

    vsnprintf(logPtr + offset, remainBufferSize - offset, fmt, args);

    offset = strlen(logPtr);
    if ((offset >= 1) && (logPtr[offset - 1] != '\n')) {
        if (offset < MAX_LOG_ITEM_LENGTH - 1)
        {
            logPtr[offset] = '\n';
            logPtr[offset + 1] = '\0';
        }
        else
        {
            logPtr[offset - 1] = '\n';
            logPtr[offset] = '\0';
        }
    }

    gpWriteBuffer->length += strlen(logPtr);
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
}

// --------------------------------------
void LoggerWriteMem(unsigned int logLevel, unsigned char* pBuffer, unsigned int length)
{
    if ((pBuffer != 0) && (length > 0) && (logLevel >= gLoggerConfig_s.logLevel) && (logLevel < E_LOG_LVL_MAX)) {
        char logBuff[MAX_LOG_ITEM_LENGTH] = {};
        int offset = 0;
        unsigned int i;
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



