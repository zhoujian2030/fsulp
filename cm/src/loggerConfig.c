/*
 * loggerConfig.c
 *
 *  Created on: June 29, 2018
 *      Author: j.zh
 */

#include "loggerConfig.h"
#include <string.h>
#include <stdio.h>

// --------------------------------
static inline int IsStringEqual(char* src, char* dst)
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

// -----------------------------------
void LoggerParseConfig(cJSON* jsonConfig, LoggerConfig* pLoggerConfig)
{
    if (jsonConfig == 0 || pLoggerConfig == 0) {
        return;
    }

    cJSON* jsonItem;
    
    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogLevel");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {            
            if (IsStringEqual(jsonItem->valuestring, "TRACE")) {
                pLoggerConfig->logLevel = TRACE;
            } else if (IsStringEqual(jsonItem->valuestring, "DEBUG")) {
                pLoggerConfig->logLevel = DEBUG;
            } else if (IsStringEqual(jsonItem->valuestring, "INFO")) {
                pLoggerConfig->logLevel = INFO;
            } else if (IsStringEqual(jsonItem->valuestring, "WARNING")) {
                pLoggerConfig->logLevel = WARNING;
            } else if (IsStringEqual(jsonItem->valuestring, "ERROR")) {
                pLoggerConfig->logLevel = ERROR;
            } else {
                printf("Invalid value for LogLevel: %s\n", jsonItem->valuestring);
                pLoggerConfig->logLevel = INFO;
            }
        } else {
            printf("Invalid config for LogLevel\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogType");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint >= SYNC_LOG) && (jsonItem->valueint < INVALID_LOG_TYPE)) {                
            pLoggerConfig->logType = jsonItem->valueint;
        } else {
            printf("Invalid config for LogType\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogToConsole");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logToConsoleFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogToConsole\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogToFile");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logToFileFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogToFile\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "MaxLogFileSize");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_Number) {                
            pLoggerConfig->maxLogFileSize = jsonItem->valueint;
        } else {
            printf("Invalid config for MaxLogFileSize\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogFilePath");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            if (strlen(jsonItem->valuestring) < MAX_LOG_FILE_PATH_LENGTH) {
                strcpy(pLoggerConfig->logFilePath, jsonItem->valuestring);
            }       
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogToSocket");
    if (jsonItem != 0) {
        if (cJSON_IsBool(jsonItem)) {                
            pLoggerConfig->logToSocketFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogToSocket\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            if (strlen(jsonItem->valuestring) < MAX_LOG_SERVER_IP_LENGTH) {
                    strcpy(pLoggerConfig->logServerIp, jsonItem->valuestring);
            }       
        } else {
            printf("Invalid config for LogServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogServerPort");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_Number) {                
            pLoggerConfig->logServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for LogServerPort\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogModuleName");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logModuleNameFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogModuleName\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogFileName");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logFileNameFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogFileName\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogFuncName");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logFuncNameFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogFuncName\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogThreadId");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
            pLoggerConfig->logThreadIdFlag = jsonItem->valueint;
        } else {
            printf("Invalid config for LogThreadId\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogAsyncWaitTime");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_Number) {                
            pLoggerConfig->asyncWaitTime = jsonItem->valueint;
        } else {
            printf("Invalid config for LogAsyncWaitTime\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonConfig, "LogBufferingSize");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint <= MAX_LOG_BLOCK_SIZE)) {                
            pLoggerConfig->logBufferingSize = jsonItem->valueint;
        } else {
            printf("Invalid config for LogBufferingSize\n");
        }
    }    
}
