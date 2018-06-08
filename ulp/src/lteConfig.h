/*
 * lteConfig.h
 *
 *  Created on: May 28, 2018
 *      Author: j.zh
 */

#ifndef LTE_CONFIG_H
#define LTE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OS_LINUX

#include "logger.h"
#include "lteKpi.h"

#define MAX_IP_ADDR_LENGTH  64
typedef struct {       
    int  pollingInterval;       // in milli second
    int  resCleanupTimer;       // in milli second
    char oamIp[MAX_IP_ADDR_LENGTH];             
    unsigned short oamUdpPort;  //
    LoggerConfig logConfig;     // TRACE, DEBUG, INFO, WARNING, ERROR
    LteKpiConfig kpiConfig;
} LteConfig;


extern LteConfig gLteConfig;

int IsStringEqual(char* src, char* dst);
void ParseConfig(char* configFileName);
void ShowConfig();

#endif

#ifdef __cplusplus
}
#endif

#endif 
