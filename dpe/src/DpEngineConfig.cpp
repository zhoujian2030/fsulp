/*
 * DpEngineConfig.cpp
 *
 *  Created on: June 28, 2018
 *      Author: j.zhou
 */

#include "DpEngineConfig.h"
#include "logger.h"
#include "cJSON.h"
#include "file.h"

using namespace std;
using namespace dpe;

// -------------------------------
DpEngineConfig::DpEngineConfig() 
: m_mobileIdDbName("/tmp/eq5_mobile_id.db"),
  m_userDbname("/tmp/eq5_user.db"),
  m_imsiServerIp("127.0.0.1"),
  m_imsiServerPort(3737),
  m_engineServerIp("127.0.0.1"),
  m_engineServerPort(6070),
  m_oamServerIp("127.0.0.1"),
  m_oamServerPort(55012)
{
    
}

// -------------------------------
DpEngineConfig::~DpEngineConfig() 
{

}

// -------------------------------
void DpEngineConfig::parseJsonConfig(std::string configFileName)
{
    int fd = FileOpen(configFileName.c_str(), FILE_OPEN, FILE_READ_ONLY);
    if (fd == -1) {
        printf("FileOpen error\n");
        return;
    }
    
    int numBytesRead = 0;
    char jsonBuffer[8192];
    int ret;
    cJSON *jsonRoot, *jsonItem;
    
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

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "MobileIdDBName");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_mobileIdDbName = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for MobileIdDBName\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "UserDBName");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_userDbname = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for UserDBName\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ImsiServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_imsiServerIp = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for ImsiServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "ImsiServerPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_imsiServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for ImsiServerPort\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "EngineServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_engineServerIp = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for EngineServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "EngineServerPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_engineServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for EngineServerPort\n");
        }
    }

    printf("m_mobileIdDbName:   %s\n", m_mobileIdDbName.c_str());
    printf("m_userDbname:       %s\n", m_userDbname.c_str());
    printf("ImsiServerIp:       %s\n", m_imsiServerIp.c_str());
    printf("ImsiServerPort:     %d\n", m_imsiServerPort);
    printf("EngineServerIp:     %s\n", m_engineServerIp.c_str());
    printf("EngineServerPort:   %d\n", m_engineServerPort);

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LoggerConfig");
    if (jsonItem != 0) {
        LoggerConfig loggerConfig;
        LoggerParseConfig(jsonItem, &loggerConfig);
        printf("logLevel:               %d\n", loggerConfig.logLevel);
        printf("logType:                %d\n", loggerConfig.logType);
        printf("logToConsoleFlag:       %d\n", loggerConfig.logToConsoleFlag);
        printf("logToFileFlag:          %d\n", loggerConfig.logToFileFlag);
        printf("maxLogFileSize:         %d\n", loggerConfig.maxLogFileSize);
        printf("logFilePath:            %s\n", loggerConfig.logFilePath);
        printf("logToSocketFlag:        %d\n", loggerConfig.logToSocketFlag);
        printf("logServerIp:            %s\n", loggerConfig.logServerIp);
        printf("logServerPort:          %d\n", loggerConfig.logServerPort);
        printf("logModuleNameFlag:      %d\n", loggerConfig.logModuleNameFlag);
        printf("logFileNameFlag:        %d\n", loggerConfig.logFileNameFlag);
        printf("logFuncNameFlag:        %d\n", loggerConfig.logFuncNameFlag);
        printf("logThreadIdFlag:        %d\n", loggerConfig.logThreadIdFlag);
        printf("asyncWaitTime:          %d\n", loggerConfig.asyncWaitTime);
        printf("logBufferingSize:       %d\n", loggerConfig.logBufferingSize);
        LoggerInit(&loggerConfig);
    }

    FileClose(fd);
}

