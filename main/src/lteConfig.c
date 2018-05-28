/*
 * lteConfig.c
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#include <stdio.h>
#include "lteConfig.h"
#include "cJSON.h"
#include "file.h"
#include <string.h>
#include "lteLogger.h"

LteConfig gLteConfig = 
{
    1, 
    5000
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
void ConfigUpdate(char* configFileName)
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
    cJSON *jsonRoot, *jsonItem;
    
    ret = FileRead(fd, jsonBuffer, 8192, &numBytesRead);
    if (ret != FILE_SUCC || numBytesRead <= 0) {
        return;
    }

    jsonRoot = cJSON_Parse(jsonBuffer);
    if (jsonRoot == 0) {
        return;
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "PollingInterval");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            gLteConfig.pollingInterval = jsonItem->valueint * 1000;
        } else {
            printf("Invalid config for PollingInterval\n");
            gLteConfig.pollingInterval = 5000;
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LogLevel");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {
            if (IsStringEqual(jsonItem->valuestring, "TRACE")) {
                gLteConfig.loglevel = LOG_LEVEL_TRACE;
            } else if (IsStringEqual(jsonItem->valuestring, "DEBUG")) {
                gLteConfig.loglevel = LOG_LEVEL_DBG;
            } else if (IsStringEqual(jsonItem->valuestring, "INFO")) {
                gLteConfig.loglevel = LOG_LEVEL_INFO;
            } else if (IsStringEqual(jsonItem->valuestring, "WARNING")) {
                gLteConfig.loglevel = LOG_LEVEL_WARN;
            } else if (IsStringEqual(jsonItem->valuestring, "ERROR")) {
                gLteConfig.loglevel = LOG_LEVEL_ERROR;
            } else {
                printf("Invalid value for LogLevel: %s\n", jsonItem->valuestring);
                gLteConfig.loglevel = LOG_LEVEL_INFO;
            }
        } else {
            printf("Invalid config for LogLevel\n");
            gLteConfig.loglevel = LOG_LEVEL_INFO;
        }
    }
}

// -------------------------------
void ConfigShow()
{
    printf("+----------------------------------------+\n");
    printf("pollingInterval: %d\n", gLteConfig.pollingInterval);
    printf("loglevel: %d\n", gLteConfig.loglevel);
    printf("+----------------------------------------+\n");
}
