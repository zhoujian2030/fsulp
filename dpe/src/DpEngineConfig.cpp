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
  m_userDbname("/tmp/eq5_user_info.db"),
  m_localIp("127.0.0.1"),
  m_localUdpServerPort(3737)
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

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LocalIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_localIp = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for LocalIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LocalUdpServerPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_localUdpServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for LocalUdpServerPort\n");
        }
    }

    printf("m_mobileIdDbName:   %s\n", m_mobileIdDbName.c_str());
    printf("m_userDbname:       %s\n", m_userDbname.c_str());
    printf("LocalIp:            %s\n", m_localIp.c_str());
    printf("LocalUdpServerPort: %d\n", m_localUdpServerPort);

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

