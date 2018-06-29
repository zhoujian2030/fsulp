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
#include "loggerConfig.h"

LteConfig gLteConfig = 
{ 
    5,      // 5ms
    10000,  // 10000ms
    1000,   // 1000ms
    "127.0.0.1",
    55012,
    1, 
#ifdef DPE
    0,
    "127.0.0.1",
    3737,
#endif

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
        "",
        0,
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
        printf("cJSON_Parse error\n");
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

#ifdef DPE
    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ReportToDpe");
    if (jsonItem != 0) {
        if (cJSON_IsBool(jsonItem)) {                
            gLteConfig.reportToDpeFlag = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for ReportToDpe\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "DpeIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            if (strlen(jsonItem->valuestring) < MAX_IP_ADDR_LENGTH) {
                strcpy(gLteConfig.dpeIp, jsonItem->valuestring);
            }       
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for DpeIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "DpeUdpPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.dpeUdpPort = jsonItem->valueint;
        } else {
            LOG_WARN(ULP_LOGGER_NAME, "Invalid config for DpeUdpPort\n");
        }
    }
#endif

    jsonRoot2 = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LoggerConfig");
    if (jsonRoot2 != 0) {
        LoggerParseConfig(jsonRoot2, &gLteConfig.logConfig);
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
                    strcpy(gLteConfig.kpiConfig.kpiFileName, jsonItem->valuestring);
                }  
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for KpiFileName\n");
            }
        }

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "KpiDetailFilePath");
        if (jsonItem != 0) {
            if (jsonItem->type == cJSON_String) {               
                if (strlen(jsonItem->valuestring) < MAX_KPI_FILE_NAME_LENGTH) {
                    strcpy(gLteConfig.kpiConfig.detailFilePath, jsonItem->valuestring);
                }  
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for KpiDetailFilePath\n");
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

        jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot2, "ReportDebugInfo");
        if (jsonItem != 0) {
            if (cJSON_IsBool(jsonItem)) {                
                gLteConfig.kpiConfig.reportDebugInfoFlag = jsonItem->valueint;
            } else {
                LOG_WARN(ULP_LOGGER_NAME, "Invalid config for ReportDebugInfo\n");
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
#ifdef DPE
    printf("reportToDpeFlag     :   %d\n", gLteConfig.reportToDpeFlag);
    printf("dpeIp:                  %s\n", gLteConfig.dpeIp);
    printf("dpeUdpPort:             %d\n", gLteConfig.dpeUdpPort);
#endif
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
    printf("kpiFileName:            %s\n", gLteConfig.kpiConfig.kpiFileName);
    printf("detailFilePath:         %s\n", gLteConfig.kpiConfig.detailFilePath);
    printf("udpPort:                %d\n", gLteConfig.kpiConfig.udpPort);
    printf("reportDebugInfoFlag:    %d\n", gLteConfig.kpiConfig.reportDebugInfoFlag);
    printf("+----------------------------------------+\n");
}

#endif
