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
    5,      // 5ms
    10000,  // 10000ms
    1000,   // 1000ms
    "127.0.0.1",
    55012,
    1, 

    // log config, default log to console only
    {
        TRACE,
        0,  // log type
        1,  // log to console flag

        0,  // log to file flag
        1024*1024*5,    // 5M bytes
        "",

        0,  // log to socket flag
        "127.0.0.1",
        5997,

        0,  // default not log module name
        0,  // log file name
        1,  // log function name
        1,  // log thread id

        5,      // async wait time
        1024*4, // default 4k bytes buffering
    },

    // KPI config, default not report
    {
        0,
        10000,  // 10ms
        "",
        0
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
        printf("FileOpen error\n");
        return;
    }

    int numBytesRead = 0;
    char jsonBuffer[8192];
    int ret;
    cJSON *jsonRoot, *jsonRoot2, *jsonItem;
    
    ret = FileRead(fd, jsonBuffer, 8192, &numBytesRead);
    if (ret != FILE_SUCC || numBytesRead <= 0) {
        printf("FileRead error\n");
        return;
    }

    if (numBytesRead == 8192) {
        printf("numBytesRead = 8192\n");
        return;
    }

    jsonRoot = cJSON_Parse(jsonBuffer);
    if (jsonRoot == 0) {
        printf("cJSON_Parse\n");
        return;
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "PollingInterval");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.pollingInterval = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for PollingInterval\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ResCleanupTimer");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.resCleanupTimer = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for resCleanupTimer\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "UeIdentityWaitTime");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.ueIdentityWaitTime = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for UeIdentityWaitTime\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "OamServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            if (strlen(jsonItem->valuestring) < MAX_IP_ADDR_LENGTH) {
                strcpy(gLteConfig.oamIp, jsonItem->valuestring);
            }       
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for OamServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "OamServerUdpPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.oamUdpPort = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for OamServerUdpPort\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ExplicitInitQmss");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.explicitInitQmssFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for ExplicitInitQmss\n");
            }
        }

    jsonRoot2 = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LoggerConfig");
    if (jsonRoot2 != 0) {
        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogLevel");
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
                    LOG_WARN(ULP_LOGGER_NAME, "Invalid value for LogLevel: %s\n", jsonItem->valuestring);
                    gLteConfig.logConfig.logLevel = LOG_LEVEL_INFO;
                }
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogLevel\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogType");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint >= SYNC_LOG) && (jsonItem->valueint < INVALID_LOG_TYPE)) {                
                gLteConfig.logConfig.logType = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogType\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogToConsole");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logToConsoleFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogToConsole\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogToFile");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logToFileFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogToFile\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "MaxLogFileSize");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.logConfig.maxLogFileSize = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for MaxLogFileSize\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogFilePath");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {      
                if (strlen(jsonItem->valuestring) < MAX_LOG_FILE_PATH_LENGTH) {
                    strcpy(gLteConfig.logConfig.logFilePath, jsonItem->valuestring);
                }       
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogToSocket");
        if (jsonItem != 0) {
            if (cJSON_IsBool(jsonItem)) {                
                gLteConfig.logConfig.logToSocketFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogToSocket\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogServerIp");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {      
                if (strlen(jsonItem->valuestring) < MAX_LOG_SERVER_IP_LENGTH) {
                     strcpy(gLteConfig.logConfig.logServerIp, jsonItem->valuestring);
                }       
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogServerIp\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogServerPort");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.logConfig.logServerPort = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogServerPort\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogModuleName");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logModuleNameFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogModuleName\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogFileName");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logFileNameFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogFileName\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogFuncName");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logFuncNameFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogFuncName\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogThreadId");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_True) || (jsonItem->type == cJSON_False)) {                
                gLteConfig.logConfig.logThreadIdFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogThreadId\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogAsyncWaitTime");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.logConfig.asyncWaitTime = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogAsyncWaitTime\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "LogBufferingSize");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint <= MAX_LOG_BLOCK_SIZE)) {                
                gLteConfig.logConfig.logBufferingSize = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for LogBufferingSize\n");
            }
        }
    }

    jsonRoot2 = cJSON_GetObjectItemCaseSensitive(jsonRoot, "KpiConfig");
    if (jsonRoot2 != 0) {
        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "ReportType");
        if (jsonItem != 0) {
            if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint <= KPI_REPORT_UDP) 
                && (jsonItem->valueint >= KPI_NO_REPORT)) {                
                gLteConfig.kpiConfig.reportType = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for kpi reportType\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "ReportFilePeriod");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.kpiConfig.reportFilePeriod = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for kpi reportFilePeriod\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "KpiFileName");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {               
                if (strlen(jsonItem->valuestring) < MAX_KPI_FILE_NAME_LENGTH) {
                    strcpy(gLteConfig.kpiConfig.fileName, jsonItem->valuestring);
                }  
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for kpi fileName\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "UdpPort");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_Number) {                
                gLteConfig.kpiConfig.udpPort = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for kpi udpPort\n");
            }
        }
    }

    // printf("%s\n", cJSON_Print(jsonRoot));
    cJSON_Delete(jsonRoot);
}

// -------------------------------
void ShowConfig()
{
    printf("+----------------------------------------+\n");
    printf("pollingInterval:        %d\n", gLteConfig.pollingInterval);
    printf("resCleanupTimer:        %d\n", gLteConfig.resCleanupTimer);
    printf("ueIdentityWaitTime:     %d\n", gLteConfig.ueIdentityWaitTime);
    printf("oamIp:                  %s\n", gLteConfig.oamIp);
    printf("oamUdpPort:             %d\n", gLteConfig.oamUdpPort);
    printf("explicitInitQmssFlag:   %d\n", gLteConfig.explicitInitQmssFlag);
    printf("logLevel:               %d\n", gLteConfig.logConfig.logLevel);
    printf("logType:                %d\n", gLteConfig.logConfig.logType);
    printf("logToConsoleFlag:       %d\n", gLteConfig.logConfig.logToConsoleFlag);
    printf("logToFileFlag:          %d\n", gLteConfig.logConfig.logToFileFlag);
    printf("maxLogFileSize:         %d\n", gLteConfig.logConfig.maxLogFileSize);
    printf("logFilePath:            %s\n", gLteConfig.logConfig.logFilePath);
    printf("logToSocketFlag:        %d\n", gLteConfig.logConfig.logToSocketFlag);
    printf("logServerIp:            %s\n", gLteConfig.logConfig.logServerIp);
    printf("logServerPort:          %d\n", gLteConfig.logConfig.logServerPort);
    printf("logModuleNameFlag:      %d\n", gLteConfig.logConfig.logModuleNameFlag);
    printf("logFileNameFlag:        %d\n", gLteConfig.logConfig.logFileNameFlag);
    printf("logFuncNameFlag:        %d\n", gLteConfig.logConfig.logFuncNameFlag);
    printf("logThreadIdFlag:        %d\n", gLteConfig.logConfig.logThreadIdFlag);
    printf("asyncWaitTime:          %d\n", gLteConfig.logConfig.asyncWaitTime);
    printf("logBufferingSize:       %d\n", gLteConfig.logConfig.logBufferingSize);
    printf("reportType:             %d\n", gLteConfig.kpiConfig.reportType);
    printf("reportFilePeriod:       %d\n", gLteConfig.kpiConfig.reportFilePeriod);
    printf("fileName:               %s\n", gLteConfig.kpiConfig.fileName);
    printf("udpPort:                %d\n", gLteConfig.kpiConfig.udpPort);
    printf("+----------------------------------------+\n");
}

#endif
