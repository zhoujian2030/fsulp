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
  m_ueDataServerIp("0.0.0.0"),
  m_ueDataServerPort(55010),
  m_engineServerIp("127.0.0.1"),
  m_engineServerPort(6070),
  m_oamServerIp("127.0.0.1"),
  m_oamServerPort(55012),
  m_targetAccTimeInterval(20000),
  m_targetAccTimeMargin(1200)
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

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "UeDataServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_ueDataServerIp = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for UeDataServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "UeDataServerPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_ueDataServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for UeDataServerPort\n");
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

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "OamServerIp");
    if (jsonItem != 0) {
        if (jsonItem->type == cJSON_String) {      
            m_oamServerIp = string(jsonItem->valuestring);     
        } else {
            printf("Invalid config for OamServerIp\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "OamServerPort");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_oamServerPort = jsonItem->valueint;
        } else {
            printf("Invalid config for OamServerPort\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "TargetAccTimeInterval");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_targetAccTimeInterval = jsonItem->valueint;
        } else {
            printf("Invalid config for TargetAccTimeInterval\n");
        }
    }

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "TargetAccTimeMargin");
    if (jsonItem != 0) {
        if ((jsonItem->type == cJSON_Number) && (jsonItem->valueint > 0)) {
            m_targetAccTimeMargin = jsonItem->valueint;
        } else {
            printf("Invalid config for TargetAccTimeMargin\n");
        }
    }

#ifdef COLLECT_IMSI
    printf("m_mobileIdDbName:           %s\n", m_mobileIdDbName.c_str());
    printf("m_userDbname:               %s\n", m_userDbname.c_str());
    printf("EngineServerIp:             %s\n", m_engineServerIp.c_str());
    printf("EngineServerPort:           %d\n", m_engineServerPort);
#endif
    printf("UeDataServerIp:             %s\n", m_ueDataServerIp.c_str());
    printf("UeDataServerPort:           %d\n", m_ueDataServerPort);
    printf("OamServerIp:                %s\n", m_oamServerIp.c_str());
    printf("OamServerPort:              %d\n", m_oamServerPort);
    printf("TargetAccTimeInterval:      %d\n", m_targetAccTimeInterval);
    printf("TargetAccTimeMargin:        %d\n", m_targetAccTimeMargin);

    jsonItem = cJSON_GetObjectItemCaseSensitive(jsonRoot, "LoggerConfig");
    if (jsonItem != 0) {
        LoggerConfig loggerConfig;
        LoggerParseConfig(jsonItem, &loggerConfig);
        printf("logLevel:                   %d\n", loggerConfig.logLevel);
        printf("logType:                    %d\n", loggerConfig.logType);
        printf("logToConsoleFlag:           %d\n", loggerConfig.logToConsoleFlag);
        printf("logToFileFlag:              %d\n", loggerConfig.logToFileFlag);
        printf("maxLogFileSize:             %d\n", loggerConfig.maxLogFileSize);
        printf("logFilePath:                %s\n", loggerConfig.logFilePath);
        printf("logToSocketFlag:            %d\n", loggerConfig.logToSocketFlag);
        printf("logServerIp:                %s\n", loggerConfig.logServerIp);
        printf("logServerPort:              %d\n", loggerConfig.logServerPort);
        printf("logModuleNameFlag:          %d\n", loggerConfig.logModuleNameFlag);
        printf("logFileNameFlag:            %d\n", loggerConfig.logFileNameFlag);
        printf("logFuncNameFlag:            %d\n", loggerConfig.logFuncNameFlag);
        printf("logThreadIdFlag:            %d\n", loggerConfig.logThreadIdFlag);
        printf("asyncWaitTime:              %d\n", loggerConfig.asyncWaitTime);
        printf("logBufferingSize:           %d\n", loggerConfig.logBufferingSize);
        LoggerInit(&loggerConfig);
    }

    FileClose(fd);
}

