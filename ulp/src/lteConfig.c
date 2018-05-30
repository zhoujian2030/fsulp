/*
 * lteConfig.c
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#ifdef OS_LINUX

#include <stdio.h>
#include "lteConfig.h"
#include "cJSON.h"
#include "file.h"
#include <string.h>
#include "lteLogger.h"

LteConfig gLteConfig = 
{ 
    5,
    10000,

    {
        TRACE,
        1, 
        0,
        1024*1024*5,
        ""
    }
};

// --------------------------------
int IsStringEqual(char* src, char* dst)
{
    if (src == 0 || dst == 0) {
        return 0;
    }

    int length = strlen(src);
    if (length != strlen(dst)) {
        return 0;
    }

    int i = 0;
    while (i < length) {
        if (src[i] != dst[i]) {
            return 0;
        }
        i++;
    }

    return 1;
}

// --------------------------------
void ParseConfig(char* configFileName)
{
    if (configFileName == 0) {
        return;
    }

    int fd = FileOpen(configFileName, FILE_OPEN, FILE_READ_ONLY);
    if (fd == -1) {
        return;
    }

    int numBytesRead = 0;
    char jsonBuffer[8192];
    int ret;
    cJSON *jsonRoot, *jsonLoggerRoot, *jsonItem;
    
    ret = FileRead(fd, jsonBuffer, 8192, &numBytesRead);
    if (ret != FILE_SUCC || numBytesRead <= 0) {
        return;
    }

    if (numBytesRead == 8192) {
        return;
    }

    jsonRoot = cJSON_Parse(jsonBuffer);
    if (jsonRoot == 0) {
        return;
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "PollingInterval");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.pollingInterval = jsonItem->valueint;
        } else {
            printf("Invalid config for PollingInterval\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ResCleanupTimer");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.resCleanupTimer = jsonItem->valueint;
        } else {
            printf("Invalid config for resCleanupTimer\n");
        }
    }

    jsonLoggerRoot = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LoggerConfig");
    if (jsonLoggerRoot != 0) {
        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonLoggerRoot, "LogLevel");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {
                if (IsStringEqual(jsonItem->valuestring, "TRACE")) {
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_TRACE;
                } else if (IsStringEqual(jsonItem->valuestring, "DEBUG")) {
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_DBG;
                } else if (IsStringEqual(jsonItem->valuestring, "INFO")) {
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_INFO;
                } else if (IsStringEqual(jsonItem->valuestring, "WARNING")) {
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_WARN;
                } else if (IsStringEqual(jsonItem->valuestring, "ERROR")) {
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_ERROR;
                } else {
                    printf("Invalid value for LogLevel: %s\n", jsonItem->valuestring);
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_INFO;
                }
            } else {
                printf("Invalid config for LogLevel\n");
                gLteConfig.logConfig.logLevel = LOG_LEVEL_INFO;
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonLoggerRoot, "LogToConsole");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_True) {                
                gLteConfig.logConfig.logToConsoleFlag = jsonItem->valueint;
            } else {
                printf("Invalid config for LogToConsole\n");
                gLteConfig.logConfig.logToConsoleFlag = 0;
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonLoggerRoot, "LogToFile");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_True) {                
                gLteConfig.logConfig.logToFileFlag = jsonItem->valueint;
            } else {
                printf("Invalid config for LogToFile\n");
                gLteConfig.logConfig.logToFileFlag = 0;
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonLoggerRoot, "MaxLogFileSize");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.logConfig.maxLogFileSize = jsonItem->valueint;
            } else {
                printf("Invalid config for MaxLogFileSize\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonLoggerRoot, "LogFilePath");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {      
                if (strlen(jsonItem->valuestring) < MAX_LOG_FILE_PATH_LENGTH) {
                    strcpy(gLteConfig.logConfig.logFilePath, jsonItem->valuestring);
                }       
            }
        }
    }
}

// -------------------------------
void ShowConfig()
{
    printf("+----------------------------------------+\n");
    printf("pollingInterval:    %d\n", gLteConfig.pollingInterval);
    printf("resCleanupTimer:    %d\n", gLteConfig.resCleanupTimer);
    printf("logLevel:           %d\n", gLteConfig.logConfig.logLevel);
    printf("logToConsoleFlag:   %d\n", gLteConfig.logConfig.logToConsoleFlag);
    printf("logToFileFlag:      %d\n", gLteConfig.logConfig.logToFileFlag);
    printf("maxLogFileSize:     %d\n", gLteConfig.logConfig.maxLogFileSize);
    printf("logFilePath:        %s\n", gLteConfig.logConfig.logFilePath);
    printf("+----------------------------------------+\n");
}

#endif
