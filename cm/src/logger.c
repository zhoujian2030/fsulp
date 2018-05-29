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
    ""
};

static char gLoggerLevelName_s[E_LOG_LVL_MAX][6] = {
        {"TRACE"},
        {"DEBUG"},
        {"INFO "},
        {"WARN "},
        {"ERROR"}
};
#define MAX_LOG_ITEM_LENGTH 4096

static void LoggerOutputLog(char* logBuff);

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
void LoggerUpdateConfig(LoggerConfig* pConfig)
{
    if (pConfig != 0) {
        memcpy((void*)&gLoggerConfig_s, (void*)pConfig, sizeof(LoggerConfig));
        if (gLoggerConfig_s.logToFileFlag) {
            if (gLoggerStatus_s.fp != 0) {
                fclose(gLoggerStatus_s.fp);
            }
            gLoggerStatus_s.logFileSize = 0;
            // truncate(gLoggerConfig_s.logFilePath, 0);
            gLoggerStatus_s.fp = fopen(gLoggerConfig_s.logFilePath, "w+");
            if (gLoggerStatus_s.fp == 0) {
                gLoggerConfig_s.logToFileFlag = 0;
                printf("Fail to create log file : %s\n", gLoggerConfig_s.logFilePath);
            }
        }
    }
}

// -------------------------------------
void LoggerWriteMsg(char* moduleId, unsigned int logLevel, const char *fileName, const char* funcName, const char *fmt,...)
{
    if ((logLevel >= gLoggerConfig_s.logLevel) && (logLevel < E_LOG_LVL_MAX)) {
        char logBuff[MAX_LOG_ITEM_LENGTH] = { };
        int offset = 0;

        struct timeval tv;
        gettimeofday(&tv, 0);

        va_list args;

        strftime(logBuff, MAX_LOG_ITEM_LENGTH, "[%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));
        offset = strlen(logBuff);
        snprintf(logBuff + offset, MAX_LOG_ITEM_LENGTH - offset, ".%03d] [%s] [%3.3s] [%lu] [%s] - [%s], ", (int)tv.tv_usec/1000, 
            gLoggerLevelName_s[logLevel], moduleId, pthread_self(), fileName,funcName);
        offset = strlen(logBuff);

        va_start(args, fmt);
        vsnprintf(logBuff + offset, MAX_LOG_ITEM_LENGTH - offset, fmt, args);
        va_end(args);

        offset = strlen(logBuff);
        if ((offset >= 1) && (logBuff[offset - 1] != '\n')) {
            if (offset < MAX_LOG_ITEM_LENGTH - 1)
			{
				logBuff[offset] = '\n';
				logBuff[offset + 1] = '\0';
			}
			else
			{
				logBuff[offset - 1] = '\n';
				logBuff[offset] = '\0';
			}
        }

        LoggerOutputLog(logBuff);
    }
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

        LoggerOutputLog(logBuff);
    }
}

// --------------------------------------
static void LoggerOutputLog(char* logBuff)
{
    if (gLoggerConfig_s.logToConsoleFlag) {
        printf("%s", logBuff);
    }

    if (gLoggerConfig_s.logToFileFlag) {
        int numBytesWritten = fwrite(logBuff, 1, strlen(logBuff), gLoggerStatus_s.fp);
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
